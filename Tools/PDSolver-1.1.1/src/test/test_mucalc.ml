
open Mucalc;;

(* Complete automaton tester *)
module MuCalc_ResultType = 
    struct
        type t = mu_formula
        let equal x y = (x = y)
        let to_string x = mu_formula_to_string x
    end;;

module MuCalcTest = Test.Tester(MuCalc_ResultType);;


(* Tests defined below *)
let test_mucalc () = 
    print_string "Testing Mucalc:\n\n";
    MuCalcTest.run_tests ();
    print_string "Done.\n\n";;


(*****************************************************************************************)

let test1 () = 
    let f = Bin(Prop("x"),And,Bin(Var("Z"),Or,Prop("x"))) in
    normalise_formula f;;

let expected1 = Bin(Prop("x"),And,Bin(Var("Z"),Or,Prop("x")));;

MuCalcTest.add_test "Test 1: Normalise, no change." test1 expected1;;


let test2 () = 
    let f = Bin(Mu("Z",Bin(Var("Z"),And,Var("Y"))),
                And,
                Nu("Y",Bin(Var("Z"),And,Var("Y")))) in
    normalise_formula f;;

let expected2 = Bin(Mu("Z_0",Bin(Var("Z_0"),And,Var("Y"))),
                    And,
                    Nu("Y_1",Bin(Var("Z"),And,Var("Y_1"))));;

MuCalcTest.add_test "Test 2: Normalise, two bound vars, not nested." test2 expected2;;



let test3 () = 
    let f = Mu("Z",Bin(Var("Z"),And,Nu("Z",Bin(Var("Z"),And,Var("Z"))))) in
    normalise_formula f;;

let expected3 =  Mu("Z_0",Bin(Var("Z_0"),And,Nu("Z_1",Bin(Var("Z_1"),And,Var("Z_1")))));;

MuCalcTest.add_test "Test 3: Normalise, two bound vars, nested." test3 expected3;;




let test4 () = 
    let f = Un(Not,Bin(Prop("x"),And,Bin(Prop("z"),Or,Prop("x")))) in
    normalise_formula f;;

let expected4 = Bin(NegProp("x"),Or,Bin(NegProp("z"),And,NegProp("x")));;

MuCalcTest.add_test "Test 4: Normalise, negation, no vars." test4 expected4;;




let test5 () = 
    let f = Un(Not,Bin(Prop("x"),And,Un(Not,Bin(Prop("z"),Or,Prop("x"))))) in
    normalise_formula f;;

let expected5 = Bin(NegProp("x"),Or,Bin(Prop("z"),Or,Prop("x")));;

MuCalcTest.add_test "Test 5: Normalise, double negation, no vars." test5 expected5;;



let test6 () = 
    let f = Un(Not,Mu("Z",Bin(Var("Z"),And,Nu("Z",Bin(Var("Z"),And,Var("Z")))))) in
    normalise_formula f;;

let expected6 =  Nu("Z_0",Bin(Var("Z_0"),Or,Mu("Z_1",Bin(Var("Z_1"),Or,Var("Z_1")))));;

MuCalcTest.add_test "Test 6: Normalise, negate nested bound vars." test6 expected6;;



let test7 () = 
    let f = Bin(Prop("x"),Implies,Prop("y")) in
    normalise_formula f;;

let expected7 = Bin(NegProp("x"),Or,Prop("y"));;

MuCalcTest.add_test "Test 7: Normalise, implication." test7 expected7;;


let test8 () = 
    let f = Un(Not,Bin(Prop("x"),Implies,Prop("y"))) in
    normalise_formula f;;

let expected8 = Bin(Prop("x"),And,NegProp("y"));;

MuCalcTest.add_test "Test 8: Normalise, negated implication." test8 expected8;;



(************************************************************************************
 * Test minimisation 
 *)

let test9 () = 
    normalise_formula (True);;
let expected9 = True;;

MuCalcTest.add_test "Test 9: minimize True." test9 expected9;;


let test10 () = 
    normalise_formula (False);;
let expected10 = False;;

MuCalcTest.add_test "Test 10: minimize False." test10 expected10;;


let test11 () = 
    normalise_formula (Prop("x"));;
let expected11 = Prop("x");;

MuCalcTest.add_test "Test 11: minimize Prop(x)." test11 expected11;;


let test12 () = 
    normalise_formula (NegProp("x"));;
let expected12 = NegProp("x");;

MuCalcTest.add_test "Test 12: minimize NegProp(x)." test12 expected12;;


let test13 () = 
    normalise_formula (Var("X"));;
let expected13 = Var("X");;

MuCalcTest.add_test "Test 13: minimize Var(X)." test13 expected13;;


let test14 () = 
    normalise_formula (Bin(True,And,Var("X")));;
let expected14 = Var("X");;

MuCalcTest.add_test "Test 14: minimize True and Var(X)." test14 expected14;;


let test15 () = 
    normalise_formula (Bin(Var("X"),And,True));;
let expected15 = Var("X");;

MuCalcTest.add_test "Test 15: minimize Var(X) and True." test15 expected15;;


let test16 () = 
    normalise_formula (Bin(Var("X"),And,False));;
let expected16 = False;;

MuCalcTest.add_test "Test 16: minimize Var(X) and False." test16 expected16;;


let test17 () = 
    normalise_formula (Bin(False,And,Var("X")));;
let expected17 = False;;

MuCalcTest.add_test "Test 17: minimize False and Var(X)." test17 expected17;;


let test18 () = 
    normalise_formula (Bin(True,Or,Var("X")));;
let expected18 = True;;

MuCalcTest.add_test "Test 18: minimize True or Var(X)." test18 expected18;;


let test19 () = 
    normalise_formula (Bin(Var("X"),Or,True));;
let expected19 = True;;

MuCalcTest.add_test "Test 19: minimize Var(X) or True." test19 expected19;;


let test20 () = 
    normalise_formula (Bin(False,Or,Var("X")));;
let expected20 = Var("X");;

MuCalcTest.add_test "Test 20: minimize False or Var(X)." test20 expected20;;


let test21 () = 
    normalise_formula (Bin(Var("X"),Or,False));;
let expected21 = Var("X");;

MuCalcTest.add_test "Test 21: minimize Var(X) or False." test21 expected21;;


let test22 () = 
    normalise_formula (Bin(True,Implies,Var("X")));;
let expected22 = Var("X");;

MuCalcTest.add_test "Test 22: minimize True -> Var(X)." test22 expected22;;


let test23 () = 
    normalise_formula (Bin(Var("X"),Implies,True));;
let expected23 = True;;

MuCalcTest.add_test "Test 23: minimize Var(X) -> True." test23 expected23;;


let test24 () = 
    normalise_formula (Bin(False,Implies,Var("X")));;
let expected24 = True;;

MuCalcTest.add_test "Test 24: minimize False -> Var(X)." test24 expected24;;


let test25 () = 
    normalise_formula (Bin(Prop("x"),And,Prop("y")));;
let expected25 = Bin(Prop("x"),And,Prop("y"));;

MuCalcTest.add_test "Test 25: minimize x and y." test25 expected25;;


let test26 () = 
    normalise_formula (Un(Not,True));;
let expected26 = False;;

MuCalcTest.add_test "Test 26: minimize not true." test26 expected26;;


let test27 () = 
    normalise_formula (Un(Not,False));;
let expected27 = True;;

MuCalcTest.add_test "Test 27: minimize not false." test27 expected27;;


let test28 () = 
    normalise_formula (Un(Not,Prop("x")));;
let expected28 = NegProp("x");;

MuCalcTest.add_test "Test 28: minimize not x." test28 expected28;;


let test29 () = 
    normalise_formula (ConstrainUn(ConstrainBox,[],Prop("x")));;
let expected29 = ConstrainUn(ConstrainBox,[],Prop("x"));;

MuCalcTest.add_test "Test 29: minimize [-]x." test29 expected29;;


let test30 () = 
    normalise_formula (Mu("X",True));;
let expected30 = True;;

MuCalcTest.add_test "Test 30: minimize mu X . true." test30 expected30;;


let test31 () = 
    normalise_formula (Mu("X",False));;
let expected31 = False;;

MuCalcTest.add_test "Test 31: minimize mu X . false." test31 expected31;;


let test32 () = 
    normalise_formula (Mu("X",Prop("x")));;
let expected32 = Prop("x");;

MuCalcTest.add_test "Test 32: minimize mu X . x." test32 expected32;;


let test33 () = 
    normalise_formula (Mu("X",NegProp("x")));;
let expected33 = NegProp("x");;

MuCalcTest.add_test "Test 33: minimize mu X . not x." test33 expected33;;


let test34 () = 
    normalise_formula (Mu("X",Var("X")));;
let expected34 = False;;

MuCalcTest.add_test "Test 34: minimize mu X . X." test34 expected34;;


let test35 () = 
    normalise_formula (Mu("X",Var("Y")));;
let expected35 = Var("Y");;

MuCalcTest.add_test "Test 35: minimize mu X . Y." test35 expected35;;


let test36 () = 
    normalise_formula (Mu("X",Bin(Var("Y"),And,Var("X"))));;
let expected36 = Mu("X_0",Bin(Var("Y"),And,Var("X_0")));;

MuCalcTest.add_test "Test 36: minimize mu X . X and Y." test36 expected36;;




let test37 () = 
    normalise_formula (Nu("X",True));;
let expected37 = True;;

MuCalcTest.add_test "Test 37: minimize nu X . true." test37 expected37;;


let test38 () = 
    normalise_formula (Nu("X",False));;
let expected38 = False;;

MuCalcTest.add_test "Test 38: minimize nu X . false." test38 expected38;;


let test39 () = 
    normalise_formula (Nu("X",Prop("x")));;
let expected39 = Prop("x");;

MuCalcTest.add_test "Test 39: minimize nu X . x." test39 expected39;;


let test40 () = 
    normalise_formula (Nu("X",NegProp("x")));;
let expected40 = NegProp("x");;

MuCalcTest.add_test "Test 40: minimize nu X . not x." test40 expected40;;


let test41 () = 
    normalise_formula (Nu("X",Var("X")));;
let expected41 = True;;

MuCalcTest.add_test "Test 41: minimize nu X . X." test41 expected41;;


let test42 () = 
    normalise_formula (Nu("X",Var("Y")));;
let expected42 = Var("Y");;

MuCalcTest.add_test "Test 42: minimize nu X . Y." test42 expected42;;


let test43 () = 
    normalise_formula (Nu("X",Bin(Var("Y"),And,Var("X"))));;
let expected43 = Nu("X_0",Bin(Var("Y"),And,Var("X_0")));;

MuCalcTest.add_test "Test 43: minimize nu X . X and Y." test43 expected43;;



let test44 () = 
    let f = Un(Not,Mu("Z",Bin(Var("Z"),And,Nu("Z",Var("Z"))))) in
    normalise_formula f;;

let expected44 = True;;

MuCalcTest.add_test "Test 44: Normalise, nested 1." test44 expected44;;



