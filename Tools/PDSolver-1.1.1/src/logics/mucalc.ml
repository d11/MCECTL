

type id = string;;

type binop = And | Or | Implies;;

type unop = Not | Box | Diamond | BackBox | BackDiamond;;

type constrain_unop = ConstrainBox | ConstrainDiamond |
                      NegConstrainBox | NegConstrainDiamond |
                      ConstrainBackBox | ConstrainBackDiamond |
                      NegConstrainBackBox | NegConstrainBackDiamond;;


type mu_formula = True | 
                  False |
                  Prop of id |
                  NegProp of id |
                  Var of id |
                  Bin of mu_formula * binop * mu_formula |
                  Un of unop * mu_formula |
                  ConstrainUn of constrain_unop * id list * mu_formula |
                  Mu of id * mu_formula |
                  Nu of id * mu_formula;;

type alt_type = None | Greatest | Least;;

let tt_prop = Prop("tt");;
let ff_prop = Prop("ff");;

let id_to_string id = id;;

let rec id_list_to_string ids = 
    match ids with
      [] -> ""
    | h::[] -> id_to_string h
    | h::t -> (id_to_string h) ^ " " ^ (id_list_to_string t)

let binop_to_string bop = 
    match bop with
      And -> "&"
    | Or  -> "|"
    | Implies -> "=>";;


let unop_to_string un = 
    match un with 
      Not -> "!"
    | Box -> "[]" 
    | Diamond -> "<>"
    | BackBox -> "~[]"
    | BackDiamond -> "~<>";;

let constrain_unop_to_string un ids = 
    match un with
      ConstrainBox -> "[@" ^ (id_list_to_string ids) ^ "]"
    | ConstrainDiamond -> "<@" ^ (id_list_to_string ids) ^ ">"
    | NegConstrainBox -> "[\\" ^ (id_list_to_string ids) ^ "]"
    | NegConstrainDiamond -> "<\\" ^ (id_list_to_string ids) ^ ">"
    | ConstrainBackBox -> "~[@" ^ (id_list_to_string ids) ^ "]"
    | ConstrainBackDiamond -> "~<@" ^ (id_list_to_string ids) ^ ">"
    | NegConstrainBackBox -> "~[\\" ^ (id_list_to_string ids) ^ "]"
    | NegConstrainBackDiamond -> "~<\\" ^ (id_list_to_string ids) ^ ">";;


let rec mu_formula_to_string fmla = 
    match fmla with
      True -> "#T"
    | False -> "#F"
    | Prop(id) -> id_to_string id
    | NegProp(id) -> "!" ^ (id_to_string id)
    | Var(id) -> id_to_string id
    | Bin(f1,op,f2) -> ("(" ^ 
                        (mu_formula_to_string f1) ^ 
                        " " ^ (binop_to_string op) ^ " " ^
                        (mu_formula_to_string f2) ^ 
                        ")")
    | Un(op,f) ->  ((unop_to_string op) ^ "(" ^ (mu_formula_to_string f) ^ ")")
    | ConstrainUn(op,ids,f) -> ((constrain_unop_to_string op ids) ^ 
                                "(" ^ (mu_formula_to_string f) ^ ")")
    | Mu(id,f) -> ("(mu " ^ id ^ "." ^ (mu_formula_to_string f) ^ ")")
    | Nu(id,f) -> ("(nu " ^ id ^ "." ^ (mu_formula_to_string f) ^ ")");;




let id_to_dot_string id = id;;

let rec id_list_to_dot_string ids = 
    match ids with
      [] -> ""
    | h::[] -> id_to_string h 
    | h::t -> (id_to_dot_string h) ^ "_" ^ (id_list_to_dot_string t)



let binop_to_dot_string bop = 
    match bop with
      And -> "AND"
    | Or  -> "OR"
    | Implies -> "IMPLIES";;


let unop_to_dot_string un = 
    match un with 
      Not -> "NOT"
    | Box -> "BOX" 
    | Diamond -> "DIAMOND"
    | BackBox -> "BACKBOX"
    | BackDiamond -> "BACKDIAMOND";;


let constrain_unop_to_dot_string un ids = 
    match un with
      ConstrainBox -> "CBL" ^ (id_list_to_dot_string ids) ^ "CBR"
    | ConstrainDiamond -> "DBL" ^ (id_list_to_string ids) ^ "DBR"
    | NegConstrainBox -> "NCBL" ^ (id_list_to_dot_string ids) ^ "NCBR"
    | NegConstrainDiamond -> "NDBL" ^ (id_list_to_dot_string ids) ^ "NDBL"
    | ConstrainBackBox -> "CBBL" ^ (id_list_to_dot_string ids) ^ "CBBR"
    | ConstrainBackDiamond -> "CBDL" ^ (id_list_to_string ids) ^ "CBDR"
    | NegConstrainBackBox -> "NCBBL" ^ (id_list_to_dot_string ids) ^ "NCBBR"
    | NegConstrainBackDiamond -> "NCBDL" ^ (id_list_to_dot_string ids) ^ "NCBDL";;




let rec mu_formula_to_dot_string fmla = 
    match fmla with
      True -> "_T"
    | False -> "_F"
    | Prop(id) -> id_to_dot_string id
    | NegProp(id) -> "NOT_" ^ (id_to_dot_string id)
    | Var(id) -> id_to_dot_string id
    | Bin(f1,op,f2) -> ("LPAR" ^ 
                        (mu_formula_to_dot_string f1) ^ 
                        "_" ^ (binop_to_dot_string op) ^ "_" ^
                        (mu_formula_to_dot_string f2) ^ 
                        "RPAR")
    | Un(op,f) ->  ((unop_to_dot_string op) ^ "LPAR" ^ (mu_formula_to_dot_string f) ^ "RPAR")
    | ConstrainUn(op,ids,f) -> ((constrain_unop_to_dot_string op ids) ^ 
                                "LPAR" ^ (mu_formula_to_dot_string f) ^ "RPAR")
    | Mu(id,f) -> ("LPARMU_" ^ id ^ "_DOT_" ^ (mu_formula_to_dot_string f) ^ "RPAR")
    | Nu(id,f) -> ("LPARNU_" ^ id ^ "_DOT_" ^ (mu_formula_to_dot_string f) ^ "RPAR");;




let normalise_formula fmla = 
    let next_var_id = ref (-1) in

    let neg_constrain_op op = 
        match op with
          ConstrainBox -> ConstrainDiamond
        | ConstrainDiamond -> ConstrainBox
        | NegConstrainBox -> NegConstrainDiamond
        | NegConstrainDiamond -> NegConstrainBox
        | ConstrainBackBox -> ConstrainBackDiamond
        | ConstrainBackDiamond -> ConstrainBackBox
        | NegConstrainBackBox -> NegConstrainBackDiamond
        | NegConstrainBackDiamond -> NegConstrainBackBox in

    let rec replace_var z z' f = 
        match f with
          True -> f
        | False -> f
        | Prop(id) -> f
        | NegProp(id) -> f
        | Var(id) -> if f = z then  z' else f
        | Bin(f1,op,f2) -> Bin(replace_var z z' f1, op, replace_var z z' f2)
        | Un(op,f1) ->  Un(op, replace_var z z' f1)
        | ConstrainUn(op,ids,f1) ->  ConstrainUn(op, ids, replace_var z z' f1)
        | Mu(id,f1) -> if Var(id) = z then f else Mu(id, replace_var z z' f1)
        | Nu(id,f1) -> if Var(id) = z then f else Nu(id, replace_var z z' f1) in

    (* uniquify variable names *)
    let rec var_norm f = 
        match f with
          True -> f
        | False -> f
        | Prop(id) -> f
        | NegProp(id) -> f
        | Var(id) -> f
        (* Note we're forcing evaluation order for the sake of testing *)
        | Bin(f1,op,f2) -> let f1' = var_norm f1 in
                           let f2' = var_norm f2 in
                           Bin(f1', op, f2')
        | Un(op,f) ->  Un(op, var_norm f)
        | ConstrainUn(op,ids,f) ->  ConstrainUn(op, ids, var_norm f)
        | Mu(id,f) -> (next_var_id := !next_var_id + 1;
                       let id' = id ^ "_" ^ (string_of_int !next_var_id) in
                       let f' = replace_var (Var(id)) (Var(id')) f in
                       Mu(id', var_norm f'))
        | Nu(id,f) -> next_var_id := !next_var_id + 1;
                      let id' = id ^ "_" ^ (string_of_int !next_var_id) in
                      let f' = replace_var (Var(id)) (Var(id')) f in
                      Nu(id', var_norm f') in

    (* positive normal form *)
    let rec neg_norm f = 
        match f with
          True -> f
        | False -> f
        | Prop(id) -> f
        | NegProp(id) -> f
        | Var(id) -> f
        | Bin(f1,And,f2) -> Bin(neg_norm f1, And, neg_norm f2)
        | Bin(f1,Or,f2) -> Bin(neg_norm f1, Or, neg_norm f2)
        | Bin(f1,Implies,f2) -> Bin(neg_norm_not f1, Or, neg_norm f2)
        | Un(Not,f) -> neg_norm_not f
        | Un(Box, f) -> Un(Box, neg_norm f)
        | Un(Diamond, f) -> Un(Diamond, neg_norm f)
        | Un(BackBox, f) -> Un(BackBox, neg_norm f)
        | Un(BackDiamond, f) -> Un(BackDiamond, neg_norm f)
        | ConstrainUn(op, ids, f) -> ConstrainUn(op, ids, neg_norm f)
        | Mu(id,f) -> Mu(id, neg_norm f)
        | Nu(id,f) -> Nu(id, neg_norm f)

    and neg_norm_not f = 
        match f with
          True -> False
        | False -> True
        | Prop(id) -> NegProp(id)
        | NegProp(id) -> Prop(id)
        | Var(id) -> failwith ("Variable " ^ id ^ " appears negatively.")
        | Bin(f1,And,f2) -> Bin(neg_norm_not f1, Or, neg_norm_not f2)
        | Bin(f1,Or,f2) -> Bin(neg_norm_not f1, And, neg_norm_not f2)
        | Bin(f1,Implies,f2) -> Bin(neg_norm f1, And, neg_norm_not f2)
        | Un(Not,f1) -> neg_norm f1
        | Un(Box, f1) -> Un(Diamond, neg_norm_not f1)
        | Un(Diamond, f1) -> Un(Box, neg_norm_not f1)
        | Un(BackBox, f1) -> Un(BackDiamond, neg_norm_not f1)
        | Un(BackDiamond, f1) -> Un(BackBox, neg_norm_not f1)
        | ConstrainUn(op, ids, f) -> ConstrainUn(neg_constrain_op op, ids, neg_norm_not f)
        | Mu(id,f1) -> Nu(id, neg_norm_not (replace_var (Var(id)) (Un(Not,Var(id))) f1))
        | Nu(id,f1) -> Mu(id, neg_norm_not (replace_var (Var(id)) (Un(Not,Var(id))) f1)) in


    (* remove (mu X . p), instances of true, false, &c. *)
    let minimize_fmla fmla = 
        let rec do_min f = 
            match f with
              True -> f
            | False -> f
            | Prop(_) -> f
            | NegProp(_) -> f
            | Var(_) -> f
            | Bin(True,And,f1) -> do_min f1
            | Bin(f1,And,True) -> do_min f1
            | Bin(False,And,f1) -> False
            | Bin(f1,And,False) -> False
            | Bin(True,Or,f1) -> True
            | Bin(f1,Or,True) -> True
            | Bin(False,Or,f1) -> do_min f1
            | Bin(f1,Or,False) -> do_min f1
            | Bin(True,Implies,f1) -> do_min f1
            | Bin(f1,Implies,True) -> True
            | Bin(False,Implies,f1) -> True
            | Bin(f1,Implies,False) -> (Un(Not, do_min f1))
            | Bin(f1,op,f2) -> Bin(do_min f1, op, do_min f2)
            | Un(Not,True) -> False
            | Un(Not,False) -> True
            | Un(Not,Prop(id)) -> NegProp(id)
            | Un(op,f1) -> Un(op, do_min f1)
            | ConstrainUn(op, ids, f1) -> ConstrainUn(op, ids, do_min f1)
            | Mu(_,True) -> True
            | Mu(_,False) -> False
            | Mu(_,Prop(x)) -> Prop(x)
            | Mu(_,NegProp(x)) -> NegProp(x)
            | Mu(id1,Var(id2)) -> if id1 = id2 then
                                      False
                                  else
                                      Var(id2)
            | Mu(id,f1) -> Mu(id, do_min f1)
            | Nu(_,True) -> True
            | Nu(_,False) -> False
            | Nu(_,Prop(x)) -> Prop(x)
            | Nu(_,NegProp(x)) -> NegProp(x)
            | Nu(id1,Var(id2)) -> if id1 = id2 then
                                      True
                                  else
                                      Var(id2)
            | Nu(id,f1) -> Nu(id, do_min f1) in
            
        let rec iter_min f = 
            let min_f = do_min f in
            if f = min_f then
                min_f
            else
                iter_min min_f in
        iter_min fmla in

    neg_norm (var_norm (minimize_fmla fmla));;

        

let rec mu_formula_prop_ids f =
    match f with
      True -> []
    | False -> []
    | Prop(id) -> [id]
    | NegProp(id) -> [id]
    | Var(id) -> []
    | Bin(f1,op,f2) -> List.append (mu_formula_prop_ids f1) (mu_formula_prop_ids f2)
    | Un(op,f1) ->  mu_formula_prop_ids f1
    | ConstrainUn(op, _,f1) ->  mu_formula_prop_ids f1
    | Mu(id,f1) -> mu_formula_prop_ids f1
    | Nu(id,f1) -> mu_formula_prop_ids f1;;



let rec mu_fixed_point_depth f = 
    match f with
      True -> 0
    | False -> 0
    | Prop(id) -> 0
    | NegProp(id) -> 0
    | Var(id) -> 0
    | Bin(f1,op,f2) -> max (mu_fixed_point_depth f1) (mu_fixed_point_depth f2)
    | Un(op,f1) ->  mu_fixed_point_depth f1
    | ConstrainUn(op, _,f1) ->  mu_fixed_point_depth f1
    | Mu(id,f1) -> 1 + (mu_fixed_point_depth f1)
    | Nu(id,f1) -> 1 + (mu_fixed_point_depth f1);;

let mu_alternation_depth fmla = 
    let rec alt_dep f last = 
        match f with
          True -> 0
        | False -> 0
        | Prop(id) -> 0
        | NegProp(id) -> 0
        | Var(id) -> 0
        | Bin(f1,op,f2) -> max (alt_dep f1 last) (alt_dep f2 last)
        | Un(op,f1) ->  alt_dep f1 last
        | ConstrainUn(op, _, f1) -> alt_dep f1 last
        | Mu(id,f1) -> if last = Least then
                           alt_dep f1 Least
                       else
                           1 + (alt_dep f1 Least)
        | Nu(id,f1) -> if last = Greatest then
                           alt_dep f1 Greatest
                       else
                           1 + (alt_dep f1 Greatest) in
    alt_dep fmla None;;




module OrderedMuFmla = 
    struct
        type t = mu_formula
        let compare = compare
        let to_string f = mu_formula_to_string f
    end;;


module MuSet = Extset.Make(OrderedMuFmla);;

module OrderedId = 
    struct
        type t = id
        let compare = compare
        let to_string id = id
    end;;


module IdSet = Extset.Make(OrderedId);;




let rec get_subformulas f = 
    match f with 
      True -> MuSet.singleton f
    | False -> MuSet.singleton f
    | Prop(_) -> MuSet.singleton f
    | NegProp(_) -> MuSet.singleton f
    | Var(_) -> MuSet.singleton f
    | Bin(f1, _, f2) -> let subs1 = get_subformulas f1 in
                         let subs2 = get_subformulas f2 in
                         let subs = MuSet.union subs1 subs2 in
                         MuSet.add f subs
    | Un(_, f1) -> MuSet.add f (get_subformulas f1)
    | ConstrainUn(_, _, f1) -> MuSet.add f (get_subformulas f1)
    | Mu(_, f1) -> MuSet.add f (get_subformulas f1)
    | Nu(_, f1) -> MuSet.add f (get_subformulas f1);;


let rec mu_lits f = 
    match f with 
      True -> MuSet.empty
    | False -> MuSet.empty
    | Prop(_) -> MuSet.singleton f
    | NegProp(_) -> MuSet.singleton f
    | Var(_) -> MuSet.empty
    | Bin(f1, _, f2) -> let subs1 = mu_lits f1 in
                        let subs2 = mu_lits f2 in
                        MuSet.union subs1 subs2
    | Un(_, f1) -> (mu_lits f1)
    | ConstrainUn(_, _, f1) -> (mu_lits f1)
    | Mu(_, f1) -> (mu_lits f1)
    | Nu(_, f1) -> (mu_lits f1);;


let rec mu_length f = 
    match f with
      True -> 1
    | False -> 1
    | Prop(_) -> 1
    | NegProp(_) -> 1
    | Var(_) -> 1
    | Bin(f1, _, f2) -> 1 + (mu_length f1) + (mu_length f2)
    | Un(_, f1) -> 1 + (mu_length f1)
    | ConstrainUn(_, _, f1) -> 1 + (mu_length f1)
    | Mu(_, f1) -> 1 + (mu_length f1)
    | Nu(_, f1) -> 1 + (mu_length f1);;


let rec mu_num_lits f = 
    let lits = mu_lits f in
    MuSet.cardinal lits


let mu_stats_string fmla =  
    let length = mu_length fmla in
    let fp_depth = mu_fixed_point_depth fmla in
    let alt_depth = mu_alternation_depth fmla in
    let nlits = mu_num_lits fmla in
    "Fmla: " ^ (mu_formula_to_string fmla) ^ "\n" ^
    "Length: " ^ (string_of_int length) ^
    "   FP Depth: " ^ (string_of_int fp_depth) ^
    "   Alt Depth: " ^ (string_of_int alt_depth) ^
    "   Num lits: " ^ (string_of_int nlits) ^ "\n"
    


class mu_stats (fmla : mu_formula) (length : int) (fp_depth : int) (alt_depth : int) (num_lits : int) = 
    object
        method get_fmla = fmla
        method get_length = length
        method get_fp_depth = fp_depth
        method get_alt_depth = alt_depth
        method get_num_lits = num_lits

        method to_string = 
            "Fmla: " ^ (mu_formula_to_string fmla) ^ "\n" ^
            "Len: " ^ (string_of_int length) ^ "  " ^
            "FP Depth: " ^ (string_of_int fp_depth) ^ "  " ^
            "Alt Depth: " ^ (string_of_int alt_depth) ^ "  " ^
            "Lits: " ^ (string_of_int num_lits) ^ "\n"

        method to_row_string = 
            (string_of_int length) ^ " " ^
            (string_of_int fp_depth) ^ " " ^
            (string_of_int alt_depth) ^ " " ^
            (string_of_int num_lits)

    end;;

let mu_get_stats fmla = 
    let length = mu_length fmla in
    let fp_depth = mu_fixed_point_depth fmla in
    let alt_depth = mu_alternation_depth fmla in
    let nlits = mu_num_lits fmla in
    new mu_stats fmla length fp_depth alt_depth nlits;;



let rec has_forwards_modalities fmla = 
    match fmla with 
      True -> false
    | False -> false
    | Prop(_) -> false
    | NegProp(_) -> false
    | Var(_) -> false
    | Bin(f1, _, f2) -> (has_forwards_modalities f1) || (has_forwards_modalities f2)
    | Un(Box, f1) -> true
    | Un(Diamond, f1) -> true
    | Un(_, f1) -> has_forwards_modalities f1
    | ConstrainUn(ConstrainBox, _, _) -> true
    | ConstrainUn(ConstrainDiamond, _, _) -> true
    | ConstrainUn(NegConstrainBox, _, _) -> true
    | ConstrainUn(NegConstrainDiamond, _, _) -> true
    | ConstrainUn(_, _, f1) -> has_forwards_modalities f1
    | Mu(_, f1) -> has_forwards_modalities f1
    | Nu(_, f1) -> has_forwards_modalities f1;;

  
let rec has_backwards_modalities fmla = 
    match fmla with 
      True -> false
    | False -> false
    | Prop(_) -> false
    | NegProp(_) -> false
    | Var(_) -> false
    | Bin(f1, _, f2) -> (has_backwards_modalities f1) || (has_backwards_modalities f2)
    | Un(BackBox, f1) -> true
    | Un(BackDiamond, f1) -> true
    | Un(_, f1) -> has_backwards_modalities f1
    | ConstrainUn(ConstrainBackBox, _, _) -> true
    | ConstrainUn(ConstrainBackDiamond, _, _) -> true
    | ConstrainUn(NegConstrainBackBox, _, _) -> true
    | ConstrainUn(NegConstrainBackDiamond, _, _) -> true
    | ConstrainUn(_, _, f1) -> has_backwards_modalities f1
    | Mu(_, f1) -> has_backwards_modalities f1
    | Nu(_, f1) -> has_backwards_modalities f1;;

let rec get_unbound_vars fmla = 
    match fmla with 
      True -> IdSet.empty
    | False -> IdSet.empty
    | Prop(_) -> IdSet.empty
    | NegProp(_) -> IdSet.empty
    | Var(z) -> IdSet.singleton z
    | Bin(f1, _, f2) -> IdSet.union (get_unbound_vars f1) (get_unbound_vars f2)
    | Un(BackBox, f1) -> get_unbound_vars f1
    | Un(BackDiamond, f1) -> get_unbound_vars f1
    | Un(_, f1) -> get_unbound_vars f1
    | ConstrainUn(ConstrainBackBox, _, f1) -> get_unbound_vars f1
    | ConstrainUn(ConstrainBackDiamond, _, f1) -> get_unbound_vars f1
    | ConstrainUn(NegConstrainBackBox, _, f1) -> get_unbound_vars f1
    | ConstrainUn(NegConstrainBackDiamond, _, f1) -> get_unbound_vars f1
    | ConstrainUn(_, _, f1) -> get_unbound_vars f1
    | Mu(z, f1) -> let unb = get_unbound_vars f1 in
                   IdSet.remove z unb
    | Nu(z, f1) -> let unb = get_unbound_vars f1 in
                   IdSet.remove z unb;;




