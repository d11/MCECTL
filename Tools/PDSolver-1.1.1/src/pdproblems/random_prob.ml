
open Pds;;
open Words;;
open Mucalc;;
open Pds_mucalc;;
open Parity_game;;


Random.self_init ();;

let random_range_int x y = 
    x + Random.int ((y - x) + 1);;

let random_range_float x y = 
    x +. Random.float (y -. x);;

let num_trans ncontrols nchars tran_density = 
    int_of_float (tran_density *. (float_of_int (ncontrols * nchars)));;

(*******************************************************
 * Words 
 *)

let nth_char n = 
    make_character ("a" ^ (string_of_int n));;

let random_char n = 
    nth_char (Random.int n);;

let rec random_word nchar len = 
    if len = 0 then
        make_word []
    else
        let head = random_char nchar in
        let tail = word_to_list (random_word nchar (len - 1)) in
        make_word (head::tail);;


(********************************************************
 * PDS 
 *)

let nth_control n = 
    make_control_state ("p" ^ (string_of_int n));;

let random_control n = 
    nth_control (Random.int n);;


let random_tran ncontrol nchar min_push max_push = 
    let p = random_control ncontrol in
    let p' = random_control ncontrol in
    let a = random_char nchar in
    let range = max_push - min_push in
    let offset = Random.int (range + 1) in
    let len = min_push + offset in
    let w = random_word nchar len in
    make_rule p a p' w;;

  
let random_pds ncontrol nchar min_push max_push tran_density_min tran_density_max =
    let tran_d = random_range_float tran_density_min tran_density_max in
    let ntran = num_trans ncontrol nchar tran_d in
    let pds = new pds in
    let num_rules = ref 0 in
    while !num_rules < ntran do
        let rule = (random_tran ncontrol nchar min_push max_push) in
        let (p, a, p', w) = rule_tuple rule in
        let similar = pds#get_next p a in
        if not (NextSet.mem (p', w) similar) then (
            pds#add_rule rule;
            num_rules := !num_rules + 1;
        )
    done;
    pds;;

(******************************************************
 * Parity Game
 *)


let random_player abelard_density = 
    if (Random.float 1.0 < abelard_density) then
        A
    else
        E;;

let random_colour min_colour max_colour = 
    let ncols = max_colour - min_colour in
    min_colour + (Random.int (ncols + 1));;


let random_pdg_props ncontrol abelard_density min_colour max_colour = 
    let rec prop_list n =
        let nprop = (nth_control n, 
                     random_player abelard_density, 
                     random_colour min_colour max_colour) in
        if n = 0 then
            [nprop]
        else
            nprop::(prop_list (n - 1)) in
    new property_map (prop_list (ncontrol - 1));;
    


let random_pdg ncontrol nchar min_push max_push tran_density_min tran_density_max min_colour max_colour abelard_density = 
    let pds = random_pds ncontrol nchar min_push max_push tran_density_min tran_density_max in
    let props = random_pdg_props ncontrol abelard_density min_colour max_colour in
    new parity_game pds props [];;

let random_pdg_ncols ncontrol 
                     nchar 
                     min_push
                     max_push 
                     tran_density_min 
                     tran_density_max 
                     ncolours_min 
                     ncolours_max 
                     abelard_density = 
    let ncolours = random_range_int ncolours_min ncolours_max in
    if Random.bool () then
        random_pdg ncontrol nchar min_push max_push tran_density_min tran_density_max 0 (ncolours - 1) abelard_density 
    else 
        random_pdg ncontrol nchar min_push max_push tran_density_min tran_density_max 1 ncolours abelard_density;;
        



(******************************************************
 * Mu Calc 
 *)


let default_mu_filter fmla = 
    (* returns (good, variables occuring before a [] or <>, variables occuring within a [] or <>) *)
    let rec var_check fmla = 
        match fmla with
          True -> (true, [], [])
        | False -> (true, [], [])
        | Prop(_) -> (true, [], [])
        | NegProp(_) -> (true, [], [])
        | Var(id) -> (true, [id], [])
        | Bin(f1,_,f2) -> let (good1, vars1, vars1') = var_check f1 in
                          let (good2, vars2, vars2') = var_check f2 in
                          (good1 && good2, List.append vars1 vars2, List.append vars1' vars2')
        | Un(Not,f1) -> var_check f1 
        | Un(_, f1) -> let (good, vars, vars') = var_check f1 in
                       (good, [], List.append vars vars')
        | ConstrainUn(_, _, f1) -> let (good, vars, vars') = var_check f1 in
                                   (good, [], List.append vars vars')
        | Mu(id,f1) -> let (good, vars, vars') = var_check f1 in
                       let is_id s = (s = id) in
                       let (_, vars_rest) = List.partition is_id vars in
                       let (is_id_vars', vars'_rest) = List.partition is_id vars' in
                       ((List.length is_id_vars') > 0 && good, vars_rest, vars'_rest)
        | Nu(id,f1) -> let (good, vars, vars') = var_check f1 in
                       let is_id s = (s = id) in
                       let (_, vars_rest) = List.partition is_id vars in
                       let (is_id_vars', vars'_rest) = List.partition is_id vars' in
                       ((List.length is_id_vars') > 0 && good, vars_rest, vars'_rest) in
    let (good, _, _) = var_check fmla in
    good;;


let rec make_props nprops = 
    if nprops = 0 then
        []
    else
        Prop("p" ^ (string_of_int nprops))::(make_props (nprops - 1));;

let next_var = ref 0;;

let reset_vars () = 
    next_var := 0;;

let new_var_id () = 
    next_var := !next_var + 1;
    "Z" ^ (string_of_int (!next_var));;


(* Not sure how best to generate anything more specific than max_depth
 * Probably better to sort by certain metrics such as nesting depth post_hoc
 * and whether a bound variable is actually used...
 *)
let random_mu_calc props max_depth =
    reset_vars ();
    let rec diver atoms max_depth = 
        if max_depth = 0 then (
            let n = Random.int (List.length atoms) in
            let prop = List.nth atoms n in
            match prop with
              Prop(id) -> if Random.bool () then 
                              prop 
                          else
                              NegProp(id)
            | _ -> prop
        ) else (
            let depth = max_depth - 1 in
            match (Random.int 6) with
              0 -> Bin(diver atoms depth, And, diver atoms depth)
            | 1 -> Bin(diver atoms depth, Or, diver atoms depth)
            | 2 -> Un(Box, diver atoms depth)
            | 3 -> Un(Diamond, diver atoms depth)
            | 4 -> let id = new_var_id () in
                   Mu(id, diver ((Var(id))::atoms) depth)
            | 5 -> let id = new_var_id () in
                   Nu(id, diver ((Var(id))::atoms) depth)
            | _ -> failwith "random_mu_calc called (Random.int 6) and got a number it shouldn't have."
        ) in
    let fmla = diver props max_depth in
    reset_vars ();
    fmla;;


(* As above, but only returns results that pass the filter *)
let rec random_mu_calc_filter filter props max_depth = 
    let fmla = random_mu_calc props max_depth in
    if filter fmla then
        fmla
    else
        random_mu_calc_filter filter props max_depth;;

let default_random_mu_calc_filter props max_depth = 
    random_mu_calc_filter default_mu_filter props max_depth;;

let random_mu_calc_nprops nprops max_depth =
    let props = make_props nprops in
    random_mu_calc props max_depth;;

let random_mu_calc_filter_nprops filter nprops max_depth = 
    let props = make_props nprops in
    random_mu_calc_filter filter props max_depth;;

let default_random_mu_calc_filter_nprops nprops max_depth = 
    let props = make_props nprops in
    random_mu_calc_filter default_mu_filter props max_depth;;


(***********************************************************************
 * PDS Mu Calc
 *)




let random_prop_map props prop_density ncontrols nchars =
    let rec do_props props = 
        match props with
          [] -> []
        | (Prop(id))::rest -> if (Random.float 1.0) < prop_density then
                                  id::(do_props rest)
                              else
                                  do_props rest 
        | _::rest -> do_props rest in
    let assignments = ref [] in
    for c = 0 to (ncontrols - 1) do
        for a = 0 to (nchars - 1) do
            assignments := (nth_control c, nth_char a, do_props props)::(!assignments)
        done;
    done;
    new proposition_map (!assignments);;


                   

let random_pds_mucalc nprops prop_density max_depth mu_filter ncontrols nchars min_push max_push tran_density_min tran_density_max = 
    let pds = random_pds ncontrols nchars min_push max_push tran_density_min tran_density_max in
    let props = make_props nprops in
    let fmla = random_mu_calc_filter mu_filter props max_depth in
    let prop_map = random_prop_map props prop_density ncontrols nchars in
    new pds_mucalc pds [] fmla prop_map;;



let default_random_pds_mucalc nprops prop_density max_depth ncontrols nchars min_push max_push tran_density_min tran_density_max = 
    random_pds_mucalc nprops prop_density max_depth default_mu_filter ncontrols nchars min_push max_push tran_density_min tran_density_max;;


