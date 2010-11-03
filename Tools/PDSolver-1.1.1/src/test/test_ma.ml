
open Ma;;
open Pds;;
open Words;;
open State;;
open Constants;;
open Parity_constants;;
open Mc_parity;;
open Mc_parity.ParityMa;;
open Mc_parity.ParityMa.MaState;;

(* Hyper transition tester *)
module HTTest = Test.Tester(SetOfStateSets);;

(* Complete automaton tester *)
module Ma_ResultType = 
    struct
        type t = ma
        let equal x y = x#equals y
        let to_string x = x#to_string
    end;;


module MaTest = Test.Tester(Ma_ResultType);;

module HeadTest = Test.Tester(MaHeadSet);;


module Bool_ResultType =
    struct 
        type t = bool
        let equal x y = (x = y)
        let to_string = string_of_bool
    end;;

module BoolMCTest = Test.Tester(Bool_ResultType);;



(* Tests defined below *)
let test_ma () = 
    print_string "Testing MA:\n\n";
    HTTest.run_tests ();
    MaTest.run_tests ();
    HeadTest.run_tests ();
    BoolMCTest.run_tests ();
    print_string "Done.\n\n";;


(******************************)


(* Test 1 -- join properly *)

let test1 () = 
    let ma = new ma in
    ma#add_hyper_transition tran1a;
    ma#add_hyper_transition tran2a;
    ma#get_char_image q1 a;;
let expected1 = setofstatesets_from_list_list [[q1; q2]; [q3]; [q1; q2; q3]];;

HTTest.add_test "test1" test1 expected1;;


(* Test 2 -- don't confuse states *)

let test2 () = 
    let ma = new ma in
    ma#add_hyper_transition tran1a;
    ma#add_hyper_transition tran3a;
    ma#get_char_image q1 a;;
let expected2 = setofstatesets_from_list_list [[q1; q2]; [q3]];;

HTTest.add_test "test2" test2 expected2;;



(* Test 3 -- add loads, don't get confused *)

let test3 () = 
    let ma = new ma in
    ma#add_hyper_transition tran1a;
    ma#add_hyper_transition tran2a;
    ma#add_hyper_transition tran3a;
    ma#add_hyper_transition tran1b;
    ma#add_hyper_transition tran2b;
    ma#get_char_image q2 b;;
let expected3 = setofstatesets_from_list_list [[q1; q2; q3]; [q1; q4]];;

HTTest.add_test "test3" test3 expected3;;


(* Test 4 -- doesn't crash if no tran *)

let test4 () = 
    let ma = new ma in
    ma#get_char_image q2 b;;
let expected4 = SetOfStateSets.empty;;

HTTest.add_test "test4" test4 expected4;;


(* Test 5 -- two transitions from same set/char, one empty target *)

let test5 () = 
    let ma = new ma in
    ma#add_hyper_transition tran2b;
    ma#add_hyper_transition tran3b;
    ma#get_char_image q2 b;;
let expected5 = setofstatesets_from_list_list [[q4; q1];[q1;q2;q3]];;

HTTest.add_test "test5" test5 expected5;;



(****************************************************************)
(* Test run enumeration *)


(* Test 6 -- Run enumeration, empty automaton *)

let test6 () = 
    let ma = new ma in
    ma#get_word_image q1 ab false;;
let expected6 = SetOfStateSets.empty;;

HTTest.add_test "Test 6 -- Run enumeration, empty automaton" test6 expected6;;



(* Test 7 -- Run enumeration, stuck run *)

let test7 () = 
    let ma = new ma in
    ma#add_hyper_transition tran1a;
    ma#add_hyper_transition tran2b;
    ma#get_word_image q1 ab false;;
let expected7 = SetOfStateSets.empty;;

HTTest.add_test "Test 7 -- Run enumeration, stuck run" test7 expected7;;



(* Test 8 -- Run enumeration, single run *)

let test8 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q2q4;
    ma#add_hyper_transition tran_q1_b_q3q4;
    ma#get_word_image q1 ab false;;
let expected8 = setofstatesets_from_list_list [[q2; q3; q4]];;

HTTest.add_test "Test 8 -- Run enumeration, single run" test8 expected8;;



(* Test 9 -- Run enumeration, four branches *)

let test9 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1_q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#get_word_image q1 ab false;;
let expected9 = setofstatesets_from_list_list [[q1]; [q2]; [q3]; [q4]];;

HTTest.add_test "Test 9 -- Run enumeration, four branches" test9 expected9;;



(* Test 10 -- Run enumeration, two step, first alternating *)

let test10 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#get_word_image q1 ab false;;
let expected10 = setofstatesets_from_list_list [[q1; q3]; [q1; q4]; [q2; q3]; [q2; q4]];;

HTTest.add_test "Test 10 -- Run enumeration, two step, first alternating" test10 expected10;;


(* Test 11 -- Run enumeration, empty word *)

let test11 () = 
    let ma = new ma in
    ma#add_hyper_transition tran1a;
    ma#add_hyper_transition tran2b;
    ma#get_word_image q1 e false;;
let expected11 = setofstatesets_from_list_list [[q1]];;

HTTest.add_test "Test 11 -- Run enumeration, empty word" test11 expected11;;






(********************************************************************************)

(* Test our self map thing *)

let test12 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q2q4;
    ma#add_hyper_transition tran_q1_b_q3q4;
    let t_id t = (`Unchanged) in
    ma#self_map t_id;
    ma;;
let expected12 = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q2q4;
    ma#add_hyper_transition tran_q1_b_q3q4;
    ma;;

MaTest.add_test "Test 12 -- Self map, tran id" test12 expected12;;



let test13 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q2q4;
    ma#add_hyper_transition tran_q1_b_q3q4;
    let t_id t = (`Remove) in
    ma#self_map t_id;
    ma;;
let expected13 = new ma;;

MaTest.add_test "Test 13 -- Self map, tran collapse" test13 expected13;;



let test14 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q2q4;
    ma#add_hyper_transition tran_q1_b_q3q4;
    let t_do t = 
        let (q, a, ss) = ht_tuple t in
        if q = q1 then
            (`NewTran(make_ht q2 a ss))
        else 
            (`Remove) in
    ma#self_map t_do;
    ma;;
let expected14 = 
    let ma = new ma in
    ma#add_hyper_transition tran_q2_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q3q4;
    ma;;


MaTest.add_test "Test 14 -- Self map, delete one, rename another" test14 expected14;;




(********************************************************************************************
 * Test non-empty heads function
 *)

let test15 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1_q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#get_accepting_heads;;
let expected15 = MaHeadSet.from_list [(q1, a); (q1, b); (q2, b)];;

HeadTest.add_test "Test 15 -- simple non-det automaton, accepting heads." test15 expected15;;



let test16 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1_q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list []);
    ma#get_accepting_heads;;
let expected16 = MaHeadSet.from_list [];;

HeadTest.add_test "Test 16 -- simple non-det automaton, accepting heads, no finals." test16 expected16;;




let test17 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#add_hyper_transition tran_q4_b_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#get_accepting_heads;;
let expected17 = MaHeadSet.from_list [(q1, a); (q1, b); (q2, b)];;

HeadTest.add_test "Test 17 -- one alt automaton, accepting heads." test17 expected17;;




let test18 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#get_accepting_heads;;
let expected18 = MaHeadSet.from_list [(q2, b)];;

HeadTest.add_test "Test 18 -- one alt automaton, accepting heads, no path" test18 expected18;;



let test19 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q2q3;
    ma#add_hyper_transition tran_q2_b_q3;
    ma#add_hyper_transition tran_q3_b_q4;
    ma#add_hyper_transition tran_q4_b_q3;
    ma#set_inits (StateSet.from_list [q1]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#get_accepting_heads;;
let expected19 = MaHeadSet.from_list [];;

HeadTest.add_test "Test 19 -- one alt automaton, accepting heads, no accept because of alt loop" test19 expected19;;



(********************************************************************************************
 * Test accepts function
 *)

let test20 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1_q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#accepts q1 bab;;
let expected20 = true;;

BoolMCTest.add_test "Test 20 -- simple non-det automaton, accepts." test20 expected20;;


let test21 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1_q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#accepts q1 ba;;
let expected21 = false;;

BoolMCTest.add_test "Test 21 -- simple non-det automaton, accepts, false." test21 expected21;;



let test22 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1_q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list []);
    ma#accepts q1 bab;;
let expected22 = false;;

BoolMCTest.add_test "Test 22 -- simple non-det automaton, accepts, no finals." test22 expected22;;




let test23 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q1_b_q1_q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#add_hyper_transition tran_q4_b_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#accepts q1 babbb;;
let expected23 = true;;

BoolMCTest.add_test "Test 23 -- one alt automaton, accept." test23 expected23;;




let test24 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q1q2;
    ma#add_hyper_transition tran_q2_b_q3_q4;
    ma#set_inits (StateSet.from_list [q1; q2]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#accepts q1 babbb;;
let expected24 = false;;

BoolMCTest.add_test "Test 24 -- one alt automaton, accept, no path" test24 expected24;;



let test25 () = 
    let ma = new ma in
    ma#add_hyper_transition tran_q1_a_q2q3;
    ma#add_hyper_transition tran_q2_b_q3;
    ma#add_hyper_transition tran_q3_b_q4;
    ma#add_hyper_transition tran_q4_b_q3;
    ma#set_inits (StateSet.from_list [q1]);
    ma#set_finals (StateSet.from_list [q4]);
    ma#accepts q1 abb;;
let expected25 = false;;

BoolMCTest.add_test "Test 25 -- one alt automaton, accepts, no accept because of alt loop" test25 expected25;;




