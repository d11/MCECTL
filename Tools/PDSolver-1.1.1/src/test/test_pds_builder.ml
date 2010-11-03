
open Pds;;
open Words;;
open Constants;;
open Bp;;
open Bp_pds_builder;;

module ResultType =
    struct
        type t = pds
        let equal pds1 pds2 = (pds1#equal pds2)
        let to_string pds = pds#to_string
    end;;

module BuilderTest = Test.Tester(ResultType);;

(* Tests defined below *)
let test_pds_builder () = 
    print_string "Testing BP to PDS translation:\n\n";
    BuilderTest.run_tests ();
    print_string "Done.\n\n";;


(****************************************************************)


(*let test1 () = 
    let assign_stmt = (["stmt1"], Assign(["l1"],[True],True)) in
    let main_method = (Void, "main", ["a1"], ["l1"], True, False, [assign_stmt]) in 
    let bp = (["g1"], [main_method]) in
    let builder = new bp_pds_builder bp in
    builder#convert;;

let expected1 = new pds;;

BuilderTest.add_test "Test 1 -- Assign true" test1 expected1;;
*)


    

      


