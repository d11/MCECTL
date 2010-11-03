
module type ResultType =
    sig
        type t
        val equal : t -> t -> bool
        val to_string : t -> string
    end

module Tester = 
    functor (RT : ResultType) ->
        struct
            type res_type = RT.t;;

            (* test method * expected result * function to check equivalence *)
            let tests = ref [];;

            let add_test name test expected = 
                tests := (name, test, expected)::!tests;;

            let run_test name test_method expected =  
                let result = test_method () in
                print_string "Test: ";
                print_string name;
                print_string "\nResult: ";
                if RT.equal result expected then
                    print_string "ok.\n"
                else (
                    print_string "FAILED.\n\n";
                    print_string "Expected result:\n\n";
                    print_string (RT.to_string expected);
                    print_string "\n\nActual result:\n\n";
                    print_string (RT.to_string result);
                    print_string "\n\n";
                    print_newline ()
                );;

            let run_tests () =
                let do_test (name, test, expected) =
                    run_test name test expected in
                let _ = List.map do_test !tests in ();;
        end;;



