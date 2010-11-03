
open Mucalc;;
open Ma;;
open Pds;;
open Pds_mucalc;;
open Random_prob;;
open Mc_pds_mucalc;;
open Constants;;
open Mc_parity;;
open Parity_game;;
open Words;;
open Conversions;;


module PdsMuStateSet = PdsMuMa.MaState.StateSet;;
module PdsMuSetofStateSets = PdsMuMa.MaState.SetOfStateSets;;
 
module PGStateSet = ParityMa.MaState.StateSet;;
module PGSetofStateSets = ParityMa.MaState.SetOfStateSets;;
    

module Bool_ResultType =
    struct 
        type t = bool
        let equal x y = (x = y)
        let to_string = string_of_bool
    end;;

module RandomTest = Test.Tester(Bool_ResultType);;

let test_random () = 
    print_string "Doing random tests:\n\n";
    RandomTest.run_tests ();
    print_string "Done.\n\n";;


(*********************************************************************************
 * Checks for tests passed 
 *)

(* from a control state p, get the corresponding initial state from ma_mu *)
let get_init_mu_p p ma_mu = 
    let filter s = 
        match s with 
          Mc_pds_mucalc.QVal(p', _, _) -> (p = p') 
        | _ -> false in
    let (matches, _) = PdsMuStateSet.partition filter (ma_mu#get_inits) in
    let nhits = PdsMuStateSet.cardinal matches in
    if nhits = 1 then
        PdsMuStateSet.choose matches
    else
        failwith ("Mu automaton had " ^ 
                  (string_of_int nhits) ^ 
                  " matches for constrol state " ^ 
                  (control_state_to_string p) ^ 
                  " in get_init_mu_p.\n");;

(* from a game ma state q, get the corresponding initial state from ma_mu *)
let get_init_mu_game q ma_mu = 
    match q with 
      Mc_parity.QStar -> Mc_pds_mucalc.QStar
    | Mc_parity.QEpsilon -> Mc_pds_mucalc.QEpsilon
    | Mc_parity.QVal(p, _) -> get_init_mu_p p ma_mu;;

(* from a mu ma state q, get the corresponding initial state from ma_mu *)
let get_init_mu_mu q ma_mu = 
    match q with 
      Mc_pds_mucalc.QStar -> Mc_pds_mucalc.QStar
    | Mc_pds_mucalc.QEpsilon -> Mc_pds_mucalc.QEpsilon
    | Mc_pds_mucalc.QFail -> failwith "get_init_mu_mu doesn't know what to do with QFail state."
    | Mc_pds_mucalc.QVal(p', _, _) -> get_init_mu_p p' ma_mu
    | Mc_pds_mucalc.QInt(_, _, _, _) -> failwith "get_init_mu_mu doesn't know what to do with QInt state.";;


(* from a control state p, get the corresponding initial state from ma_game *)
let get_init_game_p p ma_game = 
    let filter s = 
        match s with 
          Mc_parity.QStar -> false
        | Mc_parity.QEpsilon -> false
        | Mc_parity.QVal(p', _) -> (p = p') in
    let (matches, _) = PGStateSet.partition filter (ma_game#get_inits) in
    let nhits = PGStateSet.cardinal matches in
    if nhits = 1 then
        PGStateSet.choose matches
    else
        failwith ("Game automaton had " ^ 
                  (string_of_int nhits) ^ 
                  " matches for constrol state " ^ 
                  (control_state_to_string p) ^ 
                  " in get_init_game_p.\n");;

(* from a game ma state q, get the corresponding initial state from ma_game *)
let get_init_game_game q ma_game = 
    match q with 
      Mc_parity.QStar -> Mc_parity.QStar
    | Mc_parity.QEpsilon -> Mc_parity.QEpsilon
    | Mc_parity.QVal(p, _) -> get_init_game_p p ma_game;;

(* from a mu ma state q, get the corresponding initial state from ma_mu *)
let get_init_game_mu q ma_game = 
    match q with 
      Mc_pds_mucalc.QStar -> Mc_parity.QStar
    | Mc_pds_mucalc.QEpsilon -> Mc_parity.QEpsilon
    | Mc_pds_mucalc.QFail -> failwith "get_init_game_mu doesn't know what to do with QFail state."
    | Mc_pds_mucalc.QVal(p, f, _) -> get_init_game_p (mu_to_pdg_control p f) ma_game
    | Mc_pds_mucalc.QInt(_, _, _, _) -> failwith "get_init_game_mu doesn't know what to do with QInt state.";;




(* Just compare ma on the transitions where the meaning is clear -- don't
 * do an exponential exact comparison...
 *)
let ma_disjoint : PdsMuMa.ma -> PdsMuMa.ma -> bool =
    fun x y -> 
        let qstar = PdsMuStateSet.singleton Mc_pds_mucalc.QStar in
        let qeps = PdsMuStateSet.singleton Mc_pds_mucalc.QEpsilon in
        let check_tran x y tx res = 
            let (txq, txa, txss) = PdsMuMa.ht_tuple tx in
            let answer = 
                if not res then
                    false
                else if not (PdsMuStateSet.mem txq x#get_inits) then
                    res
                else (
                    let qy = get_init_mu_mu txq y in
                    let tyss = y#get_char_image qy txa in
                    if PdsMuSetofStateSets.mem qstar txss then
                        not (PdsMuSetofStateSets.mem qstar tyss) 
                    else if PdsMuSetofStateSets.mem qeps txss then
                        not (PdsMuSetofStateSets.mem qeps tyss)
                    (* q-sbot->qe should be in ma or its negation *)
                    else if txa = sbot then 
                        PdsMuSetofStateSets.mem qeps tyss
                    else    
                        true
                ) in
            if (not answer) && res then (
                (*print_string ("Ma first: " ^ x#to_string);
                print_string ("\nMa second: " ^ y#to_string);*)
                print_string ("\ntwo not distinct on trans:\n\n");
                print_string (PdsMuMa.ht_to_string tx);
                print_string "\n -------\n";
                let tyss = y#get_char_image txq txa in
                print_string (PdsMuSetofStateSets.to_string tyss);
                print_newline ();
            );
            answer in
        (x#tran_fold (check_tran x y) true) &&
        (y#tran_fold (check_tran y x) true);;

(* Just compare ma on the transitions where the meaning is clear -- don't
 * do an exponential exact comparison...
 *)
let ma_disjoint_game : ParityMa.ma -> ParityMa.ma -> bool =
    fun x y -> 
        let qstar = PGStateSet.singleton Mc_parity.QStar in
        let qeps = PGStateSet.singleton Mc_parity.QEpsilon in
        let check_tran x y tx res = 
            let (txq, txa, txss) = ParityMa.ht_tuple tx in
            if not res then
                false
            else if not (PGStateSet.mem txq x#get_inits) then
                res
            else (
                let qy = get_init_game_game txq y in
                let tyss = y#get_char_image qy txa in
                if PGSetofStateSets.mem qstar txss then
                    not (PGSetofStateSets.mem qstar tyss) 
                else if PGSetofStateSets.mem qeps txss then
                    not (PGSetofStateSets.mem qeps tyss)
                (* q-sbot->qe should be in ma or its negation *)
                else if txa = sbot then 
                    PGSetofStateSets.mem qeps tyss
                else    
                    true
        ) in
        x#tran_fold (check_tran x y) true &&
        y#tran_fold (check_tran y x) true;;


(* checks whether a game ma and a mu ma are disjoint from given states and given char *)
let tran_neg_eq a ma_game q_game ma_mu q_mu = 
    let qeps = PGStateSet.singleton Mc_parity.QEpsilon in
    let qstar = PGStateSet.singleton Mc_parity.QStar in
    let qeps_mu = PdsMuStateSet.singleton Mc_pds_mucalc.QEpsilon in
    let qstar_mu = PdsMuStateSet.singleton Mc_pds_mucalc.QStar in
    let game_image = ma_game#get_char_image q_game a in
    let mu_image = ma_mu#get_char_image q_mu a in
    let dir1 = if PGSetofStateSets.mem qstar game_image then
                   not (PdsMuSetofStateSets.mem qstar_mu mu_image)
               else if PGSetofStateSets.mem qeps game_image then
                   not (PdsMuSetofStateSets.mem qeps_mu mu_image)
               else if a = sbot then 
                   PdsMuSetofStateSets.mem qeps_mu mu_image
               else    
                   true in
    let dir2 = if PdsMuSetofStateSets.mem qstar_mu mu_image then
                   not (PGSetofStateSets.mem qstar game_image)
               else if PdsMuSetofStateSets.mem qeps_mu mu_image then
                   not (PGSetofStateSets.mem qeps game_image)
               else if a = sbot then 
                   PGSetofStateSets.mem qeps game_image
               else    
                   true in
    if not (dir1 && dir2) then (
        print_string ("Ma Game: " ^ ma_game#to_string);
        print_string ("\nMa Mu: " ^ ma_mu#to_string);
        print_string ("\ntwo not distinct on char " ^ (character_to_string a) ^ ":\n\n");
        print_string (Mc_parity.state_to_string q_game);
        print_string " -- ";
        print_string (PGSetofStateSets.to_string game_image);
        print_string "\n\n";
        print_string (Mc_pds_mucalc.state_to_string q_mu);
        print_string " -- ";
        print_string (PdsMuSetofStateSets.to_string mu_image);
        print_newline ();
    );
    dir1 && dir2;;

(* 
 * alphabet is the alphabet the automata are over 
 *)
let game_to_mu_neg_eq : ParityMa.ma -> PdsMuMa.ma -> CharacterSet.t -> bool =
    fun x y alphabet -> 
        let do_pair q a res = 
            if not res then
                false
            else if q = Mc_parity.QStar then
                res
            else
                let q_mu = get_init_mu_game q y in
                tran_neg_eq a x q y q_mu in
        PGStateSet.fold (fun q -> CharacterSet.fold (do_pair q) alphabet) 
                        (x#get_inits) 
                        true;;
            

                


(* 
 * alphabet is the alphabet the automata are over 
 *)
let mu_to_game_neg_eq : PdsMuMa.ma -> ParityMa.ma -> CharacterSet.t -> bool =
    fun x y alphabet -> 
        let do_pair q a res = 
            if not res then
                false
            else
                let q_game = get_init_game_mu q y in
                tran_neg_eq a y q_game x q in
        PdsMuStateSet.fold (fun q -> CharacterSet.fold (do_pair q) alphabet) 
                           (x#get_inits) 
                           true;;
 

let random_pds_mucalc () = 
    default_random_pds_mucalc 3 0.1 5 7 7 0 2 1.0 2.0;;

let random_pdg () =
    random_pdg_ncols 7 7 0 2 1.0 2.0 2 3 0.5;;


let negate_pdg pdg = 
    let pds = pdg#get_pds in
    let props = pdg#get_props in
    let do_control p ps = 
        let owner = match props#get_owner p with
                      E -> A
                    | A -> E in
        let colour = (props#get_colour p + 1) in
        (p, owner, colour)::ps in
    let prop_list = ControlStateSet.fold do_control (pds#get_control_states) [] in
    new parity_game pds (new property_map prop_list) [];;

let negate_pdmu pdmu = 
    new pds_mucalc pdmu#get_pds [] (Un(Not, pdmu#get_fmla)) pdmu#get_props;;
          


(*********************************************************************************
 * Random tests
 *)


let test1 () = 
    let pds = new pds in
    pds#add_rule p_sbot_f_sbot;
    pds#add_rule p_a_p;
    pds#add_rule f_a_p_a;
    pds#add_rule f_a_f_aa;
    pds#add_rule f_sbot_f_sbot;
    let prop_map = new proposition_map [(p, a, ["p"]);
                                        (p, sbot, ["p"]);
                                        (f, a, ["f"]);
                                        (f, sbot, ["f"])] in
    let fmla = Mu("Z1", Nu("Z2", Bin(Bin(Prop("p"), And, Un(Box, Var("Z1"))),
                                     Or,
                                     Bin(Prop("f"), And, Un(Box, Var("Z2")))))) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] fmla prop_map) in
    mc#set_disable_hard_opt true;
    mc#set_prod_union_opt true;
    let ma = mc#construct_denotation in
    let mcneg = new mc_pds_mucalc (new pds_mucalc pds [] (Un(Not, fmla)) prop_map) in
    mcneg#set_disable_hard_opt true;
    mcneg#set_prod_union_opt true;
    let maneg = mcneg#construct_denotation in
    ma_disjoint ma maneg;;

RandomTest.add_test "Test 1: journal version example and its negation" test1 true;;



let random_instance () = 
    let pdmu = random_pds_mucalc () in
    let pdneg = negate_pdmu pdmu in
    let mc = new mc_pds_mucalc pdmu in
    mc#set_disable_hard_opt true;
    mc#set_prod_union_opt true;
    let ma = mc#construct_denotation in
    let mcneg = new mc_pds_mucalc pdneg in
    mcneg#set_disable_hard_opt true;
    mcneg#set_prod_union_opt true;
    let maneg = mcneg#construct_denotation in
    ma_disjoint ma maneg;;

RandomTest.add_test "Test 2: A random pds mucalc instance and its negation" random_instance true;;
RandomTest.add_test "Test 3: A random pds mucalc instance and its negation" random_instance true;;
RandomTest.add_test "Test 4: A random pds mucalc instance and its negation" random_instance true;;
RandomTest.add_test "Test 5: A random pds mucalc instance and its negation" random_instance true;;
RandomTest.add_test "Test 6: A random pds mucalc instance and its negation" random_instance true;;



let random_game () = 
    let pdg = random_pdg () in
    let negpdg = negate_pdg pdg in
    let mc = new mc_parity pdg in
    mc#set_disable_hard_opt true;
    mc#set_prod_union_opt true;
    let ma = mc#construct_winning_region in
    let mcneg = new mc_parity negpdg in
    mcneg#set_disable_hard_opt true;
    mcneg#set_prod_union_opt true;
    let maneg = mcneg#construct_winning_region in
    ma_disjoint_game ma maneg;;

RandomTest.add_test "Test 7: A random pushdown game and its negation" random_game true;;
RandomTest.add_test "Test 8: A random pushdown game and its negation" random_game true;;
RandomTest.add_test "Test 9: A random pushdown game and its negation" random_game true;;
RandomTest.add_test "Test 10: A random pushdown game and its negation" random_game true;;
RandomTest.add_test "Test 11: A random pushdown game and its negation" random_game true;;



let random_mucalc_to_game () =
    let pdmu = random_pds_mucalc () in
    let mc = new mc_pds_mucalc pdmu in
    mc#set_disable_hard_opt true;
    mc#set_prod_union_opt true;
    let ma = mc#construct_denotation in
    let pdg = negate_pdg (pdmu_to_pdg pdmu) in
    let mc_game = new mc_parity pdg in
    mc_game#set_disable_hard_opt true;
    mc_game#set_prod_union_opt true;
    let ma_game = mc_game#construct_winning_region in
    mu_to_game_neg_eq ma ma_game (pdmu#get_pds#get_alphabet);;

RandomTest.add_test "Test 12: A random mu calc to game test." random_mucalc_to_game true;;
RandomTest.add_test "Test 13: A random mu calc to game test." random_mucalc_to_game true;;
RandomTest.add_test "Test 14: A random mu calc to game test." random_mucalc_to_game true;;
RandomTest.add_test "Test 15: A random mu calc to game test." random_mucalc_to_game true;;
RandomTest.add_test "Test 16: A random mu calc to game test." random_mucalc_to_game true;;


let random_game_to_mucalc () =
    let pdg = random_pdg () in
    let mc = new mc_parity pdg in
    mc#set_disable_hard_opt true;
    mc#set_prod_union_opt true;
    let ma = mc#construct_winning_region in
    let pdmu = negate_pdmu (pdg_to_pdmu pdg) in
    let mc_mu = new mc_pds_mucalc pdmu in
    mc_mu#set_disable_hard_opt true;
    mc_mu#set_prod_union_opt true;
    let ma_mu = mc_mu#construct_denotation in
    game_to_mu_neg_eq ma ma_mu (pdmu#get_pds#get_alphabet);;

RandomTest.add_test "Test 17: A random game to mu calc test." random_game_to_mucalc true;;
RandomTest.add_test "Test 18: A random game to mu calc test." random_game_to_mucalc true;;
RandomTest.add_test "Test 19: A random game to mu calc test." random_game_to_mucalc true;;
RandomTest.add_test "Test 20: A random game to mu calc test." random_game_to_mucalc true;;
RandomTest.add_test "Test 21: A random game to mu calc test." random_game_to_mucalc true;;



