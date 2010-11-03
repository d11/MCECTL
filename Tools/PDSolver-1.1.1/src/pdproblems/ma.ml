
open Words;;
open Pds;;
open State;;


let char_order = 10;;
let state_order = 1000;;


module type S =
    sig
        type state

        module MaState : State.S with type state = state

        type hyper_transition 

        type ma_internals

        val make_ht : MaState.state -> character -> MaState.SetOfStateSets.t -> hyper_transition 
        val make_ht_list : MaState.state -> character -> MaState.state list list -> hyper_transition
        val ht_q : hyper_transition -> MaState.state
        val ht_a : hyper_transition -> character
        val ht_ss : hyper_transition -> MaState.SetOfStateSets.t
        val ht_tuple : hyper_transition -> MaState.state * character * MaState.SetOfStateSets.t 
        val ht_cardinal : hyper_transition -> int 
        val ht_to_string : hyper_transition -> string 
        val ht_equal : hyper_transition -> hyper_transition -> bool 

        module MaHeadSet : Extset.S with type elt = MaState.state * character


        class ma :
            object
                method num_states : int
                method num_trans : int
                method clone : ma 
                method selective_clone : (MaState.state -> bool) -> ma
                method add_hyper_transition : hyper_transition -> unit
                method add_hyper_transition_and_test : hyper_transition -> bool
                method get_char_image : MaState.state -> character -> MaState.SetOfStateSets.t
                method char_image_from_hyper : MaState.SetOfStateSets.t -> character -> bool -> MaState.SetOfStateSets.t 
                method get_char_image_from_set : MaState.StateSet.t -> character -> bool -> MaState.SetOfStateSets.t 
                method get_word_image : MaState.state -> word -> bool -> MaState.SetOfStateSets.t
                method to_channel : out_channel -> unit
                method to_channel_dot : out_channel -> unit
                method to_string : string
                method to_dot_string : string
                method get_internals : ma_internals
                method set_internals : ma_internals -> unit
                method set_internals_selective : (MaState.state -> bool) -> ma_internals -> unit
                method equals : ma -> bool
                method has_defaults : MaState.state -> bool
                method get_non_defaults : MaState.state -> CharacterSet.t
                (* returns all heads with a violation of A <= A' with respect to given alphabet (resolves default trans *)
                method less_than_heads : ma -> CharacterSet.t -> MaHeadSet.t
                method less_than : ma -> CharacterSet.t -> bool
                (* takes an image {Q1,...,Qn} and adds q -a-> Q for each Qi -a-> Q *)
                method copy_from : MaState.state -> MaState.SetOfStateSets.t -> bool -> unit 
                method copy_from_and_test : MaState.state -> MaState.SetOfStateSets.t -> bool -> bool 
                (* for each q in stateset, copy trans from ma *)
                method inherit_from : MaState.StateSet.t -> ma -> unit
                method recursive_inherit_from : MaState.StateSet.t -> (MaState.state -> bool) -> ma -> unit
                method recursive_move_from : MaState.StateSet.t -> (MaState.state -> bool) -> ma -> unit
                method remove_head : MaState.state -> character -> unit
                method self_map : (hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition]) -> unit
                method self_map_with_state_filter : (MaState.state -> [ `Remove | `Unchanged | `Iter | `IterRemove ]) -> (hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition | `AddTran of hyper_transition]) -> unit
                method self_iter : (hyper_transition -> unit) -> unit
                method self_iter_by_state : MaState.state -> (hyper_transition -> unit) -> unit
                method tran_fold : 'a. (hyper_transition -> 'a -> 'a) -> 'a -> 'a 
                method set_inits : MaState.StateSet.t -> unit 
                method get_inits : MaState.StateSet.t
                method is_init : MaState.state -> bool
                method set_finals : MaState.StateSet.t -> unit 
                method get_finals : MaState.StateSet.t
                method add_states : MaState.StateSet.t -> unit
                method remove_states : MaState.StateSet.t -> unit
                method delete_state : MaState.state -> unit
                method get_states : MaState.StateSet.t
                method self_map_states : (MaState.state -> [ `None | `State of MaState.state]) -> unit
                method remove_unreachable : unit 
                method get_reachable : MaState.StateSet.t -> MaState.StateSet.t
                method get_accepting_heads : MaHeadSet.t
                method accepting_heads_to_string : string
                method accepting_heads_to_channel : out_channel -> unit
                method accepts : MaState.state -> word -> bool
            end;;
    end;;





module Make =
    functor (BasicS : BasicState) -> 
        struct
            type state = BasicS.t

            module MaState = State.Make(BasicS)

            type hyper_transition = MaState.state * character * MaState.SetOfStateSets.t

            let make_ht q a ss = (q, a, ss);;

            let make_ht_list q a ss_list = 
                (q, a, MaState.setofstatesets_from_list_list ss_list);;

            let ht_q (q, _, _) = q;;

            let ht_a (_, a, _) = a;;

            let ht_ss (_, _, ss) = ss;;

            let ht_tuple t = t;;

            let ht_cardinal (_, _, ss) = 
                MaState.SetOfStateSets.cardinal ss;;

            let ht_to_string (q, a, ss) =
                String.concat "" [MaState.state_to_string q;
                                  " -"; character_to_string a; "-> ";
                                  MaState.SetOfStateSets.to_string ss]

            let ht_equal (q1, a1, ss1) (q2, a2, ss2) = (
                    (MaState.states_equal q1  q2) &&
                    (characters_equal a1 a2) &&
                    (MaState.SetOfStateSets.equal ss1 ss2)
                );;


            module HashState = 
                struct
                    type t = MaState.state
                    let equal = MaState.states_equal
                    let hash = Hashtbl.hash
                    let compare = compare
                end;;

            module HashChar = 
                struct 
                    type t = character
                    let equal = characters_equal
                    let hash = Hashtbl.hash
                end;;


            module MaHead = 
                struct
                    type t = MaState.state * character 
                    let compare = compare
                    let to_string (q, a) = "(" ^ (MaState.state_to_string q) ^ ", " ^
                                                 (character_to_string a) ^ ")"
                end;;


            module MaHeadSet = Extset.Make(MaHead);;
            module StateMap = Map.Make(HashState);;


            (* A Double nested transition look up structure:
             * q -> (a -> hyper_transition) 
             *)
            module StateTransitionTbl = Hashtbl.Make(HashState);;
            module CharTransitionTbl = Hashtbl.Make(HashChar);;

            type ma_internals = (MaState.SetOfStateSets.t CharTransitionTbl.t) StateTransitionTbl.t * 
                                MaState.StateSet.t *
                                CharacterSet.t StateMap.t;;


            class ma =
                object (self)
                    (* Note: we avoid double bindings.  Has implications for equality of ma. *)
                    val mutable transitions = StateTransitionTbl.create state_order 
                    (* a map to sets of characters that have nondefault transitions -- this has to 
                     * be kept manually in sync with transitions *)
                    val mutable non_defs = StateMap.empty
                    val mutable init_states = MaState.StateSet.empty
                    val mutable final_states = MaState.StateSet.empty
                    val mutable states = MaState.StateSet.empty

                    (* because our data are not mutable, copy with do for deep clone *)
                    method clone = 
                        let ma = new ma in
                        ma#set_internals (transitions, states, non_defs);
                        ma

                    method selective_clone filter = 
                        let ma = new ma in
                        ma#set_internals_selective filter (transitions, states, non_defs);
                        ma
                           



                    method tran_fold : 'a. (hyper_transition -> 'a -> 'a) -> 'a -> 'a = 
                        fun f init ->
                            let do_tran q a ss v = f (make_ht q a ss) v in
                            let do_state q tbl v = CharTransitionTbl.fold (do_tran q) tbl v in
                            StateTransitionTbl.fold do_state transitions init 


                    method private tran_iter f = 
                        let do_tran q a ss = f (make_ht q a ss) in
                        let do_state q tbl = CharTransitionTbl.iter (do_tran q) tbl in
                        StateTransitionTbl.iter do_state transitions 



                    method private heads_tran_iter heads f = 
                        MaHeadSet.foreach heads (fun (q, a) ->
                            f q a (self#get_char_image q a)
                        )


                    method to_string =
                        let do_tran t a = 
                            String.concat "" [a; ht_to_string t; "\n"] in
                        let tran_string = self#tran_fold do_tran "" in
                        let num_states = MaState.StateSet.cardinal states in
                        ((string_of_int num_states) ^ " States: " ^ 
                        (MaState.StateSet.to_string states) ^ "\n\n" ^
                        "Initial States: " ^ (MaState.StateSet.to_string init_states) ^ "\n\n" ^
                        "Final States: " ^ (MaState.StateSet.to_string final_states) ^ "\n\n" ^
                        "Transitions:\n\n" ^ tran_string)


                    method to_channel ch =
                        let num_states = MaState.StateSet.cardinal states in
                        output_string ch ((string_of_int num_states) ^ " States: ");
                        output_string ch (MaState.StateSet.to_string states);
                        output_string ch "\n\n";
                        output_string ch "Initial States: ";
                        output_string ch (MaState.StateSet.to_string init_states);
                        output_string ch "\n\n";
                        output_string ch "Final States: ";
                        output_string ch (MaState.StateSet.to_string final_states);
                        output_string ch "\n\n";
                        output_string ch "Transitions:\n\n";
                        let do_tran t = 
                            output_string ch (ht_to_string t);
                            output_string ch "\n" in
                        self#tran_iter do_tran 


                    method num_states = 
                        MaState.StateSet.cardinal states

                    method num_trans =
                        let count_tran t a =  a + (ht_cardinal t) in
                        self#tran_fold count_tran 0 


                    method private make_nondef q a = 
                        if not (characters_equal a cdefault) then (
                            if StateMap.mem q non_defs then
                                let new_chars = CharacterSet.add a (StateMap.find q non_defs) in
                                non_defs <- StateMap.add q new_chars non_defs
                            else
                                non_defs <- StateMap.add q (CharacterSet.singleton a) non_defs
                        )


                    method add_hyper_transition t =
                        let (q, a, ss) = ht_tuple t in

                        let add_nondef image tbl = 
                            let def_image = self#get_char_image q cdefault in
                            let full_image = MaState.SetOfStateSets.union def_image image in
                            if not (MaState.SetOfStateSets.equal full_image def_image) then (
                                self#make_nondef q a;
                                CharTransitionTbl.replace tbl a full_image;
                            ) in

                        if not (MaState.SetOfStateSets.is_empty ss) then
                            if StateTransitionTbl.mem transitions q then 
                                let q_tbl = StateTransitionTbl.find transitions q in
                                if CharTransitionTbl.mem q_tbl a then
                                    let ss_old = CharTransitionTbl.find q_tbl a in
                                    let ss_new = MaState.SetOfStateSets.union ss ss_old in
                                    CharTransitionTbl.replace q_tbl a ss_new
                                else (
                                    add_nondef ss q_tbl;
                                )
                            else (
                                let q_tbl = CharTransitionTbl.create char_order in
                                add_nondef ss q_tbl;
                                StateTransitionTbl.replace transitions q q_tbl;
                            )



                    method add_hyper_transition_and_test t =
                        let (q, a, ss) = ht_tuple t in

                        let add_nondef image tbl = 
                            let def_image = self#get_char_image q cdefault in
                            let full_image = MaState.SetOfStateSets.union def_image image in
                            if not (MaState.SetOfStateSets.equal full_image def_image) then (
                                self#make_nondef q a;
                                CharTransitionTbl.replace tbl a full_image;
                            ) in

                        if not (MaState.SetOfStateSets.is_empty ss) then (
                            if StateTransitionTbl.mem transitions q then (
                                let q_tbl = StateTransitionTbl.find transitions q in
                                if CharTransitionTbl.mem q_tbl a then (
                                    let ss_old = CharTransitionTbl.find q_tbl a in
                                    let ss_new = MaState.SetOfStateSets.union ss ss_old in
                                    let result = MaState.SetOfStateSets.equal ss_old ss_new in
                                    CharTransitionTbl.replace q_tbl a ss_new;
                                    result
                                ) else (
                                    add_nondef ss q_tbl;
                                    false
                                )
                            ) else (
                                let q_tbl = CharTransitionTbl.create char_order in
                                add_nondef ss q_tbl;
                                StateTransitionTbl.replace transitions q q_tbl;
                                false
                            )
                        ) else (
                            true
                        )





                    method get_char_image q a = 
                        try 
                            let q_tbl = StateTransitionTbl.find transitions q in
                            try
                                CharTransitionTbl.find q_tbl a 
                            with Not_found ->
                                CharTransitionTbl.find q_tbl cdefault
                        with Not_found -> (
                            MaState.SetOfStateSets.empty
                        )

                    
                    method get_char_image_from_set s a opt =
                        try 
                            let do_state q ss' = 
                                let image = self#get_char_image q a in
                                if MaState.SetOfStateSets.is_empty image then
                                    raise Not_found;
                                MaState.setofstatesets_prod_union_opt ss' image opt in
                            let init_ss = MaState.SetOfStateSets.singleton MaState.StateSet.empty in
                            MaState.StateSet.fold do_state s init_ss
                        with Not_found ->
                            MaState.SetOfStateSets.empty
                   


                    method char_image_from_hyper ss a opt = 
                        let do_set s ss' = 
                            let char_ss = self#get_char_image_from_set s a opt in
                            MaState.SetOfStateSets.union char_ss ss' in
                        MaState.SetOfStateSets.fold do_set ss MaState.SetOfStateSets.empty


                    method private word_image_from_hyper ss clist opt = 
                        match clist with 
                          [] -> ss
                        | a::rest -> 
                            let ss' = self#char_image_from_hyper ss a opt in
                            self#word_image_from_hyper ss' rest opt


                    method get_word_image q w opt = 
                        let ss = MaState.setofstatesets_from_list_list [[q]] in
                        self#word_image_from_hyper ss (word_to_list w) opt


                    method get_internals = (transitions, states, non_defs)


                    method set_internals (t, s, nds) = 
                        transitions <- StateTransitionTbl.create (StateTransitionTbl.length t);
                        let do_state key tbl = 
                            let new_tbl = CharTransitionTbl.copy tbl in
                            StateTransitionTbl.replace transitions key new_tbl in
                        StateTransitionTbl.iter do_state t;
                        states <- s;
                        non_defs <- nds

                    method private copy_nondefs_from q nds_src = 
                        if StateMap.mem q nds_src then
                            non_defs <- StateMap.add q (StateMap.find q nds_src) non_defs
                            

                    method set_internals_selective filter (t, s, nds) = 
                        transitions <- StateTransitionTbl.create (StateTransitionTbl.length t);
                        non_defs <- StateMap.empty;
                        let do_state key tbl = 
                            if filter key then (
                                let new_tbl = CharTransitionTbl.copy tbl in
                                StateTransitionTbl.replace transitions key new_tbl;
                                self#copy_nondefs_from key nds 
                            ) in
                        StateTransitionTbl.iter do_state t;
                        states <- MaState.StateSet.filter filter s




                    method private equal_tran_tables t1 t2 = 
                        let len1 = StateTransitionTbl.length t1 in
                        let len2 = StateTransitionTbl.length t2 in
                        if not (len1 = len2) then
                            false
                        else
                            try (* Hacky, but use exceptions to get us out early *)
                                (* Since we have the same number of key value pairs, we can get
                                 * away with only one check, assuming no double
                                 * bindings
                                 *)
                                let check_q q q_tbl_1 = 
                                    let q_tbl_2 = StateTransitionTbl.find t2 q in
                                    let len1 = CharTransitionTbl.length q_tbl_1 in
                                    let len2 = CharTransitionTbl.length q_tbl_2 in
                                    if not (len1 = len2) then
                                        raise Not_found
                                    else
                                        let check_a a ss1 =
                                            let ss2 = CharTransitionTbl.find q_tbl_2 a in
                                            if not (MaState.SetOfStateSets.equal ss1 ss2) then
                                                raise Not_found in
                                        CharTransitionTbl.iter check_a q_tbl_1 in
                                StateTransitionTbl.iter check_q t1;
                                true
                            with Not_found -> false
                                    


                    method equals (y : ma) = 
                        let (transitions_y, states_y, _) = y#get_internals in
                        (MaState.StateSet.equal states states_y) &&
                        (self#equal_tran_tables transitions transitions_y)


                    (* returns all chars with a non-default transition from q *)
                    method get_non_defaults q =
                        if StateMap.mem q non_defs then
                            StateMap.find q non_defs 
                        else
                            CharacterSet.empty


                    method has_defaults q = 
                        try 
                            let q_tbl = StateTransitionTbl.find transitions q in
                            CharTransitionTbl.mem q_tbl cdefault
                        with Not_found ->
                            false

                    (* self <~= y with respect to given alphabet (default trans are over this) *)
                    method less_than (y : ma) alphabet =
                        let valid_states = y#get_states in
                        try
                            let do_tran t = 
                                let (q, a, _) = ht_tuple t in
                                if (MaState.StateSet.mem q valid_states) then (
                                    if not (characters_equal a cdefault) then (
                                        let less_than = MaState.hyper_set_less_than (self#get_char_image q a) 
                                                                                    (y#get_char_image q a) in
                                        if not less_than then (
                                            raise Not_found
                                        )
                                    ) else (
                                        (* if we have default transition we have to do some extra work *)
                                        let qchars = self#get_non_defaults q in
                                        let yqchars = y#get_non_defaults q in
                                        let only_y = CharacterSet.diff yqchars qchars in
                                        let default_image = self#get_char_image q cdefault in
                                        CharacterSet.foreach only_y (fun a ->
                                            let less_than = MaState.hyper_set_less_than default_image
                                                                                        (y#get_char_image q a) in
                                            if not less_than then (
                                                raise Not_found
                                            )
                                        );
                                        let num_qchars = CharacterSet.cardinal qchars in
                                        let num_yonly = CharacterSet.cardinal only_y in
                                        let together = num_qchars + num_yonly in
                                        let total = CharacterSet.cardinal alphabet in
                                        let remaining = together < total in
                                        if remaining then (
                                            let y_image = y#get_char_image q cdefault in
                                            let less_than = MaState.hyper_set_less_than default_image
                                                                                        y_image in
                                            if not less_than then (
                                                raise Not_found
                                            )
                                        )
                                    )
                                ) in
                            self#tran_iter do_tran;
                            true
                        with Not_found -> (
                            false
                        )



                    (* self <~= y with respect to given alphabet (default trans are over this) *)
                    method less_than_heads (y : ma) alphabet =
                        let valid_states = y#get_states in
                        let bad_heads = ref MaHeadSet.empty in
                        let do_tran t = 
                            let (q, a, _) = ht_tuple t in
                            if (MaState.StateSet.mem q valid_states) then (
                                let less_than = MaState.hyper_set_less_than (self#get_char_image q a) 
                                                                            (y#get_char_image q a) in
                                if not less_than then (
                                    bad_heads := MaHeadSet.add (q, a) !bad_heads
                                )
                            ) else (
                                (* if we have default transition we have to do some extra work *)
                                let qchars = self#get_non_defaults q in
                                let yqchars = y#get_non_defaults q in
                                let only_y = CharacterSet.diff yqchars qchars in
                                let default_image = self#get_char_image q cdefault in
                                CharacterSet.foreach only_y (fun a ->
                                    let less_than = MaState.hyper_set_less_than default_image
                                                                                (y#get_char_image q a) in
                                    if not less_than then (
                                        bad_heads := MaHeadSet.add (q, a) !bad_heads
                                    )
                                );
                                let num_qchars = CharacterSet.cardinal qchars in
                                let num_yonly = CharacterSet.cardinal only_y in
                                let together = num_qchars + num_yonly in
                                let total = CharacterSet.cardinal alphabet in
                                let remaining = together < total in
                                if remaining then (
                                    let y_image = y#get_char_image q cdefault in
                                    let less_than = MaState.hyper_set_less_than default_image
                                                                                y_image in
                                    if not less_than then 
                                        bad_heads := MaHeadSet.add (q, cdefault) !bad_heads;
                                )
                            ) in
                        self#tran_iter do_tran;
                        !bad_heads




                    method private copy_from_body q image prod_union_opt do_char = 
                        let rec copy_from_loop q q_set remaining_q done_c do_char =
                            if not (MaState.StateSet.is_empty remaining_q) then (
                                let q' = MaState.StateSet.choose remaining_q in
                                let q'_cs = self#get_non_defaults q' in
                                CharacterSet.foreach q'_cs (fun a ->
                                    if not (CharacterSet.mem a done_c) then (
                                        do_char q_set a
                                    )
                                );
                                let rest = MaState.StateSet.remove q' remaining_q in
                                let new_done_c = CharacterSet.union done_c q'_cs in
                                copy_from_loop q q_set rest new_done_c do_char
                            ) in

                        MaState.SetOfStateSets.foreach image (fun q_set ->
                            do_char q_set cdefault;
                            copy_from_loop q q_set q_set CharacterSet.empty do_char;
                        )

                    method copy_from_and_test q image prod_union_opt = 
                        let changed = ref false in
                        let do_char q_set a = 
                            let image_set = self#get_char_image_from_set q_set a prod_union_opt in
                            let existed = self#add_hyper_transition_and_test (make_ht q a image_set) in
                            changed := !changed || (not existed) in
                        self#copy_from_body q image prod_union_opt do_char;
                        not !changed

                    method copy_from q image prod_union_opt = 
                        let do_char q_set a = 
                            let image_set = self#get_char_image_from_set q_set a prod_union_opt in
                            (*let is_emp = MaState.SetOfStateSets.is_empty image_set in
                            let has_def = self#has_defaults q in
                            let def_image = self#get_char_image q cdefault in
                            let empty_def = MaState.SetOfStateSets.is_empty def_image in
                            let is_def = characters_equal a cdefault in
                            let q_string = MaState.state_to_string q in
                            if is_emp && has_def && (not empty_def) && (not is_def) then (
                                print_string ("yeah, problem at " ^ q_string);
                                print_newline ();
                                print_string ("Def image: " ^ (MaState.SetOfStateSets.to_string def_image));
                                print_newline ();
                                MaState.StateSet.foreach q_set (fun q' -> 
                                    let q'_nondefs = self#get_non_defaults q' in
                                    let has_a = CharacterSet.mem a q'_nondefs in
                                    let a_image = self#get_char_image q' a in
                                    let q'def_image = self#get_char_image q' cdefault in
                                    print_string ((MaState.state_to_string q') ^ 
                                                  " has a: " ^ 
                                                  (string_of_bool has_a));
                                    print_newline ();
                                    print_string ("image: " ^ (MaState.SetOfStateSets.to_string a_image));
                                    print_newline ();
                                    print_string ("def image: " ^ (MaState.SetOfStateSets.to_string q'def_image));
                                    print_newline ();
                                    print_string ("a is " ^ (character_to_string a));
                                    print_newline ();
                                )
                            );*)
                            self#add_hyper_transition (make_ht q a image_set) in
                        self#copy_from_body q image prod_union_opt do_char


                    method inherit_from states (src_ma : ma) = 
                        let (src_trans, _, nds) = src_ma#get_internals in
                        let do_state q = 
                            let c_tbl = StateTransitionTbl.find src_trans q in
                            let new_c_tbl = CharTransitionTbl.copy c_tbl in
                            StateTransitionTbl.replace transitions q new_c_tbl;
                            self#copy_nondefs_from q nds in
                        MaState.StateSet.iter do_state states

                            
                     method recursive_inherit_from from_s filter (src_ma : ma) = 
                        let (src_trans, src_states, src_nds) = src_ma#get_internals in
                        let do_state q = 
                            if (filter q) then (
                                if (MaState.StateSet.mem q src_states) then (
                                    states <- MaState.StateSet.add q states;
                                    self#copy_nondefs_from q src_nds;
                                    try 
                                        let c_tbl = StateTransitionTbl.find src_trans q in
                                        let new_c_tbl = CharTransitionTbl.copy c_tbl in
                                        StateTransitionTbl.replace transitions q new_c_tbl;
                                    with Not_found -> ()
                                ) else (
                                    failwith "Recursive_inherit_from tried to inherit from a non-existing state."
                                )
                            ) in
                        let reachable = src_ma#get_reachable from_s in
                        MaState.StateSet.iter do_state reachable


                      method recursive_move_from from_s filter (src_ma : ma) = 
                        let (src_trans, src_states, src_nds) = src_ma#get_internals in
                        let do_state q = 
                            if (filter q) then (
                                if (MaState.StateSet.mem q src_states) then (
                                    states <- MaState.StateSet.add q states;
                                    self#copy_nondefs_from q src_nds;
                                    try 
                                        let c_tbl = StateTransitionTbl.find src_trans q in
                                        StateTransitionTbl.remove src_trans q;
                                        StateTransitionTbl.replace transitions q c_tbl;
                                    with Not_found -> ()
                                ) else (
                                    failwith "Recursive_inherit_from tried to inherit from a non-existing state."
                                )
                            ) in
                        let reachable = src_ma#get_reachable from_s in
                        MaState.StateSet.iter do_state reachable

                        

                    (* Note: removes all transitions q-a->Q, but doesn't prevent
                     * q-default-> coming into play *)
                    method remove_head q a = 
                        if StateMap.mem q non_defs then (
                            let nond = StateMap.find q non_defs in
                            non_defs <- StateMap.add q (CharacterSet.remove a nond) non_defs
                        );
                        try
                            let q_tbl = StateTransitionTbl.find transitions q in
                            CharTransitionTbl.remove q_tbl a;
                            if (CharTransitionTbl.length q_tbl) = 0 then
                                StateTransitionTbl.remove transitions q
                        with Not_found -> ()
                       


                    method self_map_with_state_filter (q_filter : MaState.state -> [ `Remove | `Unchanged | `Iter |`IterRemove ]) 
                                                      (f : hyper_transition -> [ `Remove |  
                                                                                 `Unchanged | 
                                                                                 `NewTran of hyper_transition |
                                                                                 `AddTran of hyper_transition]) =
                        let additions = ref [] in
                        let do_tran q a ss = 
                            match (f (make_ht q a ss)) with
                              `Remove -> self#remove_head q a
                            | `Unchanged -> ()
                            | `NewTran(t') -> (additions := t'::!additions; 
                                               self#remove_head q a) 
                            | `AddTran(t') -> additions := t'::!additions; in
                        let do_state q tbl = 
                            match q_filter q with
                              `Remove -> StateTransitionTbl.remove transitions q
                            | `Unchanged -> ()
                            | `Iter -> CharTransitionTbl.iter (do_tran q) tbl 
                            | `IterRemove -> (CharTransitionTbl.iter (do_tran q) tbl;
                                              StateTransitionTbl.remove transitions q) in
                        StateTransitionTbl.iter do_state transitions;
                        List.iter (fun t -> self#add_hyper_transition t) 
                                  !additions



                    method self_map (f : hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition]) =
                        let additions = ref [] in
                        let do_tran tran =
                            let (q, a, _) = ht_tuple tran in
                            match (f tran) with
                              `Remove -> self#remove_head q a
                            | `Unchanged -> ()
                            | `NewTran(t') -> (additions := t'::!additions; 
                                               self#remove_head q a) in
                        self#tran_iter do_tran;
                        List.iter (fun t -> self#add_hyper_transition t) 
                                  !additions





                    method to_dot_string = 
                        let arrow_string q a q' atts = 
                            let arrow = (q ^ " -> " ^ q' ^ " [label=\"" ^ a ^ "\"") in
                            if not (atts = "") then
                                arrow ^ ", " ^ atts ^ "];\n"
                            else
                                arrow ^ "];\n" in
                        let char_string a = 
                            let s = character_to_string a in
                            if (s = "#") then "_bot_" else s in
                        let set_string s = 
                            let do_node n sofar = (sofar ^ (BasicS.to_dot_string n)) in
                            MaState.StateSet.fold do_node s "" in
                        let int_id q a s = 
                            (BasicS.to_dot_string q) ^ "_" ^ (char_string a) ^ "_" ^ (set_string s) in
                        let do_end_point buf mid_point q' = 
                            Buffer.add_string buf ("    " ^ (arrow_string mid_point "" (BasicS.to_dot_string q') "")) in
                        let do_tran buf q a s = 
                            let size = MaState.StateSet.cardinal s in
                            if size = 0 then
                                () (* Should do something here for full generality *)
                            else if size = 1 then
                                let q' = MaState.StateSet.choose s in
                                Buffer.add_string buf ("    " ^ (arrow_string (BasicS.to_dot_string q) 
                                                                              (character_to_string a) 
                                                                              (BasicS.to_dot_string q') 
                                                                              ""))
                            else (* size > 1 *)
                                let mid_point = int_id q a s in
                                Buffer.add_string buf ("    " ^ mid_point ^ " [height=0,width=0,label=\"\"];\n");
                                Buffer.add_string buf ("    " ^ 
                                                       (BasicS.to_dot_string q) ^ 
                                                       " -> " ^ mid_point ^ 
                                                       " [dir=none, label=\"" ^ (char_string a) ^ "\"];\n");
                                MaState.StateSet.iter (do_end_point buf mid_point) s in
                        let do_hyper_tran buf t = 
                            let (q, a, ss) = ht_tuple t in
                            MaState.SetOfStateSets.iter (do_tran buf q a) ss in
                        let do_state buf s = 
                            if (MaState.StateSet.mem s final_states) then
                                Buffer.add_string buf ("    " ^ 
                                                       (BasicS.to_dot_string s) ^ 
                                                       " [shape=doublecircle," ^
                                                       "label=\"" ^ BasicS.to_string s ^ "\"];\n")
                            else
                                Buffer.add_string buf ("    " ^ 
                                                       (BasicS.to_dot_string s) ^ 
                                                       " [label=\"" ^ BasicS.to_string s ^ "\"];\n") in
                        let buf = Buffer.create 16 in (* Ok, we could be smarter about the initial size. *)
                        Buffer.add_string buf "digraph winning_region {\n";
                        Buffer.add_string buf "    rankdir=LR;\n";
                        Buffer.add_string buf "    subgraph inits { rank=min; ";
                        MaState.StateSet.iter (fun s -> Buffer.add_string buf ((BasicS.to_dot_string s) ^ "; ")) init_states;
                        Buffer.add_string buf " }\n";
                        MaState.StateSet.iter (do_state buf) states;
                        self#tran_iter (do_hyper_tran buf);
                        Buffer.add_string buf "}";
                        Buffer.contents buf



                    method to_channel_dot ch = 
                        let arrow_string q a q' atts = 
                            let arrow = (q ^ " -> " ^ q' ^ " [label=\"" ^ a ^ "\"") in
                            if not (atts = "") then
                                arrow ^ ", " ^ atts ^ "];\n"
                            else
                                arrow ^ "];\n" in
                        let char_string a = 
                            let s = character_to_string a in
                            if (s = "#") then "_bot_" else s in
                        let set_string s = 
                            let do_node n sofar = (sofar ^ (BasicS.to_dot_string n)) in
                            MaState.StateSet.fold do_node s "" in
                        let int_id q a s = 
                            (BasicS.to_dot_string q) ^ "_" ^ (char_string a) ^ "_" ^ (set_string s) in
                        let do_end_point mid_point q' = 
                            output_string ch ("    " ^ (arrow_string mid_point "" (BasicS.to_dot_string q') "")) in
                        let do_tran q a s = 
                            let size = MaState.StateSet.cardinal s in
                            if size = 0 then
                                () (* Should do something here for full generality *)
                            else if size = 1 then
                                let q' = MaState.StateSet.choose s in
                                output_string ch ("    " ^ (arrow_string (BasicS.to_dot_string q) 
                                                  (character_to_string a) 
                                                  (BasicS.to_dot_string q') 
                                                  ""))
                            else (* size > 1 *)
                                let mid_point = int_id q a s in
                                output_string ch ("    " ^ mid_point ^ " [height=0,width=0,label=\"\"];\n");
                                output_string ch ("    " ^ 
                                                  (BasicS.to_dot_string q) ^ 
                                                  " -> " ^ mid_point ^ 
                                                  " [dir=none, label=\"" ^ (char_string a) ^ "\"];\n");
                                MaState.StateSet.iter (do_end_point mid_point) s in
                        let do_hyper_tran t = 
                            let (q, a, ss) = ht_tuple t in
                            MaState.SetOfStateSets.iter (do_tran q a) ss in
                        let do_state s = 
                            if (MaState.StateSet.mem s final_states) then
                                output_string ch ("    " ^ 
                                                  (BasicS.to_dot_string s) ^ 
                                                  " [shape=doublecircle," ^
                                                  "label=\"" ^ BasicS.to_string s ^ "\"];\n")
                            else
                                output_string ch ("    " ^ 
                                                  (BasicS.to_dot_string s) ^ 
                                                  " [label=\"" ^ BasicS.to_string s ^ "\"];\n") in
                        output_string ch "digraph winning_region {\n";
                        output_string ch "    rankdir=LR;\n";
                        output_string ch "    subgraph inits { rank=min; ";
                        MaState.StateSet.iter (fun s -> output_string ch ((BasicS.to_dot_string s) ^ "; ")) init_states;
                        output_string ch " }\n";
                        MaState.StateSet.iter do_state states;
                        self#tran_iter do_hyper_tran;
                        output_string ch "}";




                    method set_inits ss = 
                        init_states <- ss

                    method get_inits = init_states

                    method is_init s = 
                        MaState.StateSet.mem s init_states

                    method set_finals ss = 
                        final_states <- ss

                    method get_finals = final_states


                    method add_states ss = 
                        states <- MaState.StateSet.union states ss

                    method remove_states ss = 
                        states <- MaState.StateSet.diff states ss;
                        MaState.StateSet.foreach ss (fun s ->
                            non_defs <- StateMap.remove s non_defs
                        )

                    method delete_state q = 
                        try
                            states <- MaState.StateSet.remove q states;
                        with Not_found -> ();
                        try
                            StateTransitionTbl.remove transitions q;
                        with Not_found -> ();
                        non_defs <- StateMap.remove q non_defs


                    method get_states = states

                    method self_map_states (f : MaState.state -> [ `None | `State of MaState.state]) =
                        let do_state s new_map = 
                            match (f s) with
                              `None -> new_map
                            | `State(s') -> MaState.StateSet.add s' new_map in
                        states <- MaState.StateSet.fold do_state states MaState.StateSet.empty
                       

                    method remove_unreachable = 
                        let reachable = self#get_reachable init_states in
                        let map_tran t = 
                            if MaState.StateSet.mem (ht_q t) reachable then
                                (`Unchanged)
                            else 
                                (`Remove) in
                        let map_state s = 
                            if MaState.StateSet.mem s reachable then
                                (`State(s))
                            else 
                                (`None) in
                        self#self_map map_tran;
                        self#self_map_states map_state;

                    method get_reachable from_s = 
                        let searched = ref MaState.StateSet.empty in
                        let get_image s = 
                            try
                                let s_tbl = StateTransitionTbl.find transitions s in
                                let do_ss _ ss image = 
                                    let l_image = MaState.SetOfStateSets.fold MaState.StateSet.union 
                                                                              ss
                                                                              MaState.StateSet.empty in
                                    MaState.StateSet.union l_image image in
                                CharTransitionTbl.fold do_ss s_tbl MaState.StateSet.empty
                            with Not_found -> MaState.StateSet.empty in
                        let rec get_reachable s = 
                            if not (MaState.StateSet.mem s !searched) then (
                                searched := MaState.StateSet.add s !searched;
                                let image = get_image s in
                                let deep_image = MaState.StateSet.fold (fun s reached ->
                                    MaState.StateSet.union (get_reachable s) reached
                                ) image MaState.StateSet.empty  in
                                MaState.StateSet.union image deep_image
                            ) else
                                MaState.StateSet.empty in
                        MaState.StateSet.fold (fun s reached ->
                            MaState.StateSet.union (get_reachable s) reached
                        ) from_s from_s 


                    method self_iter f = 
                        self#tran_iter f 


                    method self_iter_by_state q f = 
                        try
                            let do_tran q a ss = f (make_ht q a ss) in
                            let q_tbl = StateTransitionTbl.find transitions q in
                            CharTransitionTbl.iter (do_tran q) q_tbl 
                        with Not_found -> ()
                       

                    (* return a list of all pairs (q, a) such that there exists 
                       an accepting run from initial state q, over a word aw. *)
                    method get_accepting_heads =
                        (* good sets {q1,...,qn} can run to acceptance *)
                        let good_sets = ref (MaState.SetOfStateSets.singleton final_states) in
                        let visited_sets = ref MaState.SetOfStateSets.empty in

                        let is_good s = MaState.SetOfStateSets.exists (MaState.StateSet.subset s) !good_sets in
                        let is_visited s = 
                            let sup_set s1 s2 = MaState.StateSet.subset s2 s1 in
                            MaState.SetOfStateSets.exists (sup_set s) !visited_sets in

                        let add_good s = 
                            (* assume is_good s does not hold *)
                            let not_subsumed s' = not (MaState.StateSet.subset s' s) in
                            let filtered_good = MaState.SetOfStateSets.filter not_subsumed !good_sets in
                            good_sets := MaState.SetOfStateSets.add s filtered_good in

                        let add_visited s = 
                            (* assume is_visited s does not hold *)
                            let not_subsumed s' = not (MaState.StateSet.subset s s') in
                            let filtered_visited = MaState.SetOfStateSets.filter not_subsumed !visited_sets in
                            visited_sets := MaState.SetOfStateSets.add s filtered_visited in


                        let rec s_can_accept s = 
                            if is_good s then
                                true
                            else if is_visited s then
                                false
                            else (
                                add_visited s;
                                (* get all possible next statesets *)
                                let add_non_defs q' nds = 
                                    CharacterSet.union (self#get_non_defaults q') nds in
                                let non_defs = MaState.StateSet.fold add_non_defs s CharacterSet.empty in
                                let add_char_next a next = 
                                    MaState.SetOfStateSets.union (self#get_char_image_from_set s a true)
                                                                 next in
                                let nd_next = CharacterSet.fold add_char_next 
                                                                non_defs 
                                                                MaState.SetOfStateSets.empty in
                                let def_next = self#get_char_image_from_set s cdefault true in
                                let next = MaState.SetOfStateSets.union nd_next def_next in
                                let res = ss_can_accept next in
                                if res then
                                    add_good s;
                                res
                            )
                                    
                        and ss_can_accept ss = 
                            if MaState.SetOfStateSets.exists is_good ss then
                                true
                            else 
                                MaState.SetOfStateSets.exists s_can_accept ss in

                        let acc_heads_q q good_heads = 
                            try
                                let q_tbl = StateTransitionTbl.find transitions q in
                                let do_ss a ss good_heads = 
                                    if ss_can_accept ss then
                                        MaHeadSet.add (q, a) good_heads
                                    else
                                        good_heads in
                                CharTransitionTbl.fold do_ss q_tbl good_heads
                            with Not_found -> good_heads in
                        MaState.StateSet.fold acc_heads_q init_states MaHeadSet.empty
                        
                    method accepting_heads_to_string =
                        MaHeadSet.to_string (self#get_accepting_heads)


                    method accepting_heads_to_channel c =
                        let output_head (q, a) = 
                            output_string c (MaState.state_to_string q);
                            output_string c "  ";
                            output_string c (character_to_string a);
                            output_string c "\n" in
                        let heads = self#get_accepting_heads in
                        MaHeadSet.iter output_head heads


                    method private char_iter f a = 
                        let do_tran q ss = f (make_ht q a ss) in
                        let do_state q tbl = 
                            if CharTransitionTbl.mem tbl a then
                                let ss = CharTransitionTbl.find tbl a in
                                do_tran q ss in
                        StateTransitionTbl.iter do_state transitions 
                      
                    method accepts q w =   
                        let pre_image s a = 
                            (* go through all (q, a, ss), include q if s' in ss subset s *)
                            let pre = ref MaState.StateSet.empty in 
                            let is_good ss s = 
                                let is_sub_s s' = 
                                    MaState.StateSet.subset s' s in
                                MaState.SetOfStateSets.exists is_sub_s ss in
                            let include_q t = 
                                let q = ht_q t in
                                let ss = ht_ss t in
                                if is_good ss s then
                                    pre := MaState.StateSet.add q !pre in
                            self#char_iter include_q a;
                            !pre in
                        let w_rev = List.rev (word_to_list w) in
                        let good_inits = List.fold_left pre_image final_states w_rev in
                        MaState.StateSet.mem q good_inits
                        
                end;;
        end;;




