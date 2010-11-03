
open Ma;;
open Pds;;
open Parity_game;;
open Words;;
open State;;
open Mc_parity;;
open Mc_parity.ParityMa;;
open Mc_parity.ParityMa.MaState;;
open Constants;;
open Parity_constants;;

module Ma_ResultType = 
    struct
        type t = ma
        let equal x y = x#equals y
        let to_string x = x#to_string
    end;;

module MCTest = Test.Tester(Ma_ResultType);;

module Bool_ResultType =
    struct 
        type t = bool
        let equal x y = (x = y)
        let to_string = string_of_bool
    end;;

module BoolMCTest = Test.Tester(Bool_ResultType);;

module PDG_ResultType =
    struct
        type t = parity_game
        let equal pdg1 pdg2 = pdg1#equal pdg2
        let to_string pdg1 =  pdg1#to_string
    end;;

module PDGMCTest = Test.Tester(PDG_ResultType);;


let test_mc_parity () = 
    print_string "Testing MC Parity:\n\n";
    MCTest.run_tests ();
    BoolMCTest.run_tests ();
    PDGMCTest.run_tests ();
    print_string "Done.\n\n";;



(********************************************************)



(* Test 1 -- Empty everything *)

let test1 () = 
    let mc = new mc_parity (new parity_game (new pds) (new property_map []) []) in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon]);
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected1 = 
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar; QEpsilon]);
    ma;;

MCTest.add_test "Test 1 -- Empty everything" test1 expected1;;



(* Test 2 -- Single rule, single tran -- push *)

let test2 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p ab);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1);
    ma#add_hyper_transition (tran_qp1_b_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1]);
    let props = new property_map [(p, E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected2 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1);
    ma#add_hyper_transition (tran_qp1_b_qstar);
    ma#add_hyper_transition (tran_qp2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2]);
    ma;;

MCTest.add_test "Test 2 -- Single rule, single tran -- push" test2 expected2;;


(* Test 3 -- Single rule, single tran -- pop *)

let test3 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p e);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_b_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1]);
    let props = new property_map [(p, E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected3 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp2_a_qp1);
    ma#add_hyper_transition (tran_qp1_b_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2]);
    ma;;

MCTest.add_test "Test 3 -- Single rule, single tran -- pop" test3 expected3;;


(* Test 4 -- Single rule, single tran -- alterating, one rule *)

let test4 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p sbot p word_sbot);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1]);
    let props = new property_map [(p, A, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected4 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp2_a_qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2]);
    ma;;

MCTest.add_test "Test 4 -- Single rule, single tran -- alternating, one rule" test4 expected4;;



(* Test 5 -- Two rule, single tran -- alterating, by player *)

let test5 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p a p' word_a);
    pds#add_rule (make_rule p sbot p word_sbot);
    pds#add_rule (make_rule p' sbot p' word_sbot);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'1]);
    let props = new property_map [(p, A, 1);(p', E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected5 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_hyper_transition (tran_qp2_a_qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2;qp'1;qp'2]);
    ma;;

MCTest.add_test "Test 5 -- Two rules, single tran -- alternating, by player" test5 expected5;;



(* Test 6 -- Single rule, two tran -- eloise *)

let test6 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1]);
    let props = new property_map [(p, E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected6 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1_qstar);
    ma#add_hyper_transition (tran_qp2_a_qp1_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2]);
    ma;;

MCTest.add_test "Test 6 -- Single rule, two tran -- eloise" test6 expected6;;



(* Test 7 -- Single rule, two tran -- abelard *)

let test7 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p a p' word_a);
    pds#add_rule (make_rule p sbot p word_sbot);
    pds#add_rule (make_rule p' sbot p' word_sbot);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1_qstar);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'1]);
    let props = new property_map [(p, A, 1);(p', E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected7 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1_qstar);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_hyper_transition (tran_qp2_a_qp1qstar_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2;qp'1;qp'2]);
    ma;;

MCTest.add_test "Test 7 -- Two rules, two tran -- abelard" test7 expected7;;





(* Test 8 -- Two rule, two tran -- eloise *)

let test8 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p a p' word_a);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'1]);
    let props = new property_map [(p, E, 1);(p', E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected8 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_hyper_transition (tran_qp2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp2;qp'1;qp'2]);
    ma;;

MCTest.add_test "Test 8 -- Two rules, two tran -- eloise" test8 expected8;;



(* Test 9 -- Two rule, two tran, two colours -- eloise *)

let test9 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p a p' word_a);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    let props = new property_map [(p, E, 1);(p', E, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected9 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_hyper_transition (tran_qp3_a_qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2;qp3;qp'3]);
    ma;;

MCTest.add_test "Test 9 -- Two rules, two tran, two colours -- eloise" test9 expected9;;


(* Test 10 -- Two rule, two colours -- eloise & abelard *)

let test10 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p' a p word_a);
    pds#add_rule (make_rule p' a p' word_a);
    pds#add_rule (make_rule p sbot p word_sbot);
    pds#add_rule (make_rule p' sbot p' word_sbot);
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    let props = new property_map [(p, E, 1);(p', A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected10 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_hyper_transition (tran_qp3_a_qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2;qp3;qp'3]);
    ma;;

MCTest.add_test "Test 10 -- Two rules, two colours -- eloise & abelard" test10 expected10;;



(* Test 11 -- Single rule, no tran -- eloise *)

let test11 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    let ma = new ma in
    let props = new property_map [(p, E, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    ma#add_states (StateSet.from_list [QStar;QEpsilon]);
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected11 = 
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp2]);
    ma;;

MCTest.add_test "Test 11 -- Single rule, no tran -- eloise" test11 expected11;;




(* Test 12 -- Single rule, no tran -- eloise *)

let test12 () = 
    let pds = new pds in
    pds#add_rule (make_rule p a p word_a);
    pds#add_rule (make_rule p sbot p word_sbot);
    let ma = new ma in
    let props = new property_map [(p, A, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    ma#add_states (StateSet.from_list [QStar;QEpsilon]);
    mc#set_disable_hard_opt true;
    mc#compute_phi ma init_ma_info;
    ma;;
let expected12 = 
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp2]);
    ma;;

MCTest.add_test "Test 12 -- Single rule, no tran -- abelard" test12 expected12;;




(**********************************************************************)

let test13 () = 
    let pds = new pds in
    let props = new property_map [(p, A, 1); (p', E, 2)] in
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#project_and_delete 3 ma;
    ma;;
let expected13 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    ma;;

MCTest.add_test "Test 13 -- projections, priority too high to change anything" test13 expected13;;





let test14 () = 
    let pds = new pds in
    let props = new property_map [(p, A, 1); (p', E, 2)] in
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#project_and_delete 2 ma;
    ma;;
let expected14 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    ma;;

MCTest.add_test "Test 14 -- projections, priority just deletes" test14 expected14;;




let test15 () = 
    let pds = new pds in
    let props = new property_map [(p, A, 1); (p', E, 2)] in
    let ma = new ma in
    ma#add_hyper_transition (tran_qp1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'2_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2]);
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#project_and_delete 1 ma;
    ma;;
let expected15 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp'1_a_qp1qstar);
    ma#add_hyper_transition (tran_qp'1_a_qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'1]);
    ma;;

MCTest.add_test "Test 15 -- projections delete 1 and rename 2 to 1" test15 expected15;;



let test16 () = 
    let pds = new pds in
    let props = new property_map [(p, A, 1); (p', E, 2)] in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'3;qp3]);
    ma#add_hyper_transition (tran_qp'3_a_qp3qp1qstar);
    let mc = new mc_parity (new parity_game pds props []) in
    mc#set_disable_hard_opt true;
    mc#project_and_delete 2 ma;
    ma;;
let expected16 = 
    let ma = new ma in
    ma#add_hyper_transition (tran_qp'2_a_qp2qp1qstar);
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qp1;qp'2;qp2]);
    ma;;

MCTest.add_test "Test 16 -- projections do not change target state with colour too small" test16 expected16;;



(****************************************************************************)

let test17 () = 
    let pds = new pds in
    pds#add_rule p_sbot_f_sbot;
    pds#add_rule p_a_p;
    pds#add_rule f_a_p_a;
    pds#add_rule f_a_f_aa;
    pds#add_rule f_sbot_f_sbot;
    let props = new property_map [(p, A, 1); (f, A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#construct_winning_region;;
let expected17 = 
    let ma = new ma in
    ma#add_hyper_transition tran_qstar_sbot_qe;
    ma#add_hyper_transition tran_qstar_a_qstar;
    ma#add_hyper_transition tran_qf1_sbot_qe;
    ma#add_hyper_transition tran_qf1_a_qstarqp1;
    ma#add_hyper_transition tran_qp1_sbot_qe;
    ma#add_hyper_transition tran_qp1_a_qp1;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qf1;qp1]);
    ma;;

MCTest.add_test "Test 17 -- full model check, Arnaud's example" test17 expected17;;





let test18 () = 
    let pds = new pds in
    pds#add_rule (make_rule p'E a p'E word_b);
    pds#add_rule (make_rule p'E b pE word_a);
    pds#add_rule (make_rule pE a p'E word_b);
    pds#add_rule (make_rule pE a pA word_a);
    pds#add_rule (make_rule pA a pE word_a);
    pds#add_rule (make_rule pA a pA word_b);
    pds#add_rule (make_rule pA b pA word_b);
    pds#add_rule (make_rule pE b pA word_b);
    pds#add_rule (make_rule pE sbot pE word_sbot);
    pds#add_rule (make_rule p'E sbot p'E word_sbot);
    pds#add_rule (make_rule pA sbot pA word_sbot);
    let props = new property_map [(pE, E, 1); (p'E, E, 0); (pA, A, 1)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#construct_winning_region;;
let expected18 = 
    let ma = new ma in
    ma#add_hyper_transition tran_qstar_sbot_qe;
    ma#add_hyper_transition tran_qstar_a_qstar;
    ma#add_hyper_transition tran_qstar_b_qstar;
    ma#add_hyper_transition tran_p'E_sbot_qe;
    ma#add_hyper_transition tran_p'E_a_qstar;
    ma#add_hyper_transition tran_p'E_b_qstar;
    ma#add_hyper_transition tran_pE_a_qstar;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;qpE;qp'E;qpA]);
    ma;;

MCTest.add_test "Test 18 -- full model check, simple paper example" test18 expected18;;


(******************************************************************************************)

let test19 () = 
    let pds = new pds in
    let props = new property_map [] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;;

BoolMCTest.add_test "Test 19 -- check an empty pushdown game errors" test19 true;;



let test20 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f a_a_bot);
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(p, A, 2); (f, A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;;

BoolMCTest.add_test "Test 20 -- good parity game has no checkable errors" test20 true;;



let test21 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(p, A, 3); (f, A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;; 
BoolMCTest.add_test "Test 21 -- incomplete pushdown system" test21 true;;


let test22 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f e);
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(p, A, 4); (f, A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;;

BoolMCTest.add_test "Test 22 -- parity game pops bot" test22 false;;


let test23 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f (make_word [a; sbot; b]));
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(p, A, 5); (f, A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;;

BoolMCTest.add_test "Test 23 -- puts bot back in the wrong place" test23 false;;


let test24 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule p a f a_bot_b);
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(p, A, 6); (f, A, 2)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;;

BoolMCTest.add_test "Test 24 -- pushes a bot for no reason" test24 false;;


let test25 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(f, A, 7)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#checkable_errors = None;;

BoolMCTest.add_test "Test 25 -- missing properties" test25 false;;







(*********************************************************************)

let test26 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(f, A, 7)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#ensure_complete;
    mc#get_pdg;;
let expected26 =       
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule p a p e);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot f word_sbot);
    let props = new property_map [(f, A, 7)] in
    new parity_game pds props [];;

PDGMCTest.add_test "Test 26 -- complete a complete pdg results in no change." test26 expected26;;


let test27 () = 
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    let props = new property_map [(f, A, 7); (p, E, 9)] in
    let mc = new mc_parity (new parity_game pds props []) in
    mc#ensure_complete;
    mc#get_pdg;;
let expected27 =       
    let pds = new pds in
    pds#add_rule (make_rule p sbot f word_sbot);
    pds#add_rule (make_rule p a eloise_stuck word_a);
    pds#add_rule (make_rule f a p word_a);
    pds#add_rule (make_rule f a f aa);
    pds#add_rule (make_rule f sbot abelard_stuck word_sbot);
    pds#add_rule (make_rule abelard_stuck a abelard_stuck word_a);
    pds#add_rule (make_rule abelard_stuck sbot abelard_stuck word_sbot);
    pds#add_rule (make_rule eloise_stuck a eloise_stuck word_a);
    pds#add_rule (make_rule eloise_stuck sbot eloise_stuck word_sbot);
    let props = new property_map [(f, A, 7); 
                                  (p, E, 9); 
                                  (abelard_stuck, E, 8); 
                                  (eloise_stuck, A, 7)] in
    new parity_game pds props [];;

PDGMCTest.add_test "Test 27 -- incomplete pushdown system completed." test27 expected27;;
