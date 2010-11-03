
open Pds;;
open Parity_game;;
open Constants;;
open Parity_constants;;

module Mappings_ResultType =
    struct
        type t = owner * int
        let equal x y = (x = y)
        let to_string (o, c) =
            "(" ^ (owner_to_string o) ^ ", " ^ (string_of_int c) ^ ")"
    end;;

module MapTest = Test.Tester(Mappings_ResultType);;

(* Tests defined below *)
let test_parity_game () = 
    print_string "Testing PARITY_GAME:\n\n";
    MapTest.run_tests ();
    print_string "Done.\n\n";;


(****************************************)


(* Test 1 -- Just doing some mappings *)

let test1 () = 
    let pm = new property_map [(p, A, 1); (p', E, 2)] in
    (pm#get_owner p, pm#get_colour p);;
let expected1 = (A, 1);;

MapTest.add_test "Test 1 -- Just doing some mappings" test1 expected1;;



