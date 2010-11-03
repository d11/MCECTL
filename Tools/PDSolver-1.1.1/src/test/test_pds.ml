
open Pds;;
open Words;;
open Constants;;


module ResultType =
    struct
        type t = NextSet.t * ControlStateSet.t * CharacterSet.t
        let equal (ns, css, cs) (ns', css', cs') = (
            NextSet.equal ns ns' &&
            ControlStateSet.equal css css' &&
            CharacterSet.equal cs cs'
        )
        let to_string (ns, css, cs) = 
            (NextSet.to_string ns) ^ "\n" ^
            (ControlStateSet.to_string css) ^ "\n" ^
            (CharacterSet.to_string cs) ^ "\n"
    end;;

module PdsTest = Test.Tester(ResultType);;

(* Tests defined below *)
let test_pds () = 
    print_string "Testing PDS:\n\n";
    PdsTest.run_tests ();
    print_string "Done.\n\n";;


(****************************************)


let get_profile ma p a = (ma#get_next p a, ma#get_control_states, ma#get_alphabet);;

(* Test 1 -- Empty PDS *)

let test1 () = 
    let ma = new pds in 
    get_profile ma p a;;
let expected1 = (NextSet.empty, ControlStateSet.empty, (CharacterSet.singleton sbot));;

PdsTest.add_test "Test 1 -- Empty PDS" test1 expected1;;



(* Test 2 -- PDS with two p a rules / p a *)

let test2 () = 
    let ma = new pds in 
    ma#add_rule p_a_p'_ab;
    ma#add_rule p_a_p_bab;
    get_profile ma p a;;
let expected2 = (NextSet.from_list [(p, bab); (p',ab)],
                 ControlStateSet.from_list [p; p'],
                 CharacterSet.from_list [a; b; sbot]);;

PdsTest.add_test "Test 2 -- PDS with two p a rules / p a" test2 expected2;;


(* Test 3 -- PDS with many rules *)

let test3 () = 
    let ma = new pds in 
    ma#add_rule p_a_p'_ab;
    ma#add_rule p_a_p_bab;
    ma#add_rule p'_a_p_bab;
    ma#add_rule p'_b_p'_ab;
    ma#add_rule p_b_p_ab;
    get_profile ma p' a;;
let expected3 = (NextSet.from_list [(p,bab)],
                 ControlStateSet.from_list [p; p'],
                 CharacterSet.from_list [a; b; sbot]);;

PdsTest.add_test "Test 3 -- PDS with many rules" test3 expected3;;


(* Test 4 -- PDS with two p a rules / p' a *)

let test4 () = 
    let ma = new pds in 
    ma#add_rule p_a_p'_ab;
    ma#add_rule p_a_p_bab;
    get_profile ma p' a;;
let expected4 = (NextSet.empty,
                 ControlStateSet.from_list [p; p'],
                 CharacterSet.from_list [a; b; sbot]);;

PdsTest.add_test "Test 4 -- PDS with two p a rules / p' a" test4 expected4;;


