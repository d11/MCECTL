open Words;;
open Pds;;
open Bp;;
open Three_val_logic;;
open Str;;
open Pds_mucalc;;

open Big_int;;


module OrderedString =
    struct
        type t = string
        let compare = compare
    end;;

(* (varname : string) -> (0 / 1) *)
module VarMap = Map.Make(OrderedString);;
(* (boolean_label : string) -> (pds_label : string) *)
module LabelMap = Map.Make(OrderedString);;
(* (method_name : string -> (first_statement : string) * (args : string list) * (locals : string list) *)
module MethodMap = Map.Make(OrderedString);;


module OrderedProp = 
    struct
        type t = id
        let compare = compare
        let to_string s = s
    end;;

module PropSet = Extset.Make(OrderedProp);;

module OrderedControlProp = 
    struct
        type t = control_state * id list
        let compare = compare
        let to_string (c, ps) = 
            "(" ^ (control_state_to_string c) ^
            "," ^ (String.concat ", " ps) ^
            ")"
    end;;

module ControlPropSet = Extset.Make(OrderedControlProp);;

module OrderedCharProp = 
    struct
        type t = character * id list
        let compare = compare
        let to_string (c, ps) = 
            "(" ^ (character_to_string c) ^
            "," ^ (String.concat ", " ps) ^
            ")"
    end;;

module CharPropSet = Extset.Make(OrderedCharProp);;



class bp_preprocessor =
    object (self)
    
        (* return T, F; has two returns *)
        val mutable max_returns = 0

        val mutable next_label = 0

        method private get_label = 
            let label = ("stmt" ^ (string_of_int next_label)) in
            next_label <- next_label + 1;
            label

        method private new_skip_block = 
            [([self#get_label], Skip)]

        method private preprocess_statement_body body = 
            match body with
              Assign(_, _, _) -> body
            | Skip -> body
            | Goto(_) -> body
            | Return(vals) -> max_returns <- max max_returns (List.length vals);
                              body
            | If(expr, ifs, elses) -> If(expr, 
                                         self#preprocess_statement_block ifs,
                                         self#preprocess_statement_block elses)
            | Assert(_) -> body
            | Assume(_) -> body
            | Call(_, _, _) -> body
            | UnsupportedStatement -> body
            | While(guard, stmts) -> While(guard, 
                                           self#preprocess_statement_block stmts)



        method private preprocess_statement stmt = 
            match stmt with
              ([], body) -> ([self#get_label], self#preprocess_statement_body body)
            | (labs, body) -> (labs, self#preprocess_statement_body body)

        method private preprocess_statement_block stmts = 
            if stmts = [] then
                self#new_skip_block
            else
                List.map self#preprocess_statement stmts 

        method private preprocess_method meth =
            let (ret_t, name, args, locals, enf, abort, stmts) = meth in
            let normed_stmts = self#preprocess_statement_block stmts in
            (ret_t, name, args, locals, enf, abort, normed_stmts)

        method preprocess (bp : boolean_program) = 
            max_returns <- 0;
            let (globals, methods) = bp in
            let normed_meths = List.map self#preprocess_method methods in
            ((globals, normed_meths), max_returns)

    end;;


(* When iterating over all possible rules the type changeable indicates
 * whether a bit may change from one state to the next
 *)
type changeable =  Yes | No | Appearing | Disappearing;;
(* When iterating over all possible variable assignments, use mask to
 * force a particular value, or hide the variable from the iteration
 *)
type mask = Zero | One | Changing | Hidden;;


let changeable_to_string c = 
    match c with
      Yes -> "Y"
    | No -> "N"
    | Appearing -> "A"
    | Disappearing -> "D";;

let change_array_to_string carr = 
    let do_ele s c = 
        (s ^ (changeable_to_string c)) in
    Array.fold_left do_ele "" carr;;

let mask_to_string c = 
    match c with
      Zero -> "0"
    | One -> "1"
    | Changing -> "C"
    | Hidden -> "H";;

let mask_array_to_string carr = 
    let do_ele s c = 
        (s ^ (mask_to_string c)) in
    Array.fold_left do_ele "" carr;;


class bp_pds_builder (bp : boolean_program) (props : [`AllProps | `Props of id list]) = 
    object (self)
        val mutable bp = bp
        val mutable processed = false

        val mutable globals = [||]
        val mutable methods = MethodMap.empty
        val mutable control_val_len = -1

        val mutable pds = new pds

        val the_control = ""

        val mutable next_label = 0
        (* map program labels to translated labels (a bp statement may have many
         * given labels, but only one label in the translated pds
         *)
        val mutable label_map = LabelMap.empty

        (* every time a control state / character is created, record it with a list of propositions
         * it satisfies (props are just variables (that are true) and label
         * (assume unique label)
         * at the end we zip them up into a proposition_map
         *)
        val mutable control_props = ControlPropSet.empty
        val mutable char_props = CharPropSet.empty
        (* relevant props are the propositions we're actually interested in *)
        val relevant_props = match props with
                               `AllProps -> PropSet.empty
                             | `Props(props) -> PropSet.from_list props
       
        method private is_relevant_prop pid =
            (PropSet.is_empty relevant_props || PropSet.mem pid relevant_props)

        method private get_true_relevant_ids vals ids = 
            let props = ref [] in
            Array.iteri (fun i v -> if (v = 1) && (self#is_relevant_prop ids.(i)) then
                                           props := ids.(i)::!props)
                        vals;
            !props


        method private add_control_props cs vals ids = 
            let props = self#get_true_relevant_ids vals ids in
            control_props <- ControlPropSet.add (cs, props) control_props


        method private add_char_props a id vals ids =
            let props = self#get_true_relevant_ids vals ids in
            if self#is_relevant_prop id then
                char_props <- CharPropSet.add (a, id::props) char_props
            else
                char_props <- CharPropSet.add (a, props) char_props
           




        method private preprocess_bp = 
            if not processed then 
                let preprocessor = new bp_preprocessor in
                let (nbp, mr) = preprocessor#preprocess bp in
                bp <- nbp;
                let (gs, _) = bp in
                globals <- Array.of_list gs;
                control_val_len <- (Array.length globals) + mr;
                processed <- true


        method get_preprocessed_bp = 
            self#preprocess_bp;
            bp


        method private get_new_return_handler = 
            self#get_label "RETURN_HANDLER"

        method private get_label prefix = 
            let label = (prefix ^ (string_of_int next_label)) in
            next_label <- next_label + 1;
            label

        method private get_statement_label s meth_name =
            match s with
              ([], _) -> failwith "We're assuming all statements have at least one label."
            | (h::t, _) ->
                    try
                        LabelMap.find h label_map
                    with Not_found ->
                        let add_lab_to_map l =
                            label_map <- LabelMap.add l h label_map in
                        List.iter add_lab_to_map (h::t);
                        h

        method private evaluate_expr expr c_globals c_locals n_globals n_locals =
            let var_lookup id = 
                try
                    try
                        if (VarMap.find id c_locals) = 0 then F else T
                    with Not_found ->
                        if (VarMap.find id c_globals) = 0 then F else T 
                with Not_found -> failwith ("Unfound variable: " ^ id ^ ".") in
            let next_var_lookup id = 
                try
                    try
                        if (VarMap.find id n_locals) = 0 then F else T
                    with Not_found ->
                        if (VarMap.find id n_globals) = 0 then F else T
                with Not_found -> failwith ("Unfound variable: " ^ id ^ ".") in
            let rec eval_bin_expr e1 op e2 = 
                match op with
                  Or -> or3 (eval e1) (eval e2)
                | And -> and3 (eval e1) (eval e2)
                | XOr -> xor3 (eval e1) (eval e2)
                | Equal -> eq3 (eval e1) (eval e2)
                | NEqual -> neq3 (eval e1) (eval e2)
                | Implies -> imp3 (eval e1) (eval e2)
            and eval_un_expr op e = 
                match op with
                  Not -> not3 (eval e)
            and eval_choose_expr e1 e2 = 
                match (eval e1) with
                  T -> T
                | F -> (match (eval e2) with
                          T -> F
                        | _ -> N)
                | N -> N
            and eval expr = 
                match expr with
                  Bin(e1, op, e2) -> eval_bin_expr e1 op e2
                | Ternary(e, eif, eelse) -> ite3 (eval e) (eval eif) (eval eelse)
                | Un(op, e) -> eval_un_expr op e 
                | True -> T
                | False  -> F
                | Id(id) -> var_lookup id
                | AposId(id) -> next_var_lookup id
                | Nondet -> N
                | Choose(e1, e2) -> eval_choose_expr e1 e2 in
            eval expr
 



        method private build_var_map name_array mask val_array =     
            let map = ref VarMap.empty in
            let val_pos = ref 0 in
            for i = 0 to ((Array.length name_array) - 1) do
                if mask.(i) != Hidden then
                    map := VarMap.add name_array.(i) val_array.(!val_pos) !map;
                    val_pos := !val_pos + 1
            done;
            !map


        method private var_map_to_string vm = 
            let do_entry id v s = 
                (s ^ id ^ ": " ^ (self#val_to_string v) ^ "\n") in
            VarMap.fold do_entry vm "" 



        method private val_to_char v = 
            match v with
              0 -> '0'
            | 1 -> '1'
            | _ -> invalid_arg "Bad value passed to bp_pds_builder#val_to_char."

        method private val_to_string v = 
            match v with
              0 -> "0"
            | 1 -> "1"
            | _ -> invalid_arg "Bad value passed to bp_pds_builder#val_to_char."


        method private val_arr_to_string arr =
            String.concat "" (Array.to_list (Array.map self#val_to_string arr)) 


        (* generates all labels of the form x1_..._xn, where xi = 0 or 1 as 
         * assignments to s_array variables calls filter on the VarMap of vars
         * to zeros and ones to check whether it should make it into the vals
         * then, if passed, sends the array of vals and varmaP to (f vals varmap).  If not passed
         * then calls (g vals varmap)
         *
         * mask is an array of either Zero, One, Changing, Hidden which specifies whether the
         * argument with the same position in s_array can be anything (Changing), must
         * be 0, or must be 1, or should be Hidden.
         *)
        method private subset_vals_iter_alt f g filter (s_array : string array) (mask : mask array) = 
            let big_zero = Big_int.big_int_of_int 0 in
            let big_two = Big_int.big_int_of_int 2 in

            (* Do big_int_to_bin_list tail rec since the order of the list isn't that important
             * n is the length of the list to produce 
             *)
            let rec big_int_to_bin_list bi n result = 
                if n = 0 then 
                     result
                else
                    let (q, r) = Big_int.quomod_big_int bi big_two in
                    if Big_int.eq_big_int r big_zero then
                        big_int_to_bin_list q (n-1) (0::result)
                    else
                        big_int_to_bin_list q (n-1) (1::result) in

            (* Count num of ele e in array a *)
            let count_in_array a e =
                let c = ref 0 in
                let do_ele e' =
                    if e = e' then
                        c := !c + 1 in
                Array.iter do_ele a;
                !c in

            (* Count num of ele in array a that are not e *)
            let count_not_in_array a e =
                let c = ref 0 in
                let do_ele e' =
                    if not (e = e') then
                        c := !c + 1 in
                Array.iter do_ele a;
                !c in

(*            let print_bin_list l = 
                print_string "list [";
                let rec yeah l = 
                    match l with
                      [] -> ()
                    | h::t -> print_string (string_of_int h);
                              yeah t in
                yeah l;
                print_string "]";
                print_newline () in
 *)               

            (* Method body *)
            let num_vars = Array.length s_array in
            if (num_vars = 0) then
                if filter VarMap.empty then
                    f (Array.make 0 0) (VarMap.empty)
                else ()
            else
                let len = count_not_in_array mask Hidden in
                let n = count_in_array mask Changing in
                let subset_int = ref zero_big_int in
                let max = power_int_positive_int 2 n in
                while lt_big_int !subset_int max do
                    let bin_list = ref (big_int_to_bin_list !subset_int n []) in
                    let values = Array.make len 0 in
                    let val_pos = ref 0 in
                    for i = 0 to (num_vars - 1) do
                        match mask.(i) with
                          Zero -> values.(!val_pos) <- 0; 
                                  val_pos := !val_pos + 1
                        | One -> values.(!val_pos) <- 1; 
                                 val_pos := !val_pos + 1
                        | Changing -> values.(!val_pos) <- List.hd !bin_list;
                                       bin_list := List.tl !bin_list;
                                       val_pos := !val_pos + 1
                        | Hidden -> ()
                    done;
                    let var_map = self#build_var_map s_array mask values in
                    if filter var_map then (
                        f values var_map
                    ) else (
                        g values var_map
                    );
                    subset_int := succ_big_int !subset_int;
                done

        (* Same as subset_vals_iter_alt with g _ _ = () (i.e. do nothing when filter fails *)
        method private subset_vals_iter f filter (s_array : string array) (mask : mask array) = 
            let g _ _ = () in
            self#subset_vals_iter_alt f g filter s_array mask


        (* takes an array of current variable assignments,
         * and replaces all the changeable ones with Changing , all Appearing
         * ones with Hidden.  This can be used with subset_vals_iter to
         * enumerate all possible assignments to changeable variables
         *)
        method private make_pre_change_mask changeable = 
            let do_ele i e =
                match changeable.(i) with
                  Yes -> Changing
                | No -> Changing
                | Appearing -> Hidden
                | Disappearing -> Changing in
            Array.mapi do_ele changeable

        (* takes an array of current variable assignments,
         * and replaces all the changeable ones with Changing, all Disappearing
         * ones with Hidden.  This can be used with subset_vals_iter to
         * enumerate all possible assignments to changeable variables
         *)
        method private make_post_change_mask curr_vals changeable = 
            let val_pos = ref 0  in
            let len = Array.length changeable in
            let mask = Array.make len Zero in
            for i = 0 to len - 1 do
                 match changeable.(i) with
                  Yes -> mask.(i) <- Changing;
                         val_pos := !val_pos + 1
                | No -> mask.(i) <- if (curr_vals.(!val_pos) = 0) then 
                                         Zero 
                                     else 
                                         One;
                                     val_pos := !val_pos + 1
                | Disappearing -> mask.(i) <- Hidden
                | Appearing -> mask.(i) <- Changing;
                               val_pos := !val_pos + 1
            done;
            mask

        (* assumes len >= length of s *)
        method private pad_string s len padding = 
            let s_len = String.length s in
            let pad_len = len - s_len in
            let padding = String.make pad_len padding in
            s ^ padding

        (* globals is the id array corresponding to the vals *)
        method private make_control id vals globals = 
            (* pad val string with 0s to make all same len 
             * (since 01 just as well represented by 0100, &c. and we 
             * don't want to have both as separate states
             *)
            let val_string = self#val_arr_to_string vals in
            let padded_vals = self#pad_string val_string    
                                              control_val_len
                                              (self#val_to_char 0) in
            let cs = if (String.compare id "") = 0 then
                         make_control_state padded_vals
                     else
                         make_control_state (id ^ "_" ^ padded_vals) in
            self#add_control_props cs vals globals;
            cs


        method private make_character p_counter vals locals = 
            let val_string = self#val_arr_to_string vals in
            let a = make_character (p_counter ^ "_" ^ val_string) in
            self#add_char_props a p_counter vals locals;
            a
            



        (* pa -> pb, with expr as a constraint on local and global vars, apos for next step 
         * g_changeable is an array booleans true if global var i can change in
         * this transition, false otherwise.  Similar for l_changeable 
         *)
        method private add_rew_rules expr p a p' b globals locals g_changeable l_changeable =
            let g_pre_change = self#make_pre_change_mask g_changeable in
            let l_pre_change = self#make_pre_change_mask l_changeable in
            let ok_filter _ = true in 
            let all_filter g_map l_map g'_map l'_map = 
                not ((self#evaluate_expr expr g_map l_map g'_map l'_map) = F) in
            let add_rule g l g' l' _ =  (* ignore l'_map *)
                let rule = make_rule (self#make_control p g globals)
                                    (self#make_character a l locals)
                                    (self#make_control p' g' globals)
                                    (make_word [(self#make_character b l' locals)]) in
                pds#add_rule rule in
            let do_g_l_g' g g_map l l_map g' g'_map = 
                let l_mask = self#make_post_change_mask l l_changeable in
                self#subset_vals_iter (add_rule g l g') 
                                      (all_filter g_map l_map g'_map) 
                                      locals 
                                      l_mask in
            let do_g_l g g_map l l_map = 
                let g_mask = self#make_post_change_mask g g_changeable in
                self#subset_vals_iter (do_g_l_g' g g_map l l_map) ok_filter globals g_mask in
            let do_g g g_map = 
                self#subset_vals_iter (do_g_l g g_map) ok_filter locals l_pre_change in
            self#subset_vals_iter do_g ok_filter globals g_pre_change



        (* pa -> pb, with expr as a constraint on local and global vars, apos for next step 
         * g_changeable is an array booleans true if global var i can change in
         * this transition, false otherwise.  Similar for l_changeable 
         *
         * To make if statements more efficient we have p' a' and p'' a'' as the
         * next states.  The first is for when expr is satisfied, the second for
         * when it is not.  Values of locals and globals do not change.
         *)
        method private add_choice_rew_rules expr p a p' a' p'' a'' globals locals g_changeable l_changeable =
            let g_pre_change = self#make_pre_change_mask g_changeable in
            let l_pre_change = self#make_pre_change_mask l_changeable in
            let ok_filter _ = true in 
            let all_filter g_map l_map g'_map l'_map = 
                not ((self#evaluate_expr expr g_map l_map g'_map l'_map) = F) in
            let add_rule p a p' a' g l g' l' _ =  (* ignore l'_map *)
                let rule = make_rule (self#make_control p g globals)
                                    (self#make_character a l locals)
                                    (self#make_control p' g' globals)
                                    (make_word [(self#make_character a' l' locals)]) in
                pds#add_rule rule in
            let do_g_l_g' g g_map l l_map g' g'_map = 
                let l_mask = self#make_post_change_mask l l_changeable in
                self#subset_vals_iter_alt (add_rule p a p' a' g l g') 
                                          (add_rule p a p'' a'' g l g') 
                                          (all_filter g_map l_map g'_map) 
                                          locals 
                                          l_mask in
            let do_g_l g g_map l l_map = 
                let g_mask = self#make_post_change_mask g g_changeable in
                self#subset_vals_iter (do_g_l_g' g g_map l l_map) ok_filter globals g_mask in
            let do_g g g_map = 
                self#subset_vals_iter (do_g_l g g_map) ok_filter locals l_pre_change in
            self#subset_vals_iter do_g ok_filter globals g_pre_change


        (* pa -> p, with expr as a constraint on local and global vars, apos for next step *)
        method private add_pop_rules expr p a p' globals locals g_changeable =
            let l_len = Array.length locals in
            let g_pre_change = self#make_pre_change_mask g_changeable in
            let l_pre_change = Array.make l_len Changing in
            let ok_filter _ = true in 
            let all_filter g_map l_map g'_map = 
                not ((self#evaluate_expr expr g_map l_map g'_map VarMap.empty) = F) in
            let add_rule g l g' _ =  (* Ignore g'_map *)
                let eword = (make_word []) in
                let rule = make_rule (self#make_control p g globals)
                                    (self#make_character a l locals)
                                    (self#make_control p' g' globals)
                                    eword in
                pds#add_rule rule in
            let do_g_l g g_map l l_map = 
                let g_mask = self#make_post_change_mask g g_changeable in 
                self#subset_vals_iter (add_rule g l) (all_filter g_map l_map) globals g_mask in
            let do_g g g_map = 
                self#subset_vals_iter (do_g_l g g_map) ok_filter locals l_pre_change in
            self#subset_vals_iter do_g ok_filter globals g_pre_change


           
        (* pa -> pbc, with expr as a constraint on local and global vars, apos for next step 
         * next step locals are call locals, return locals are the same as curr
         * locals 
         *)
        method private add_push_rules expr p a p' b c globals locals call_locals g_changeable =
            let l_len = Array.length locals in
            let cl_len = Array.length call_locals in
            let g_pre_change = self#make_pre_change_mask g_changeable in
            let l_all_change = Array.make l_len Changing in
            let cl_all_change = Array.make cl_len Changing in
            let ok_filter _ = true in 
            let all_filter g_map l_map g'_map l'_map = 
                not ((self#evaluate_expr expr g_map l_map g'_map l'_map) = F) in
            let add_rule g l g' l' _ =  (* ignore l'_map *)
                let push_word = make_word [(self#make_character b l' call_locals);
                                           (self#make_character c l locals)] in
                let rule = make_rule (self#make_control p g globals)
                                    (self#make_character a l locals)
                                    (self#make_control p' g' globals)
                                    push_word in
                pds#add_rule rule in
            let do_g_l_g' g g_map l l_map g' g'_map = 
                self#subset_vals_iter (add_rule g l g') 
                                      (all_filter g_map l_map g'_map) 
                                      call_locals 
                                      cl_all_change in
            let do_g_l g g_map l l_map = 
                let g_mask = self#make_post_change_mask g g_changeable in
                self#subset_vals_iter (do_g_l_g' g g_map l l_map) ok_filter globals g_mask in
            let do_g g g_map = 
                self#subset_vals_iter (do_g_l g g_map) ok_filter locals l_all_change in
            self#subset_vals_iter do_g ok_filter globals g_pre_change



        method private zip_lists l1 l2 =
            let rec tr_zip l1 l2 res = 
                match (l1, l2) with
                  ([], []) -> res
                | (h1::t1, h2::t2) -> tr_zip t1 t2 ((h1, h2)::res)
                | _ -> failwith "Attempt in bp_pds_build to zip lists of mismatched length." in
            tr_zip l1 l2 []


        (* Assumes arrays of same length and non-empty *)
        method private zip_arrays a1 a2 = 
            let make_pair i e = (e, a2.(i)) in
            let res = Array.mapi make_pair a1 in
            res


        method private assign_expr id e = 
            Bin(AposId(id), Equal, e)


        method private make_returns n = 
            let make_var i = "_r" ^ (string_of_int i) in
            Array.init n make_var

           
        (* Goto labels may only be "logical" until full translation is done
         * so we make a temporary rule with the destination in brackets,
         * then replace them with concrete labels at the end.
         * Bit hacky, but that's the way this file has gone.
         *)
        method private make_temp_label l = 
            "[[" ^ l ^ "]]"

        (* Relabels all rules in pds with resolved temporary labels.
         * Assumes no more than one temp label per stack character. 
         *)
        method private relabel_pds =
            let relabel_string s = 
                let reg_match = regexp "\\[\\[\\(.*\\)\\]\\]\\(.*\\)" in
                if string_match reg_match s 0 then
                    let temp_lab = matched_group 1 s in
                    let rest_lab = matched_group 2 s in
                    try
                        let resolution = LabelMap.find temp_lab label_map in
                        global_replace reg_match s (resolution ^ rest_lab)
                    with Not_found ->
                        failwith ("Unresolved call/goto label: " ^ temp_lab ^ ".")
                else
                    s in
            let relabel c = 
                let c'_string = relabel_string (character_to_string c) in
                make_character c'_string in
            let do_rule r = 
                let (p, a, p', w) = rule_tuple r in
                let new_w = word_map relabel w in
                let new_a = relabel a in
                (`Rule(make_rule p new_a p' new_w)) in
            let do_prop (a, ps) = 
                (relabel a, List.map relabel_string ps) in
            pds#self_map do_rule;
            char_props <- CharPropSet.map do_prop char_props
                




        (* ids := vals contraint cons *)
        method private do_assign stmt_label locals next_stmt_label ids vals cons =
            let changeable var = 
                if List.exists ((=) var) ids then
                    Yes
                else
                    No in
            let l_changeable = Array.map changeable locals in
            let g_changeable = Array.map changeable globals in

            let do_single_assign_expr rest (id, val_expr) = 
                Bin(self#assign_expr id val_expr,
                    And,
                    rest) in
            let paired_assigns = self#zip_lists ids vals in
            let assign_expr = List.fold_left do_single_assign_expr True paired_assigns in

            self#add_rew_rules assign_expr 
                               the_control 
                               stmt_label
                               the_control 
                               next_stmt_label
                               globals
                               locals
                               g_changeable
                               l_changeable;


        (* Guarded skip (assert) *)
        method private do_skip stmt_label locals next_stmt_label guard =
            let changeable _ =  No in
            let l_changeable = Array.map changeable locals in
            let g_changeable = Array.map changeable globals in
            let guard = True in
            self#add_rew_rules guard
                               the_control 
                               stmt_label
                               the_control 
                               next_stmt_label
                               globals
                               locals
                               g_changeable
                               l_changeable;



        method private do_goto stmt_label locals next_stmt_label ids = 
            let changeable _ =  No in
            let l_changeable = Array.map changeable locals in
            let g_changeable = Array.map changeable globals in
            let guard = True in
            let do_dest lab = 
                self#add_rew_rules guard
                                   the_control 
                                   stmt_label
                                   the_control 
                                   (self#make_temp_label lab)
                                   globals
                                   locals
                                   g_changeable
                                   l_changeable in
            List.iter do_dest ids

            
        (* A return e1,...,en creates the pop rule
         *    g l -> g_r1...rn 
         * where r1...rn are assigned the vals of e1...en 
         *)
        method private do_return stmt_label locals next_stmt_label es = 
            let es_arr = Array.of_list es in
            let g_len = (Array.length globals) in
            let r_len = (Array.length es_arr) in
            let return_vars = self#make_returns r_len in
            let all_gs = Array.append globals return_vars in
            let g_change_mask = (Array.make g_len No) in
            let r_pop_change_mask = (Array.make r_len Appearing) in
            let gr_pop_change_mask = Array.append g_change_mask r_pop_change_mask in
            let do_single_assign_expr rest (id, val_expr) = 
                Bin(self#assign_expr id val_expr,
                    And,
                    rest) in
            let paired_assigns = self#zip_arrays return_vars es_arr in
            let assign_expr = Array.fold_left do_single_assign_expr True paired_assigns in

            self#add_pop_rules assign_expr 
                               the_control 
                               stmt_label
                               the_control 
                               all_gs
                               locals
                               gr_pop_change_mask

                   
                    
                

        (* Assumes no non-empty branches (preprocessed) *)
        method private do_if meth_name stmt_label locals next_stmt_label if_exp ifs elses = 
            let changeable _ =  No in
            let l_changeable = Array.map changeable locals in
            let g_changeable = Array.map changeable globals in
            let ifs_head_lab = self#get_statement_label (List.hd ifs) meth_name in
            let elses_head_lab = self#get_statement_label (List.hd elses) meth_name in
            self#add_choice_rew_rules if_exp
                                      the_control 
                                      stmt_label
                                      the_control 
                                      ifs_head_lab
                                      the_control
                                      elses_head_lab
                                      globals
                                      locals
                                      g_changeable
                                      l_changeable;
            self#process_statement_block meth_name locals ifs next_stmt_label;
            self#process_statement_block meth_name locals elses next_stmt_label





        method private do_assume stmt_label locals next_stmt_label expr = 
            let changeable _ =  No in
            let l_changeable = Array.map changeable locals in
            let g_changeable = Array.map changeable globals in
            self#add_choice_rew_rules expr
                                      the_control 
                                      stmt_label
                                      the_control 
                                      next_stmt_label
                                      the_control
                                      "HappyPlace"
                                      globals
                                      locals
                                      g_changeable
                                      l_changeable




        method private do_call stmt_label locals next_stmt_label returns name args =  
            (* Do call *)
            let (start_lab, m_args, m_locals) = self#get_method_info name in
            let changeable _ = No in
            let g_changeable = Array.map changeable globals in
            let zero_me _ = False in
            let zero_locals = List.map zero_me m_locals in
            let call_locals = List.append m_args m_locals in
            let call_assigns = List.append args zero_locals in

            let do_single_assign_expr rest (id, val_expr) = 
                Bin(self#assign_expr id val_expr,
                    And,
                    rest) in
            let paired_assigns = self#zip_lists call_locals call_assigns in
            let assign_expr = List.fold_left do_single_assign_expr True paired_assigns in

            let return_label = self#get_new_return_handler in

            self#add_push_rules assign_expr 
                                the_control 
                                stmt_label
                                the_control 
                                (self#make_temp_label start_lab)
                                return_label
                                globals
                                locals
                                (Array.of_list call_locals)
                                g_changeable;

            (* Handle return *)
            let ret_changeable var = 
                if List.exists ((=) var) returns then
                    Yes
                else
                    No in
            let l_ret_changeable = Array.map ret_changeable locals in
            let g_ret_changeable = Array.map ret_changeable globals in
            let r_len = (List.length returns) in
            let return_vars = self#make_returns r_len in
            let all_gs = Array.append globals return_vars in
            let r_changeable = (Array.make r_len Disappearing) in
            let gr_ret_changeable = Array.append g_ret_changeable r_changeable in
            let return_vars_es = Array.map (fun v -> Id(v)) return_vars in
            let r_paired_assigns = self#zip_arrays (Array.of_list returns) return_vars_es in
            let r_assign_expr = Array.fold_left do_single_assign_expr True r_paired_assigns in

            self#add_rew_rules r_assign_expr 
                               the_control 
                               return_label
                               the_control 
                               next_stmt_label
                               all_gs
                               locals
                               gr_ret_changeable
                               l_ret_changeable


           


        method private do_while meth_name stmt_label locals next_stmt_label guard stmts = 
            let changeable _ =  No in
            let l_changeable = Array.map changeable locals in
            let g_changeable = Array.map changeable globals in
            let body_head_lab = self#get_statement_label (List.hd stmts) meth_name in
            self#add_choice_rew_rules guard
                                      the_control 
                                      stmt_label
                                      the_control 
                                      body_head_lab
                                      the_control
                                      next_stmt_label
                                      globals
                                      locals
                                      g_changeable
                                      l_changeable;
            self#process_statement_block meth_name locals stmts stmt_label
           
            


        (* in method name, array of local variable names, statement to translate *)
        method private process_statement meth_name locals next_stmt_label stmt =
            let stmt_label = self#get_statement_label stmt meth_name in
            let (_, body) = stmt in
            match body with
              Assign(ids, vals, cons) -> self#do_assign stmt_label locals next_stmt_label ids vals cons
            | Skip -> self#do_skip stmt_label locals next_stmt_label Bp.True
            | Goto(ids) -> self#do_goto stmt_label locals next_stmt_label ids
            | Return(es) -> self#do_return stmt_label locals next_stmt_label es
            | If(expr, ifs, elses) -> self#do_if meth_name stmt_label locals next_stmt_label expr ifs elses
            | Assert(expr) -> self#do_skip stmt_label locals next_stmt_label expr
            | Assume(expr) -> self#do_assume stmt_label locals next_stmt_label expr
            | Call(returns, name, args) -> self#do_call stmt_label locals next_stmt_label returns name args
            | UnsupportedStatement -> ()
            | While(guard, stmts) -> self#do_while meth_name stmt_label locals next_stmt_label guard stmts


        method private process_statement_block meth_name locals stmts next_label = 
            let rec do_stmts stmts = 
                match stmts with
                  [] -> failwith ("No statements in block")
                | [s] -> self#process_statement meth_name locals next_label s
                | s::s'::t -> let s'_lab = self#get_statement_label s' meth_name in
                              self#process_statement meth_name locals s'_lab s;
                              do_stmts (s'::t) in
            do_stmts stmts



        method private resolve_method_start_label name stmts = 
            match stmts with
              [] -> failwith ("No statements in block")
            | s::t -> let s_lab = self#get_statement_label s name in
                      let (m_lab,_,_) = self#get_method_info name in
                      label_map <- LabelMap.add m_lab s_lab label_map





        method private process_method meth = 
            let (_, name, args, locals, _, _, stmts) = meth in
            self#resolve_method_start_label name stmts;
            let all_locals = Array.of_list (List.append args locals) in
            let def_return_label = "DefRet" in
            self#process_statement_block name all_locals stmts def_return_label


        method private method_start meth_name =
            meth_name ^ "_ENTRY"


        method private build_method_map ms =
            let do_method m = 
                let (_, name, args, locals, _, _, stmts) = m in
                methods <- MethodMap.add name (self#method_start name, args, locals) methods in
            List.iter do_method ms


        method private get_method_info name = 
            try
                MethodMap.find name methods
            with Not_found ->
                failwith ("Method not found: " ^ name ^ ".")

                


        method convert = 
            self#preprocess_bp;
            let (gs, ms) = bp in
                self#build_method_map ms;
            List.iter self#process_method ms;
            self#relabel_pds;
            (pds, self#build_prop_map)


        method private build_prop_map =
            let do_pair (c, cps) (a, aps) plist = 
                (c, a, List.append cps aps)::plist in
            let do_control (c, cps) plist = 
                let result = CharPropSet.fold (do_pair (c, cps)) char_props plist in
                (c, sbot, cps)::result in
            new proposition_map (ControlPropSet.fold do_control control_props [])
            



    end;;
