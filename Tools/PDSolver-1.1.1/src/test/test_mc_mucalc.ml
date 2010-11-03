
open Mucalc;;
open Ma;;
open Pds;;
open Pds_mucalc;;
open Words;;
open State;;
open Constants;;
open Mc_pds_mucalc;;
open Mc_pds_mucalc.PdsMuMa;;
open Mc_pds_mucalc.PdsMuMa.MaState;;


let x = Prop("x");;
let y = Prop("y");;

let px = QVal(p, x, 0);;
let p'x = QVal(p', x, 0);;
let py = QVal(p, y, 0);;
let p'y = QVal(p', y, 0);;

let pxandy1 = QVal(p, Bin(x, And, y), 1);;
let p'xandy1 = QVal(p', Bin(x, And, y), 1);;
let pxory1 = QVal(p, Bin(x, Or, y), 1);;
let p'xory1 = QVal(p', Bin(x, Or, y), 1);;
let pboxx1 = QVal(p, Un(Box,x), 1);;
let p'boxx1 = QVal(p', Un(Box,x), 1);;
let pdiax1 = QVal(p, Un(Diamond,x), 1);;
let p'diax1 = QVal(p', Un(Diamond,x), 1);;



let tran_px_a_pxp'x_py = make_ht_list px a [[px; p'x];[py]];;
let tran_p'x_a_p'xpx_pxp'xpy = make_ht_list p'x a [[p'x; px];[px;p'x;py]];;
let tran_px_b_pxp'x_py = make_ht_list px b [[px; p'x];[py]];;
let tran_p'x_b_p'ypx_pxp'xpy = make_ht_list p'x b [[p'y; px];[px;p'x;py]];;
let tran_p'x_b_ = make_ht_list p'x b [];;

let tran_py_a_p'x_py = make_ht_list py a [[p'x];[py]];;
let tran_p'y_a_p'xpxpy_p'xpy = make_ht_list p'y a [[p'x; px; py];[p'x;py]];;
let tran_py_b_py = make_ht_list py b [[py]];;
let tran_p'y_b_p'ypx = make_ht_list p'y b [[p'y; px]];;
let tran_p'y_b_ = make_ht_list p'y b [];;

let empty_prop_map = new proposition_map [];;


let default_pds = let pds = new pds in
                  pds#add_rule p_b_p_ab;
                  pds#add_rule p'_a_p_bab;
                  pds;;

let pp0 = QVal(p, Prop("p"), 0);;
let pf0 = QVal(p, Prop("f"), 0);;
let ff0 = QVal(f, Prop("f"), 0);;
let fp0 = QVal(f, Prop("p"), 0);;
let pnotp0 = QVal(p, NegProp("p"), 0);;
let fnotp0 = QVal(f, NegProp("p"), 0);;
let pnotf0 = QVal(p, NegProp("f"), 0);;
let fnotf0 = QVal(f, NegProp("f"), 0);;
let pZ1 = QVal(p, Var("Z"), 1);;
let fZ1 = QVal(f, Var("Z"), 1);;
let pY2 = QVal(p, Var("Y"), 2);;
let fY2 = QVal(f, Var("Y"), 2);;
let ppBZ2 = QVal(p, Bin(Prop("p"), And, Un(Box, Var("Z"))), 2);;
let fpBZ2 = QVal(p, Bin(Prop("p"), And, Un(Box, Var("Z"))), 2);;
let pfBY2 = QVal(p, Bin(Prop("f"), And, Un(Box, Var("Y"))), 2);;
let ffBY2 = QVal(f, Bin(Prop("f"), And, Un(Box, Var("Y"))), 2);;
let pBZ2 = QVal(p, Un(Box, Var("Z")), 2);;
let fBZ2 = QVal(p, Un(Box, Var("Z")), 2);;
let pBY2 = QVal(f, Un(Box, Var("Y")), 2);;
let fBY2 = QVal(f, Un(Box, Var("Y")), 2);;
let pall3 = QVal(p, Bin(Bin(Prop("f"), And, Un(Box, Var("Y"))), 
                        And, 
                        Bin(Prop("p"), And, Un(Box, Var("Z")))), 3);;
let fall3 = QVal(f, Bin(Bin(Prop("f"), And, Un(Box, Var("Y"))), 
                        And, 
                        Bin(Prop("p"), And, Un(Box, Var("Z")))), 3);;
let ppBZ3 = QVal(p, Bin(Prop("p"), And, Un(Box, Var("Z"))), 3);;
let fpBZ3 = QVal(p, Bin(Prop("p"), And, Un(Box, Var("Z"))), 3);;
let pfBY3 = QVal(f, Bin(Prop("f"), And, Un(Box, Var("Y"))), 3);;
let ffBY3 = QVal(f, Bin(Prop("f"), And, Un(Box, Var("Y"))), 3);;
let pBZ3 = QVal(p, Un(Box, Var("Z")), 3);;
let fBZ3 = QVal(p, Un(Box, Var("Z")), 3);;
let pBY3 = QVal(f, Un(Box, Var("Y")), 3);;
let fBY3 = QVal(f, Un(Box, Var("Y")), 3);;



let pZ11 = QVal(p, Var("Z1_0"), 1);;
let fZ11 = QVal(f, Var("Z1_0"), 1);;
let pBZ11 = QVal(p, Un(Box, Var("Z1_0")), 1);;
let fBZ11 = QVal(f, Un(Box, Var("Z1_0")), 1);;
let pBZ21 = QVal(p, Un(Box, Var("Z2_1")), 1);;
let fBZ21 = QVal(f, Un(Box, Var("Z2_1")), 1);;
let ppBZ11 = QVal(p, Bin(Prop("p"),And,Un(Box, Var("Z1_0"))), 1);;
let fpBZ11 = QVal(f, Bin(Prop("p"),And,Un(Box, Var("Z1_0"))), 1);;
let pfBZ21 = QVal(p, Bin(Prop("f"),And,Un(Box, Var("Z2_1"))), 1);;
let ffBZ21 = QVal(f, Bin(Prop("f"),And,Un(Box, Var("Z2_1"))), 1);;

let tran_pp0_sbot_qe = make_ht_list pp0 sbot [[QEpsilon]];;
let tran_pp0_a_qstar= make_ht_list pp0 a [[QStar]];;
let tran_ff0_sbot_qe = make_ht_list ff0 sbot [[QEpsilon]];;
let tran_ff0_a_qstar= make_ht_list ff0 a [[QStar]];;
let tran_fnotp0_sbot_qe = make_ht_list fnotp0 sbot [[QEpsilon]];;
let tran_fnotp0_a_qstar = make_ht_list fnotp0 a [[QStar]];;
let tran_pnotf0_sbot_qe = make_ht_list pnotf0 sbot [[QEpsilon]];;
let tran_pnotf0_a_qstar = make_ht_list pnotf0 a [[QStar]];;
let tran_pZ11_sbot_qe = make_ht_list pZ11 sbot [[QEpsilon]];;
let tran_pZ11_a_pZ11qstar = make_ht_list pZ11 a [[pZ11;QStar]];;
let tran_fZ11_sbot_qe = make_ht_list fZ11 sbot [[QEpsilon]];;
let tran_fZ11_a_pZ11qstar = make_ht_list fZ11 a [[pZ11;QStar]];;
let tran_pBZ11_a_pZ11 = make_ht_list pBZ11 a [[pZ11]];;
let tran_pBZ11_sbot_qe = make_ht_list pBZ11 sbot [[QEpsilon]];;
let tran_fBZ11_sbot_qe = make_ht_list fBZ11 sbot [[QEpsilon]];;
let tran_fBZ11_a_pZ11qstar = make_ht_list fBZ11 a [[pZ11;QStar]];;
let tran_pBZ21_a_pZ11 = make_ht_list pBZ21 a [[pZ11]];;
let tran_pBZ21_sbot_qe = make_ht_list pBZ21 sbot [[QEpsilon]];;
let tran_fBZ21_a_qstar = make_ht_list fBZ21 a [[QStar]];;
let tran_fBZ21_sbot_qe = make_ht_list fBZ21 sbot [[QEpsilon]];;
let tran_ppBZ11_a_pZ11qstar = make_ht_list ppBZ11 a [[pZ11;QStar]];;
let tran_ppBZ11_sbot_qe = make_ht_list ppBZ11 sbot [[QEpsilon]];;
let tran_ffBZ21_a_qstar = make_ht_list ffBZ21 a [[QStar]];;
let tran_ffBZ21_sbot_qe = make_ht_list ffBZ21 sbot [[QEpsilon]];;

let tran_qstar_a_qstar = make_ht_list QStar a [[QStar]];;
let tran_qstar_sbot_qe = make_ht_list QStar sbot [[QEpsilon]];;


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


let test_mc_mucalc () = 
    print_string "Testing MC MuCalc:\n\n";
    MCTest.run_tests ();
    BoolMCTest.run_tests ();
    print_string "Done.\n\n";;



(********************************************************)


(* Test 1 -- And, Props, Empty *)

let test1 () = 
    let pds = default_pds in
    let xandy = Bin(x, And, y) in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] xandy empty_prop_map) in
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch xandy 1 ma init_ma_info in
    ma;;
let expected1 = 
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;pxandy1;p'xandy1;px;p'x;py;p'y]);
    ma;;

MCTest.add_test "Test 1 -- And, Props, Empty" test1 expected1;;



(* Test 2 -- And, Props, with trans on one side *)

let test2 () = 
    let pds = default_pds in
    let xandy = Bin(x, And, y) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] xandy empty_prop_map) in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch xandy 1 ma init_ma_info in
    ma;;
let expected2 = 
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;pxandy1;p'xandy1;px;p'x;py;p'y]);
    ma;;

MCTest.add_test "Test 2 -- And, Props, with trans on one side" test2 expected2;;



(* Test 3 -- And, Props, with trans on both sides *)

let test3 () = 
    let pds = default_pds in
    let xandy = Bin(x, And, y) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] xandy empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_a_p'x_py;
    ma#add_hyper_transition tran_p'y_a_p'xpxpy_p'xpy;
    ma#add_hyper_transition tran_py_b_py;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch xandy 1 ma init_ma_info in
    ma;;
let expected3 = 
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_a_p'x_py;
    ma#add_hyper_transition tran_p'y_a_p'xpxpy_p'xpy;
    ma#add_hyper_transition tran_py_b_py;
    let tran_pxy1 = make_ht_list pxandy1 a [[px; py; p'x]; 
                                            [px; p'x];
                                            [py];
                                            [py;p'x]] in
    let tran_p'xy1 = make_ht_list p'xandy1 a [[px; py; p'x]] in
    ma#add_hyper_transition tran_pxy1;
    ma#add_hyper_transition tran_p'xy1;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;pxandy1;p'xandy1;px;p'x;py;p'y]);
    ma;;

MCTest.add_test "Test 3 -- And, Props, with trans on both sides" test3 expected3;;












(* Test 4 -- Or, Props, Empty *)

let test4 () = 
    let pds = default_pds in
    let xory = Bin(x, Or, y) in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] xory empty_prop_map) in
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch xory 1 ma init_ma_info in
    ma;;
let expected4 = 
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pxory1;p'xory1]);
    ma;;

MCTest.add_test "Test 4 -- Or, Props, Empty" test4 expected4;;



(* Test 5 -- Or, Props, with trans on one side *)

let test5 () = 
    let pds = default_pds in
    let xory = Bin(x, Or, y) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] xory empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch xory 1 ma init_ma_info in
    ma;;
let expected5 = 
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    let pxy1 = QVal(p, Bin(x, Or, y), 1) in
    let p'xy1 = QVal(p', Bin(x, Or, y), 1) in
    let tran_pxy1 = make_ht_list pxy1 a [[px;p'x];[py]] in
    let tran_p'xy1 = make_ht_list p'xy1 a [[p'x; px]; [px; p'x; py]] in
    ma#add_hyper_transition tran_pxy1;
    ma#add_hyper_transition tran_p'xy1;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pxory1;p'xory1]);
    ma;;

MCTest.add_test "Test 5 -- Or, Props, with trans on one side" test5 expected5;;



(* Test 6 -- Or, Props, with trans on both sides *)

let test6 () = 
    let pds = default_pds in
    let xory = Bin(x, Or, y) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] xory empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_a_p'x_py;
    ma#add_hyper_transition tran_p'y_a_p'xpxpy_p'xpy;
    ma#add_hyper_transition tran_py_b_py;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch xory 1 ma init_ma_info in
    ma;;
let expected6 = 
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_a_p'x_py;
    ma#add_hyper_transition tran_p'y_a_p'xpxpy_p'xpy;
    ma#add_hyper_transition tran_py_b_py;
    let pxy1 = QVal(p, Bin(x, Or, y), 1) in
    let p'xy1 = QVal(p', Bin(x, Or, y), 1) in
    let tran_pxy1_a = make_ht_list pxy1 a [[px; p'x]; [py]; [p'x]] in
    let tran_pxy1_b = make_ht_list pxy1 b [[py]] in
    let tran_p'xy1 = make_ht_list p'xy1 a [[px;p'x]; [px;p'x;py];[p'x;py]] in
    ma#add_hyper_transition tran_pxy1_a;
    ma#add_hyper_transition tran_pxy1_b;
    ma#add_hyper_transition tran_p'xy1;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pxory1;p'xory1]);
    ma;;

MCTest.add_test "Test 6 -- Or, Props, with trans on both sides" test6 expected6;;







(* Test 7 -- Box, Props, Empty *)

let test7 () = 
    let pds = default_pds in
    let boxx = Un(Box, x) in
    let ma = new ma in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] boxx empty_prop_map) in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    let _ = mc#dispatch boxx 1 ma init_ma_info in
    ma;;
let expected7 = 
    let ma = new ma in
    let tran_pboxx_sbot = make_ht_list pboxx1 sbot [[QEpsilon]] in
    let tran_p'boxx_sbot = make_ht_list p'boxx1 sbot [[QEpsilon]] in
    let tran_pboxx_a = make_ht_list pboxx1 a [[QStar]] in
    let tran_p'boxx_b = make_ht_list p'boxx1 b [[QStar]] in
    ma#add_hyper_transition tran_pboxx_sbot;
    ma#add_hyper_transition tran_pboxx_a;
    ma#add_hyper_transition tran_p'boxx_sbot;
    ma#add_hyper_transition tran_p'boxx_b;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pboxx1;p'boxx1]);
    ma;;

MCTest.add_test "Test 7 -- Box, Props, Empty" test7 expected7;;



(* Test 8 -- Box, Props, with default rules and some trans *)

let test8 () = 
    let pds = default_pds in
    let boxx = Un(Box, x) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] boxx empty_prop_map) in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_b_py;
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch boxx 1 ma init_ma_info in
    ma;;
let expected8 = 
    let ma = new ma in
    let tran_pboxx_sbot = make_ht_list pboxx1 sbot [[QEpsilon]] in
    let tran_p'boxx_sbot = make_ht_list p'boxx1 sbot [[QEpsilon]] in
    let tran_pboxx_a = make_ht_list pboxx1 a [[QStar]] in
    let tran_pboxx_b = make_ht_list pboxx1 b [[py]] in
    let tran_p'boxx_b = make_ht_list p'boxx1 b [[QStar]] in
    ma#add_hyper_transition tran_py_b_py;
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_pboxx_sbot;
    ma#add_hyper_transition tran_pboxx_a;
    ma#add_hyper_transition tran_pboxx_b;
    ma#add_hyper_transition tran_p'boxx_sbot;
    ma#add_hyper_transition tran_p'boxx_b;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pboxx1;p'boxx1]);
    ma;;


MCTest.add_test "Test 8 -- Box, Props, with default rules and some trans" test8 expected8;;




(* Test 9 -- Box, Props, two rules *)

let test9 () = 
    let pds = new pds in
    pds#add_rule p_b_p_ab;
    pds#add_rule p_b_p_a;
    pds#add_rule p'_a_p_bab;
    let boxx = Un(Box, x) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] boxx empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_b_py;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch boxx 1 ma init_ma_info in
    ma;;
let expected9 = 
    let ma = new ma in
    let tran_pboxx_sbot = make_ht_list pboxx1 sbot [[QEpsilon]] in
    let tran_p'boxx_sbot = make_ht_list p'boxx1 sbot [[QEpsilon]] in
    let tran_pboxx_a = make_ht_list pboxx1 a [[QStar]] in
    let tran_pboxx_b = make_ht_list pboxx1 b [[px; p'x; py]; [py]] in
    let tran_p'boxx_b = make_ht_list p'boxx1 b [[QStar]] in
    ma#add_hyper_transition tran_py_b_py;
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_pboxx_sbot;
    ma#add_hyper_transition tran_pboxx_a;
    ma#add_hyper_transition tran_pboxx_b;
    ma#add_hyper_transition tran_p'boxx_sbot;
    ma#add_hyper_transition tran_p'boxx_b;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pboxx1;p'boxx1]);
    ma;;


MCTest.add_test "Test 9 -- Box, Props, two rules" test9 expected9;;





(* Test 10 -- Diamond, Props, Empty *)

let test10 () = 
    let pds = default_pds in
    let diax = Un(Diamond, x) in
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] diax empty_prop_map) in
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch diax 1 ma init_ma_info in
    ma;;
let expected10 = 
    let ma = new ma in
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pdiax1;p'diax1]);
    ma;;

MCTest.add_test "Test 10 -- Diamond, Props, Empty" test10 expected10;;



(* Test 11 -- Diamond, Props, with default rules and some trans *)

let test11 () = 
    let pds = default_pds in
    let diax = Un(Diamond, x) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] diax empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_b_py;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch diax 1 ma init_ma_info in
    ma;;
let expected11 = 
    let ma = new ma in
    let tran_pdiax_b = make_ht_list pdiax1 b [[py]] in
    ma#add_hyper_transition tran_py_b_py;
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_pdiax_b;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pdiax1;p'diax1]);
    ma;;


MCTest.add_test "Test 11 -- Box, Props, with default rules and some trans" test11 expected11;;




(* Test 12 -- Diamond, Props, two rules *)

let test12 () = 
    let pds = new pds in
    pds#add_rule p_b_p_ab;
    pds#add_rule p_b_p_a;
    pds#add_rule p'_a_p_bab;
    let diax = Un(Diamond, x) in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] diax empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_py_b_py;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y]);
    mc#set_disable_hard_opt true;
    let _ = mc#dispatch diax 1 ma init_ma_info in
    ma;;
let expected12 = 
    let ma = new ma in
    let tran_pdiax_b = make_ht_list pdiax1 b [[py]; [px;p'x]] in
    ma#add_hyper_transition tran_py_b_py;
    ma#add_hyper_transition tran_px_a_pxp'x_py;
    ma#add_hyper_transition tran_p'x_a_p'xpx_pxp'xpy;
    ma#add_hyper_transition tran_pdiax_b;
    ma#add_states (StateSet.from_list [QStar;QEpsilon;px;p'x;py;p'y;pdiax1;p'diax1]);
    ma;;




MCTest.add_test "Test 12 -- Diamond, Props, two rules" test12 expected12;;





(* Test 13 -- Project and delete *)



let test13 () = 
    let pds = new pds in
    let mc = new mc_pds_mucalc (new pds_mucalc pds [] (Var("X")) empty_prop_map) in
    let ma = new ma in
    ma#add_hyper_transition (make_ht_list pp0 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ff0 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ppBZ2 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list ffBY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list pBZ2 a [[pZ1]]);
    ma#add_hyper_transition (make_ht_list pBY2 a [[pY2]]);
    ma#add_hyper_transition (make_ht_list pY2 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list fY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ppBZ2 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list ffBY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list pBZ2 a [[pZ1]]);
    ma#add_hyper_transition (make_ht_list fBY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ppBZ3 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list ffBY3 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list pBZ3 a [[pZ1]]);
    ma#add_hyper_transition (make_ht_list pBY3 a [[pY2]]);
    ma#add_hyper_transition (make_ht_list pall3 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list fall3 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ppBZ3 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list ffBY3 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list pBZ3 a [[pZ1]]);
    ma#add_hyper_transition (make_ht_list fBY3 a [[QStar]]);
    ma#add_states (StateSet.from_list [ 
        pp0; ff0; pZ1; fZ1; pY2; fY2; ppBZ2; fpBZ2; pfBY2; 
        ffBY2; pBZ2; fBZ2; pBY2; fBY2; pall3; fall3; ppBZ3; 
        fpBZ3; pfBY3; ffBY3; pBZ3; fBZ3; pBY3; fBY3 
    ]);
    let rename_inits = StateSet.from_list [pall3;fall3] in
    let new_inits = StateSet.from_list [pY2;fY2] in
    mc#set_disable_hard_opt true;
    let _ = mc#project_rename_and_delete rename_inits new_inits 2 ma in
    ma;;
let expected13 = 
    let ma = new ma in
    ma#add_hyper_transition (make_ht_list pp0 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ff0 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ppBZ2 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list ffBY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list pBZ2 a [[pZ1]]);
    ma#add_hyper_transition (make_ht_list pBY2 a [[pY2]]);
    ma#add_hyper_transition (make_ht_list pY2 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list fY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list ppBZ2 a [[pZ1;QStar]]);
    ma#add_hyper_transition (make_ht_list ffBY2 a [[QStar]]);
    ma#add_hyper_transition (make_ht_list pBZ2 a [[pZ1]]);
    ma#add_hyper_transition (make_ht_list fBY2 a [[QStar]]);
    ma#add_states (StateSet.from_list [ 
        pp0; ff0; pZ1; fZ1; pY2; fY2; ppBZ2; fpBZ2; pfBY2; 
        ffBY2; pBZ2; fBZ2; pBY2; fBY2;
    ]);
    ma;;





MCTest.add_test "Test 13 -- Project and delete" test13 expected13;;





let test14 () = 
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
    mc#construct_denotation;;

let expected14 = 
    let ma = new ma in
    ma#add_hyper_transition tran_qstar_sbot_qe;
    ma#add_hyper_transition tran_qstar_a_qstar;
    ma#add_hyper_transition tran_pZ11_sbot_qe;
    ma#add_hyper_transition tran_pZ11_a_pZ11qstar;
    ma#add_hyper_transition tran_fZ11_sbot_qe;
    ma#add_hyper_transition tran_fZ11_a_pZ11qstar;
    ma#add_states (StateSet.from_list [
        QStar;QEpsilon; pZ11;fZ11
    ]);
    ma;;


MCTest.add_test "Test 14 -- Journal version example" test14 expected14;;

