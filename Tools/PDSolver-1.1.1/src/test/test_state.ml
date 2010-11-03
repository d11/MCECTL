
open State;;
open Pds;;
open Constants;;
open Parity_constants;;
open Mc_parity;;
open Mc_parity.ParityMa;;
open Mc_parity.ParityMa.MaState;;



module StateTest = Test.Tester(SetOfStateSets);;

(* Tests defined below *)
let test_state () = 
    print_string "Testing STATE:\n\n";
    StateTest.run_tests ();
    print_string "Done.\n\n";;


(****************************************)

(* Test 1 -- product union both empty *)

let test1 () = setofstatesets_prod_union ss_empty ss_empty 
let expected1 = ss_empty;;

StateTest.add_test "Product Union Test 1 -- Empties" test1 expected1;;


(* Test 2 -- product union singleton empty ({{}}) and empty *)

let test2 () = setofstatesets_prod_union ss_empty ss_singleton_empty
let expected2 = ss_empty;;

StateTest.add_test "Product Union Test 2 -- Singleton (empty) and empty" test2 expected2;;


(* Test 3 -- product union singleton and empty *)

let test3 () = setofstatesets_prod_union ss_empty ss_singleton_qstar;;
let expected3 = ss_empty;;

(* Test 3.5 -- product union empty and singleton *)

let test3'5 () = setofstatesets_prod_union ss_singleton_qstar ss_empty;;
let expected3'5 = ss_empty;;


StateTest.add_test "Product Union Test 3.5 -- Empty and singleton" test3'5 expected3'5;;


(* Test 4 -- product union singleton and singleton (empty) *)

let test4 () = setofstatesets_prod_union ss_singleton_empty ss_singleton_qstar;;
let expected4 = ss_singleton_qstar;;

StateTest.add_test "Product Union Test 4 -- Singleton and singleton (empty)" test4 expected4;;




(* Test 5 -- product union singleton and singleton *)

let test5 () = setofstatesets_prod_union ss_singleton_qepsilon ss_singleton_qstar;;
let expected5 = setofstatesets_from_list_list [[QStar; QEpsilon]];;

StateTest.add_test "Product Union Test 5 -- Singleton and singleton" test5 expected5;;



(* Test 6 -- product union singleton and singleton (equal) *)

let test6 () = setofstatesets_prod_union ss_singleton_qstar ss_singleton_qstar;;
let expected6 = setofstatesets_from_list_list [[QStar]];;

StateTest.add_test "Product Union Test 6 -- Singleton and singleton" test6 expected6;;




(* Test 7 -- product union singleton and two sets*)

let test7 () = setofstatesets_prod_union ss_singleton_qstar ss_two_sets;;
let expected7 = setofstatesets_from_list_list [[QStar; q1]; [QStar; q2]];;

StateTest.add_test "Product Union Test 7 -- Singleton and two sets" test7 expected7;;



(* Test 8 -- product union singleton and singleton (pair)*)

let test8 () = setofstatesets_prod_union ss_pair_set ss_singleton_qstar;;
let expected8 = setofstatesets_from_list_list [[QStar; q1; q2]];;

StateTest.add_test "Product Union Test 8 -- Singleton and singleton (pair)" test8 expected8;;



(* Test 9 -- product union singleton (pair) and singleton (pair)*)

let test9 () = setofstatesets_prod_union ss_pair_set ss_pair_set2;;
let expected9 = setofstatesets_from_list_list [[q1; q2; q3; q4]];;

StateTest.add_test "Product Union Test 9 -- Singleton (pair) and singleton (pair)" test9 expected9;;


(* Test 10 -- product union two sets and two sets *)

let test10 () = setofstatesets_prod_union ss_two_sets ss_two_sets2;;
let expected10 = setofstatesets_from_list_list [[q1; q3]; [q1; q4]; [q2; q3]; [q2; q4]];;

StateTest.add_test "Product Union Test 10 -- two sets and two sets" test10 expected10;;




(* Test 11 -- product singleton (pair) and two sets *)

let test11 () = setofstatesets_prod_union ss_pair_set ss_two_sets2;;
let expected11 = setofstatesets_from_list_list [[q1; q2; q3]; [q1; q2; q4]];;

StateTest.add_test "Product Union Test 11 -- singleton (pair) and two sets" test11 expected11;;



(* Test 12 -- product singleton (pair) and two sets with overlapping elements *)

let test12 () = setofstatesets_prod_union ss_pair_set ss_two_sets;;
let expected12 = setofstatesets_from_list_list [[q1; q2]];;

StateTest.add_test "Product Union Test 12 -- singleton (pair) and two sets" test12 expected12;;


