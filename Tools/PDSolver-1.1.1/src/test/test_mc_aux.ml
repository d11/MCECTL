
open State;;
open Pds;;
open Constants;;
open Parity_constants;;
open Ma;;
open Mc_parity.ParityMa;;
open Mc_parity.ParityMa.MaState;;


module Bool_ResultType =
    struct 
        type t = bool
        let equal x y = (x = y)
        let to_string = string_of_bool
    end;;

module BoolMCTest = Test.Tester(Bool_ResultType);;


module HyperStateSet_ResultType = 
    struct
        type t = SetOfStateSets.t
        let equal = SetOfStateSets.equal
        let to_string = SetOfStateSets.to_string
    end;;

module HyperStateSetTest = Test.Tester(HyperStateSet_ResultType);;


let test_mc_aux () = 
    print_string "Testing MC Auxilliary Functions:\n\n";
    BoolMCTest.run_tests ();
    HyperStateSetTest.run_tests ();
    print_string "Done.\n\n";;


(**************************************************************)


(* Test state set less than *)

let test_less_than_sets ss_list ss'_list = 
    hyper_set_less_than (setofstatesets_from_list_list ss_list) 
                        (setofstatesets_from_list_list ss'_list);;


let construct_ma tran_list = 
    let ma = new ma in
    let do_tran t = 
        let (q, _, ss) = ht_tuple t in
        ma#add_states (StateSet.singleton q);
        SetOfStateSets.foreach ss (fun s ->
            ma#add_states s
        );
        ma#add_hyper_transition t in
    List.iter do_tran tran_list;
    ma


let test_ma_less_than tran_list tran_list' = 
    let ma = construct_ma tran_list in
    let ma' = construct_ma tran_list' in
    let ma_states = ma#get_states in
    let ma'_states = ma'#get_states in
    let states = StateSet.union ma_states ma'_states in
    ma#add_states states;
    ma'#add_states states;
    ma#less_than ma' alphabet_ab;;

let test1 () = test_less_than_sets [] [];;

BoolMCTest.add_test "Test 1: set less than, both empty" test1 true;;


let test2 () = test_less_than_sets [[]] [];;

BoolMCTest.add_test "Test 2: set less than, set of empty under empty" test2 false;;


let test3 () = test_less_than_sets [] [[]];;

BoolMCTest.add_test "Test 3: set less than, empty under set of empty" test3 true;;


let test4 () = test_less_than_sets [[qp1];[qp2]] [[qp1; qp2]];;

BoolMCTest.add_test "Test 4: set less than, straight subset, false" test4 false;;


let test5 () = test_less_than_sets [[qp1; qp2]] [[qp1];[qp1; qp'1; qp2]];;

BoolMCTest.add_test "Test 5: set less than, straight subset, true" test5 true;;


let test6 () = test_less_than_sets [[qp1; qstar]] [[qp1];[qp1; qp'1; qp2]];;

BoolMCTest.add_test "Test 6: set less than, qstar subset, true" test6 true;;


let test7 () = test_less_than_sets [[qp1; qstar]] [[qp1];[qp1; qstar; qp2]];;

BoolMCTest.add_test "Test 7: set less than, qstar subset, true but literally" test7 true;;


let test8 () = test_less_than_sets [[qp1; qstar];[qp2]] [[qp1];[qp1; qstar; qp2]];;

BoolMCTest.add_test "Test 8: set less than, qstar subset" test8 false;;


let test9 () = test_less_than_sets [[qstar]] [[qe]];; 

BoolMCTest.add_test "Test 9: set less than, qstar false" test9 false;;


let test10 () = test_less_than_sets [[qstar;qp1];[qe]] [[qe];[qp1]];; 

BoolMCTest.add_test "Test 10: set less than, qstar and qe, true" test10 true;;


let test11 () = test_less_than_sets [[qstar;qp1];[qe]] [[qe];[qp2]];; 

BoolMCTest.add_test "Test 11: set less than, qstar and qe, false" test11 false;;


let test12 () = test_less_than_sets [[qe];[qstar]] [[qe]];; 

BoolMCTest.add_test "Test 12: set less than, an extra false case" test12 false;;

let test125 () = 
    let opt = optimise_hyper_state_set (setofstatesets_from_list_list [[qstar; qp1]; [qstar]]) in
    SetOfStateSets.is_empty opt;;

BoolMCTest.add_test "Test 12.5: check that {s, s'} with s << s' and s' << s doesn't result in empty after opt." test125 false;;

(*******************************************************************)

let test13 () = test_ma_less_than [] [];;

BoolMCTest.add_test "Test 13: ma less than, empty vs. empty" test13 true;;


let test14 () = test_ma_less_than [] [tran_qp1_a_qp1_qstar];;

BoolMCTest.add_test "Test 14: ma less than, empty vs. non-empty" test14 true;;


let test15 () = test_ma_less_than [tran_qp2_b_qp2qp'2] [];;

(* true because qp2 is not in the intersection of the two statesets *)
BoolMCTest.add_test "Test 15: ma less than, non-empty vs. empty" test15 false;;


let test16 () = test_ma_less_than [tran_qp2_b_qp2qp'2; tran_qp1_a_qp1_qstar] 
                                  [tran_qp2_b_qp2qp'2; tran_qp1_a_qp1_qstar];;

BoolMCTest.add_test "Test 16: ma less than, identical, non-empty" test16 true;;


let test17 () = test_ma_less_than [tran_qp1_a_qstar] 
                                  [tran_qp2_b_qp2qp'2; tran_qp1_a_qp1_qstar];;

BoolMCTest.add_test "Test 17: ma less than, straight simulation" test17 true;;


let test18 () = test_ma_less_than [tran_qp1_a_qp1_qstar] 
                                  [tran_qp2_b_qp2qp'2; tran_qp1_a_qstar];;

BoolMCTest.add_test "Test 18: ma less than, straight simulation" test18 true;;



(****************************************************************************)


let test19 () = 
    optimise_hyper_state_set (setofstatesets_from_list_list [[qp1; qp2]; [qp3]; [qe]; [qp'2; qp'1]]);;
let expected19 = setofstatesets_from_list_list [[qp1; qp2]; [qp3]; [qe]; [qp'2; qp'1]];;

HyperStateSetTest.add_test "Test 19: Optimising an optimised hyper state set." test19 expected19;;


let test20 () = 
    optimise_hyper_state_set (setofstatesets_from_list_list [[qp2; qp3]; [qstar]; [qe]; [qp'3; qp'2]]);;
let expected20 = setofstatesets_from_list_list [[qstar]; [qe]];;

HyperStateSetTest.add_test "Test 20: Optimising a hyper set, q* does a hatchet job." test20 expected20;;


let test21 () = 
    optimise_hyper_state_set (setofstatesets_from_list_list []);;
let expected21 = setofstatesets_from_list_list [];;

HyperStateSetTest.add_test "Test 21: Optimising a hyper set, empty set" test21 expected21;;


let test22 () = 
    optimise_hyper_state_set (setofstatesets_from_list_list [[]; [qp2; qp3]; [qstar]; [qe]; [qp'3; qp'2]]);;
let expected22 = setofstatesets_from_list_list [[]];;

HyperStateSetTest.add_test "Test 22: Optimising a hyper set, has an empty set" test22 expected22;;


let test23 () = 
    optimise_hyper_state_set (setofstatesets_from_list_list [[qp1; qp2]; [qstar; qp1]; [qe]; [qp'1; qp'2]; [qp'1]]);;
let expected23 = (setofstatesets_from_list_list [[qstar; qp1]; [qe]; [qp'1]]);;

HyperStateSetTest.add_test "Test 23: Optimising a hyper set, qstar has a partner, another set zaps some others" test23 expected23;;



