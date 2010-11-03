
open Constants;;
open Pds;;
open Ma;;
open Mc_parity;;
open Mc_parity.ParityMa;;
open Mc_parity.ParityMa.MaState;;
open Words;;


let qp1 = QVal(p, 1);;
let qp2 = QVal(p, 2);;
let qp3 = QVal(p, 3);;
let qp'1 = QVal(p', 1);;
let qp'2 = QVal(p', 2);;
let qp'3 = QVal(p', 3);;
let qstar = QStar;;
let qe = QEpsilon;;
let qf1 = QVal(f, 1);;
let qpE = QVal(pE, 0);;
let qp'E = QVal(p'E, 0);;
let qpA = QVal(pA, 0);;

let q1 = QVal(p, 1);;
let q2 = QVal(p, 2);;
let q3 = QVal(p', 1);;
let q4 = QVal(p', 2);;


let tran1a = make_ht_list q1 a [[q1; q2];[q3]];;
let tran2a = make_ht_list q1 a2 [[q2; q1];[q1;q2;q3]];;
let tran3a = make_ht_list q2 a [[q2; q1];[q1;q2;q3]];;
let tran1b = make_ht_list q1 b [[q1; q2];[q3]];;
let tran2b = make_ht_list q2 b [[q4; q1];[q1;q2;q3]];;
let tran3b = make_ht_list q2 b [];;

let tran_q1_a_q1q2 = make_ht_list q1 a [[q1; q2]];;
let tran_q2_b_q2q4 = make_ht_list q2 b [[q2; q4]];;
let tran_q1_b_q3q4 = make_ht_list q1 b [[q3; q4]];;
let tran_q1_a_q2q3 = make_ht_list q1 a [[q2; q3]];;
let tran_q1_a_q1_q2 = make_ht_list q1 a [[q1]; [q2]];;
let tran_q1_b_q1_q2 = make_ht_list q1 b [[q1]; [q2]];;
let tran_q2_b_q3_q4 = make_ht_list q2 b [[q3]; [q4]];;
let tran_q2_a_q1q2 = make_ht_list q2 a2 [[q1;q2]];;
let tran_q2_b_q3q4 = make_ht_list q2 b [[q3; q4]];;
let tran_q4_b_q4 = make_ht_list q4 b [[q4]];;
let tran_q2_b_q3 = make_ht_list q2 b [[q3]];;
let tran_q3_b_q4 = make_ht_list q3 b [[q4]];;
let tran_q4_b_q3 = make_ht_list q4 b [[q3]];;


let tran_qp1_a_qp1 = make_ht_list qp1 a [[qp1]];;
let tran_qp1_b_qstar = make_ht_list qp1 b [[qstar]];;
let tran_qp2_b_qp2qp'2 = make_ht_list qp2 b [[qp2;qp'2]];;
let tran_qp2_b_qp'1_qp'2 = make_ht_list qp2 b [[qp'1]; [qp'2]];;
let tran_qp2_a_qstar = make_ht_list qp2 a [[qstar]];;
let tran_qp2_a_qp1 = make_ht_list qp2 a [[qp1]];;
let tran_qp1_a_qp1qstar = make_ht_list qp1 a [[qp1;qstar]];;
let tran_qp2_a_qp1qstar = make_ht_list qp2 a [[qp1;qstar]];;
let tran_qp3_a_qp1qstar = make_ht_list qp3 a [[qp1;qstar]];;
let tran_qp'1_a_qstar = make_ht_list qp'1 a [[qstar]];;
let tran_qp1_a_qp1_qstar = make_ht_list qp1 a [[qp1];[qstar]];;
let tran_qp2_a_qp1_qstar = make_ht_list qp2 a [[qp1];[qstar]];;
let tran_qp2_a_qp1qstar_qstar = make_ht_list qp2 a [[qp1;qstar]; [qstar]];;
let tran_qp'2_a_qstar = make_ht_list qp'2 a [[qstar]];;
let tran_qp3_a_qstar = make_ht_list qp3 a [[qstar]];;
let tran_qp'2_a_qp1qstar = make_ht_list qp'2 a [[qp1;qstar]];;
let tran_qp3_a_qp1qstar = make_ht_list qp3 a [[qp1;qstar]];;
let tran_qp'3_a_qp1qstar = make_ht_list qp'3 a [[qp1;qstar]];;
let tran_qp'1_a_qstar = make_ht_list qp'1 a [[qstar]];;
let tran_qp'1_a_qp1qstar = make_ht_list qp'1 a [[qp1;qstar]];;
let tran_qp'3_a_qp3qp1qstar = make_ht_list qp'3 a [[qp3;qp1;qstar]];;
let tran_qp'2_a_qp2qp1qstar = make_ht_list qp'2 a [[qp2;qp1;qstar]];;
let tran_qp1_a_qstar = make_ht_list qp1 a [[qstar]];;

let tran_qstar_sbot_qe = make_ht_list qstar sbot [[qe]];;
let tran_qstar_a_qstar = make_ht_list qstar a [[qstar]];;
let tran_qstar_b_qstar = make_ht_list qstar b [[qstar]];;
let tran_qf1_sbot_qe = make_ht_list qf1 sbot [[qe]];;
let tran_qf1_a_qstarqp1 = make_ht_list qf1 a [[qstar; qp1]];;
let tran_qp1_sbot_qe = make_ht_list qp1 sbot [[qe]];;
let tran_qp1_a_qp1 = make_ht_list qp1 a [[qp1]];;

let tran_p'E_sbot_qe = make_ht_list qp'E sbot [[qe]];;
let tran_p'E_a_qstar = make_ht_list qp'E a [[qstar]];;
let tran_p'E_b_qstar = make_ht_list qp'E b [[qstar]];;
let tran_pE_a_qstar = make_ht_list qpE a [[qstar]];;

let ss_empty = SetOfStateSets.empty;;
let ss_singleton_empty = SetOfStateSets.singleton StateSet.empty;;
let ss_singleton_qstar = setofstatesets_from_list_list [[QStar]];;
let ss_singleton_qepsilon = setofstatesets_from_list_list [[QEpsilon]];;
let ss_two_sets = setofstatesets_from_list_list [[q1]; [q2]];;
let ss_two_sets2 = setofstatesets_from_list_list [[q3]; [q4]];;
let ss_pair_set = setofstatesets_from_list_list [[q1; q2]];;
let ss_pair_set2 = setofstatesets_from_list_list [[q3; q4]];;






