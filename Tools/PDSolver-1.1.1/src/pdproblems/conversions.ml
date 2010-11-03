
open Pds;;
open Pds_mucalc;;
open Parity_game;;
open Mucalc;;
open Words;;


module OrderedFmla = 
    struct
        type t = mu_formula
        let compare = compare
    end;;

module FmlaParityMap = Map.Make(OrderedFmla);;

let mu_to_pdg_control p f = 
    make_control_state ((control_state_to_string p) ^ "__" ^ (mu_formula_to_string f));;


let pdmu_to_pdg pdmu = 
    let pds = pdmu#get_pds in
    let fmla = pdmu#get_fmla in
    let map = pdmu#get_props in
    let g_pds = new pds in
    let norm_f = normalise_formula fmla in
    let subs = get_subformulas norm_f in
    let controls = pds#get_control_states in
    let alphabet = pds#get_alphabet in
    let psat = make_control_state "psat" in
    let punsat = make_control_state "punsat" in


    let add_state_change_rules p p' = 
        CharacterSet.foreach alphabet (fun c ->
            let rule = make_rule p c p' (make_word [c]) in
            g_pds#add_rule rule
        ) in

    let add_next_rules p f f' = 
        let pf = mu_to_pdg_control p f in
        CharacterSet.foreach alphabet (fun a ->
            NextSet.foreach (pds#get_next p a) (fun (p', w) ->
                let p'f' = mu_to_pdg_control p' f' in
                let rule = make_rule pf a p'f' w in
                g_pds#add_rule rule
            )
        ) in

    let add_prop_rules p id =
        let pf = mu_to_pdg_control p (Prop(id)) in
        CharacterSet.foreach alphabet (fun a ->
            let resp = if map#head_sat p a id then psat else punsat in
            let rule = make_rule pf a resp (make_word [a]) in
            g_pds#add_rule rule
        ) in

    let add_neg_prop_rules p id =
        let pf = mu_to_pdg_control p (NegProp(id)) in
        CharacterSet.foreach alphabet (fun a ->
            let resp = if map#head_sat p a id then punsat else psat in
            let rule = make_rule pf a resp (make_word [a]) in
            g_pds#add_rule rule
        ) in

    let add_rules () = 
        ControlStateSet.foreach controls (fun p ->
            MuSet.foreach subs (fun f ->
                let pf = mu_to_pdg_control p f in
                match f with
                  True -> failwith ("pds_mucalc#to_pdg found a 'True' in normed fmla " ^  
                                                  (mu_formula_to_string fmla))
                | False -> failwith ("pds_mucalc#to_pdg found a 'False' in normed fmla " ^  
                                                  (mu_formula_to_string fmla))
                | Prop(id) -> add_prop_rules p id
                | NegProp(id) -> add_neg_prop_rules p id
                | Var(_) -> ()
                | Bin(f1, And, f2) -> (let pf1 = mu_to_pdg_control p f1 in
                                       let pf2 = mu_to_pdg_control p f2 in
                                       add_state_change_rules pf pf1;
                                       add_state_change_rules pf pf2)
                | Bin(f1, Or, f2) -> (let pf1 = mu_to_pdg_control p f1 in
                                      let pf2 = mu_to_pdg_control p f2 in
                                      add_state_change_rules pf pf1;
                                      add_state_change_rules pf pf2)
                | Bin(_, Implies, _) -> failwith ("pds_mucalc#to_pdg found an implication in normed fmla " ^  
                                                  (mu_formula_to_string fmla))
                | Un(Box,f1) -> add_next_rules p f f1
                | Un(Diamond,f1) -> add_next_rules p f f1
                | Un(BackBox,f1) -> failwith "implement conversions add rules back box"
                | Un(BackDiamond,f1) -> failwith "implement conversions add rules back diamond"
                | Un(Not,_) -> failwith ("pds_mucalc#to_pdg found a negation in normed fmla " ^  
                                       (mu_formula_to_string fmla))
                | ConstrainUn(op,ids,f1) -> failwith "implement conversions add rules constrain _"
                | Mu(id, f1) -> (let pz = mu_to_pdg_control p (Var(id)) in
                                 let pf1 = mu_to_pdg_control p f1 in
                                 add_state_change_rules pf pz;
                                 add_state_change_rules pz pf1)
                | Nu(id, f1) -> (let pz = mu_to_pdg_control p (Var(id)) in
                                 let pf1 = mu_to_pdg_control p f1 in
                                 add_state_change_rules pf pz;
                                 add_state_change_rules pz pf1)
            )
        ) in

    let fmla_parity = 
        (* choose 1 if first fixed point is least, 0 if greatest *)
        let rec first_parity f =
            match f with
              True | False | Prop(_) | NegProp(_) | Var(_) -> 0
            | Bin(f1, _, f2) -> min (first_parity f1) (first_parity f2)
            | Un(_, f1) -> first_parity f1
            | ConstrainUn(_, _, f1) -> first_parity f1
            | Mu(_, _) -> 1
            | Nu(_, _) -> 0 in
        let join_maps m1 m2 = 
            FmlaParityMap.fold (fun v d m -> FmlaParityMap.add v d m) m1 m2 in
        let next_odd n =
            if n mod 2 = 0 then n + 1 else n in
        let next_even n =
            if n mod 2 = 1 then n + 1 else n in
        let rec diver f depth = 
            match f with
              True | False | Prop(_) | NegProp(_) -> FmlaParityMap.add f depth (FmlaParityMap.empty)
            | Var(id) -> FmlaParityMap.empty
            | Bin(f1, _, f2) -> let m1 = diver f1 depth in
                                let m2 = diver f2 depth in
                                FmlaParityMap.add f depth (join_maps m1 m2)
            | Un(_, f1) -> FmlaParityMap.add f depth (diver f1 depth)
            | ConstrainUn(_, _, f1) -> FmlaParityMap.add f depth (diver f1 depth)
            | Mu(id, f1) -> let depth' = next_odd depth in
                            let m = FmlaParityMap.add (Var(id)) depth' (diver f1 depth') in
                            FmlaParityMap.add f depth' m
            | Nu(id, f1) -> let depth' = next_even depth in
                            let m = FmlaParityMap.add (Var(id)) depth' (diver f1 depth') in
                            FmlaParityMap.add f depth' m in
        diver norm_f (first_parity norm_f) in

    let min_colour = FmlaParityMap.fold (fun _ d dmin -> min d dmin) fmla_parity max_int in

    let props = 
        ControlStateSet.fold (fun p ps -> 
            MuSet.fold (fun f ps ->
                let pf = mu_to_pdg_control p f in
                let c = FmlaParityMap.find f fmla_parity in
                match f with
                  True -> failwith ("pds_mucalc#to_pdg found a 'True' in normed fmla " ^  
                                       (mu_formula_to_string fmla))
                | False -> failwith ("pds_mucalc#to_pdg found a 'False' in normed fmla " ^  
                                       (mu_formula_to_string fmla))
                | Prop(id) -> (pf, E, min_colour)::ps (* Just moves to appropriate sink state *)
                | NegProp(id) -> (pf, E, min_colour)::ps
                | Var(_) -> (pf, E, c)::ps
                | Bin(f1, And, f2) -> (pf, A, c)::ps
                | Bin(f1, Or, f2) -> (pf, E, c)::ps
                | Bin(_, Implies, _) -> failwith ("pds_mucalc#to_pdg found an implication in normed fmla " ^  
                                                  (mu_formula_to_string fmla))
                | Un(Box, _) -> (pf, A, c)::ps
                | Un(Diamond, f1) -> (pf, E, c)::ps
                | Un(BackBox, f1) -> failwith "implement conversions props back box"
                | Un(BackDiamond, f1) -> failwith "implement conversions props back diamond"
                | Un(Not,_) -> failwith ("pds_mucalc#to_pdg found a negation in normed fmla " ^  
                                       (mu_formula_to_string fmla))
                | ConstrainUn(_, _, _) -> failwith "implement conversions props constrain _"
                | Mu(id, _) -> (pf, E, c)::ps
                | Nu(id, _) -> (pf, E, c)::ps
            ) subs ps
        ) controls [] in

    let all_props = (psat, A, min_colour)::(punsat, E, min_colour)::props in
    add_rules ();
    new parity_game g_pds (new property_map all_props) [];;








let pdg_to_pdmu pdg = 
    let pds = pdg#get_pds in
    let props = pdg#get_props in
    let min_c = props#get_min_colour in
    let max_c = props#get_max_colour in
    let controls = pds#get_control_states in
    let alphabet = pds#get_alphabet in

    let e_id = "E" in
    let ep = Prop(e_id) in

    let c_prop_id c = 
        string_of_int c in
    let make_c_prop c = 
        Prop(c_prop_id c) in

    let c_var_id c = 
        "Z" ^ (string_of_int c) in
    let make_c_var c = 
        Var(c_var_id c) in

    let rec colour_imps min max modal =
        let pc = make_c_prop max in
        let zc = make_c_var max in
        let imp = Bin(pc, Implies, Un(modal, zc)) in
        if min = max then
            imp
        else    
            Bin(imp, And, colour_imps min (max - 1) modal) in

    let f_e_imps = colour_imps min_c max_c Diamond in
    let f_a_imps = colour_imps min_c max_c Box in

    let f_e = Bin(ep, Implies, f_e_imps) in
    let f_a = Bin(Un(Not, ep), Implies, f_a_imps) in

    let phi_E = Bin(f_e, And, f_a) in

    let rec make_phi min max =
        if min > max then
            phi_E
        else if (min mod 2 = 0) then
            Nu(c_var_id min, make_phi (min + 1) max)
        else
            Mu(c_var_id min, make_phi (min + 1) max) in

    let phi = make_phi min_c max_c in

    let props_list = 
        ControlStateSet.fold (fun p ps ->
            let col_id = c_prop_id (props#get_colour p) in
            let is_E = (props#get_owner p = E) in
            CharacterSet.fold (fun a ps ->
                if is_E then
                    (p, a, [col_id; e_id])::ps
                else
                    (p, a, [col_id])::ps
            ) alphabet ps
        ) controls [] in
    let props = new proposition_map props_list in
    
    new pds_mucalc pds [] phi props;;

