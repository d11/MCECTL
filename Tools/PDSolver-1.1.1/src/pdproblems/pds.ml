open Words;;

type control_state = int;;

exception Unknown_control_lookup of control_state;;

type rule_internals = control_state * character * control_state * word;;

type config = (control_state * word)


module OrderedControl =
    struct
        type t = control_state
        let compare = compare
    end;;

module OrderedString = 
    struct 
        type t = string
        let compare = compare
    end;;

module ControlStateNameMap = Map.Make(OrderedControl);;
module ControlStateNumMap = Map.Make(OrderedString);;

let eloise_stuck = 0;;
let abelard_stuck = 1;;

let next_id = ref 2;;
let get_next_id () = 
    let id = !next_id in
    next_id := !next_id + 1;
    id;;

let init_state_name_map = let zero = ControlStateNameMap.add 0 "eloise_stuck" (ControlStateNameMap.empty) in
                          let one = ControlStateNameMap.add 1 "abelard_stuck" zero in
                          one;;
let init_state_num_map = let zero = ControlStateNumMap.add "eloise_stuck" 0 (ControlStateNumMap.empty) in
                         let one = ControlStateNumMap.add "abelard_stuck" 1 zero in
                         one;;


let state_name_map = ref init_state_name_map;;
let state_num_map = ref init_state_num_map;;

let reset_module () = 
    next_id := 2;
    state_num_map := init_state_num_map;
    state_name_map := init_state_name_map;;

let make_control_state name = 
    if ControlStateNumMap.mem name !state_num_map then
        ControlStateNumMap.find name !state_num_map 
    else (
        let id = get_next_id () in
        state_num_map := ControlStateNumMap.add name id !state_num_map;
        state_name_map := ControlStateNameMap.add id name !state_name_map;
        id
    )

let control_state_to_string cs =
    if ControlStateNameMap.mem cs !state_name_map then
        ControlStateNameMap.find cs !state_name_map
    else
        raise (Unknown_control_lookup(cs));;




let control_states_equal p p' = (compare p p' = 0)

module OrderedControlState = 
    struct 
        type t = control_state
        let compare = compare
        let to_string = control_state_to_string
    end;;

module ControlStateSet = Extset.Make(OrderedControlState);;


module OrderedHead =
    struct 
        type t = control_state * character
        let compare = compare
        let to_string (c, a) = ("(" ^ (control_state_to_string c) ^ "," 
                                    ^ (character_to_string a) ^ ")")
    end;;

module HeadSet = Extset.Make(OrderedHead);;


type headset_filter = HeadSet.elt -> bool;;



type rule = control_state * character * control_state * word;;

let make_rule p a p' w = (p, a, p', w);;

let rule_p (p, _, _, _) = p;;

let rule_a (_, a, _, _) = a;;

let rule_p' (_, _, p', _) = p';;

let rule_w (_, _, _, w) = w;;

let rule_tuple r = r 

let rule_to_string (p, a, p', w) = 
    let word_list = word_to_list w in
    let word_string = String.concat " " (List.map character_to_string word_list) in
    String.concat "" [control_state_to_string p; " "; 
                      character_to_string a; " -> "; 
                      control_state_to_string p'; 
                      " ("; word_string; ")"] 

let rule_compare r1 r2 = compare r1 r2;;

module OrderedState = 
    struct 
        type t = control_state
        let compare = compare
    end;;

module OrderedChar = 
    struct
        type t = character
        let compare = compare
    end;;

module StateMap = Map.Make(OrderedState);;
module CharMap = Map.Make(OrderedChar);;

module OrderedNext = 
    struct
        type t = control_state * word
        let compare = compare
        let to_string (p, w) = "(" ^ (control_state_to_string p) ^ ", " ^ 
                                     (word_to_string w) ^ ")"
    end;;

module NextSet = Extset.Make(OrderedNext);;

module OrderedRule = 
    struct
        type t = rule
        let compare r r' = rule_compare r r'
        let to_string r = rule_to_string r
    end;;
 
module RuleSet = Extset.Make(OrderedRule);;



let config_to_string (p, w) = 
    (control_state_to_string p) ^ " " ^ (word_to_string w)


class pds_stats (ncontrols : int) (nchars : int) (ntrans : int) =
    object
        method get_ncontrols = ncontrols
        method get_nchars = nchars
        method get_ntrans = ntrans

        method to_string = 
            "Controls: " ^ (string_of_int ncontrols) ^ "  " ^
            "Characters: " ^ (string_of_int nchars) ^ "  " ^
            "Transitions: " ^ (string_of_int ntrans) ^ "\n"

        method to_row_string = 
            (string_of_int ncontrols) ^ " " ^
            (string_of_int nchars) ^ " " ^
            (string_of_int ntrans)


    end;;


let pds_min_alphabet = CharacterSet.singleton sbot;;

(* next_map * pre_pop_map * pre_rew_map * pre_push_map *)
type pds_internals = ((NextSet.t CharMap.t) StateMap.t) * 
                     (HeadSet.t StateMap.t) * 
                     ((HeadSet.t CharMap.t) StateMap.t) *
                     (((HeadSet.t CharMap.t) CharMap.t) StateMap.t);;
class pds =
    object (self)
        val mutable next_map = StateMap.empty
        val mutable pre_pop_map = StateMap.empty
        val mutable pre_rew_map = StateMap.empty
        val mutable pre_push_map = StateMap.empty
        val mutable pre_push_char_map = CharMap.empty
        val mutable control_states = ControlStateSet.empty
        val mutable alphabet = pds_min_alphabet

        method add_rule rule = 
            let (p, a, p', w) = rule_tuple rule in
            let clist = word_to_list w in
            control_states <- ControlStateSet.add p control_states;
            control_states <- ControlStateSet.add p' control_states;
            alphabet <- CharacterSet.add a alphabet;
            List.iter (fun c -> (alphabet <- CharacterSet.add c alphabet)) clist;
            if StateMap.mem p next_map then
                let char_map = StateMap.find p next_map in
                if CharMap.mem a char_map then
                    let next_set = CharMap.find a char_map in
                    let new_set = NextSet.add (p', w) next_set in
                    let new_char_map = CharMap.add a new_set char_map in
                    next_map <- StateMap.add p new_char_map next_map
                else
                    let new_set = NextSet.singleton (p', w) in
                    let new_char_map = CharMap.add a new_set char_map in
                    next_map <- StateMap.add p new_char_map next_map
            else
                
                let new_set = NextSet.singleton (p', w) in
                let new_char_map = CharMap.add a new_set (CharMap.empty) in
                next_map <- StateMap.add p new_char_map next_map


        method get_next (p : control_state) (a : character) = 
            try
                let next_char_map = StateMap.find p next_map in
                CharMap.find a next_char_map 
            with Not_found ->
                NextSet.empty


        method rules_iter f = 
            let do_rule p a (p', w) = f (make_rule p a p' w) in
            let do_nexts p a r = NextSet.iter (do_rule p a) r in
            let do_cmap p cmap = CharMap.iter (do_nexts p) cmap in
            StateMap.iter do_cmap next_map


        method rules_state_iter p f = 
            let do_rule a (p', w) = f (make_rule p a p' w) in
            let do_nexts a r = NextSet.iter (do_rule a) r in
            if StateMap.mem p next_map then
                let cmap = StateMap.find p next_map in
                CharMap.iter do_nexts cmap


        method rules_heads_iter heads f = 
            let do_rule p a (p', w) = f (make_rule p a p' w) in
            let do_nexts p a r = NextSet.iter (do_rule p a) r in
            let do_head (p, a) = 
                if StateMap.mem p next_map then (
                    let cmap = StateMap.find p next_map in
                    if CharMap.mem a cmap then 
                        do_nexts p a (CharMap.find a cmap)
                ) in
            HeadSet.iter do_head heads



        method rules_fold : 'a. (rule -> 'a -> 'a) -> 'a -> 'a =
            fun f init ->
                let do_rule p a (p', w) res = f (make_rule p a p' w) res in
                let do_char p a rs res = NextSet.fold (do_rule p a) rs res in
                let do_state p cmap res = CharMap.fold (do_char p) cmap res in
                StateMap.fold do_state next_map init
           
        method get_all_rules = 
            let do_rule r res = RuleSet.add r res in
            self#rules_fold do_rule RuleSet.empty

        method get_control_states = control_states

        method get_alphabet = alphabet

        method expand_alphabet chars = 
            alphabet <- CharacterSet.union chars alphabet

        method to_channel c = 
            let do_rule r = 
                output_string c (rule_to_string r);
                output_string c "\n" in 
            self#rules_iter do_rule 


        method to_string =
            let do_rule r s = 
                s ^ (rule_to_string r) ^ "\n" in 
            self#rules_fold do_rule "" 

        method get_stats = 
            new pds_stats (ControlStateSet.cardinal control_states)
                          (CharacterSet.cardinal alphabet)
                          (RuleSet.cardinal (self#get_all_rules))


        method internals = (next_map, pre_pop_map, pre_rew_map, pre_push_map)

        method equal (pds2 : pds) = 
            let (nexts, _, _, _) = pds2#internals in
            let char_map_equal cm1 cm2 = CharMap.equal NextSet.equal cm1 cm2 in
            StateMap.equal char_map_equal next_map nexts


        method self_map (f : rule -> [ `None | `Rule of rule]) =
            control_states <- ControlStateSet.empty; (* clear: rebuilt during map *) 
            alphabet <- pds_min_alphabet;
            let old_next_map = next_map in
            next_map <- StateMap.empty;
            let do_next p a (p', w) = 
                match (f (make_rule p a p' w)) with
                  `None -> ()
                | `Rule(r') -> self#add_rule r' in
            let do_char p a nexts = 
                NextSet.iter (do_next p a) nexts in
            let do_state p cmap = 
                CharMap.iter (do_char p) cmap in
            StateMap.iter do_state old_next_map



        method build_backwards_maps = 
            pre_pop_map <- StateMap.empty;
            pre_rew_map <- StateMap.empty;
            pre_push_map <- StateMap.empty;

            let add_pop_rule r =
                let (p, a, p', w) = rule_tuple r in
                if StateMap.mem p' pre_pop_map then (
                    let heads = StateMap.find p' pre_pop_map in
                    let new_heads = HeadSet.add (p, a) heads in
                    pre_pop_map <- StateMap.add p' new_heads pre_pop_map
                ) else (
                    let heads = HeadSet.singleton (p, a) in
                    pre_pop_map <- StateMap.add p' heads pre_pop_map
                ) in

             let add_rew_rule r =
                let (p, a, p', w) = rule_tuple r in
                let b = List.hd (word_to_list w) in
                let update_c_map b new_head c_map = 
                    if CharMap.mem b c_map then (
                        let heads = CharMap.find b c_map in
                        let new_heads = HeadSet.add new_head heads in
                        CharMap.add b new_heads c_map
                    ) else (
                        let heads = HeadSet.singleton new_head in
                        CharMap.add b heads c_map
                    ) in
                let update_state_map p' b new_head state_map =
                    if StateMap.mem p' state_map then (
                        let c_map = StateMap.find p' state_map in
                        let new_c_map = update_c_map b new_head c_map in
                        StateMap.add p' new_c_map state_map
                    ) else (
                        let c_map = update_c_map b new_head (CharMap.empty) in
                        StateMap.add p' c_map state_map
                    ) in
                pre_rew_map <- update_state_map p' b (p, a) pre_rew_map in

             let add_push_rule r =
                let (p, a, p', w) = rule_tuple r in
                let b = List.hd (word_to_list w) in
                let c = List.hd (List.tl (word_to_list w)) in

                let update_c_map2 c new_head c_map2 = 
                    if CharMap.mem c c_map2 then (
                        let heads = CharMap.find c c_map2 in
                        let new_heads = HeadSet.add new_head heads in
                        CharMap.add c new_heads c_map2
                    ) else (
                        let heads = HeadSet.singleton new_head in
                        CharMap.add c heads c_map2
                    ) in
                let update_c_map1 b c new_head c_map1 = 
                    if CharMap.mem b c_map1 then (
                        let c_map2 = CharMap.find b c_map1 in
                        let new_c_map2 = update_c_map2 c new_head c_map2 in
                        CharMap.add b new_c_map2 c_map1
                    ) else (
                        let new_c_map2 = update_c_map2 c new_head CharMap.empty in
                        CharMap.add b new_c_map2 c_map1
                    ) in
                let update_state_map p' b c new_head state_map = 
                    if StateMap.mem p' state_map then (
                        let c_map1 = StateMap.find p' state_map in
                        let new_c_map1 = update_c_map1 b c new_head c_map1 in
                        StateMap.add p' new_c_map1 state_map
                    ) else (
                        let new_c_map1 = update_c_map1 b c new_head (CharMap.empty) in
                        StateMap.add p' new_c_map1 state_map
                    ) in
                pre_push_map <- update_state_map p' b c (p, a) pre_push_map in

            let do_rule r = 
              let w_len = List.length (word_to_list (rule_w r)) in
              match w_len with
                0 -> add_pop_rule r
              | 1 -> add_rew_rule r
              | 2 -> add_push_rule r
              | _ -> failwith ("pds#build_backwards_maps failed on rule " ^ 
                               (rule_to_string r) ^ 
                               " -- rule pushes too many characters (max = 2).") in

            self#rules_iter do_rule

          
        method get_pre_pop_filter p f = 
            try
                let hs = StateMap.find p pre_pop_map in
                HeadSet.filter f hs
            with Not_found ->
                HeadSet.empty


        method get_pre_rew_filter p a f = 
            try
                let c_map = StateMap.find p pre_rew_map in
                let hs = CharMap.find a c_map in
                HeadSet.filter f hs
            with Not_found ->
                HeadSet.empty



        method get_pre_push_filter p a b f = 
            try
                let c_map1 = StateMap.find p pre_push_map in
                let c_map2 = CharMap.find a c_map1 in
                let hs = CharMap.find b c_map2 in
                HeadSet.filter f hs
            with Not_found ->
                HeadSet.empty


        method pre_pop_iter_filter f filter = 
            let do_f p hs = f p (HeadSet.filter filter hs) in
            StateMap.iter do_f pre_pop_map


        method pre_rew_iter_filter f filter = 
            let do_f p a hs = f p a (HeadSet.filter filter hs) in
            let do_state p c_map = CharMap.iter (do_f p) c_map in
            StateMap.iter do_state pre_rew_map


        method pre_push_iter_filter f filter = 
            let do_f p a b hs = f p a b (HeadSet.filter filter hs) in
            let do_char2 p a c_map2 = CharMap.iter (do_f p a) c_map2 in
            let do_char1 p c_map1 = CharMap.iter (do_char2 p) c_map1 in
            StateMap.iter do_char1 pre_push_map
            

        method has_pre_push_filter p a filter = 
            try
                let c_map1 = StateMap.find p pre_push_map in
                let c_map2 = CharMap.find a c_map1 in
                let do_heads _ heads res = (res || (HeadSet.exists filter heads)) in
                CharMap.fold do_heads c_map2 false
            with Not_found ->
                false

        method pre_push_chars_filter p a filter = 
            try
                let c_map1 = StateMap.find p pre_push_map in
                let c_map2 = CharMap.find a c_map1 in
                let do_entry b heads res = if HeadSet.exists filter heads then
                                               CharacterSet.add b res 
                                           else 
                                               res in
                CharMap.fold do_entry c_map2 CharacterSet.empty
            with Not_found ->
                CharacterSet.empty
 
    end;;














