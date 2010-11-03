
open Words;;
open Pds;;
open State;;

let p = make_control_state "p";;
let p' = make_control_state "p'";;
let f = make_control_state "f";;
let pE = make_control_state "pE";;
let pA = make_control_state "pA";;
let p'E = make_control_state "p'E";;
let eloise_stuck = make_control_state "eloise_stuck";;
let abelard_stuck = make_control_state "abelard_stuck";;

let a = make_character "a";;
let a2 = make_character "a";;
let b = make_character "b";;

let alphabet_ab = CharacterSet.add a (CharacterSet.add b CharacterSet.empty);;

let e = make_word [];;
let ab = make_word [a; b];;
let abb = make_word [a; b; b];;
let ba = make_word [b; a];;
let bab = make_word [b; a; b];;
let babbb = make_word [b; a; b; b; b];;
let word_a = make_word [a];;
let word_b = make_word [b];;
let word_sbot = make_word [sbot];;
let aa = make_word [a; a];;
let a_bot_b = make_word [a; sbot; b];;
let a_a_bot = make_word [a; a; sbot];;

let p_a_p'_ab = make_rule p a p' ab;;
let p_a_p_bab = make_rule p a p bab;;
let p_b_p_ab = make_rule p b p ab;;
let p_b_p_a = make_rule p b p word_a;;
let p'_a_p_bab = make_rule p' a p bab;;
let p'_b_p'_ab = make_rule p' b p' ab;;

let p_sbot_f_sbot = make_rule p sbot f word_sbot;;
let p_a_p = make_rule p a p e;;
let f_a_p_a = make_rule f a p word_a;;
let f_a_f_aa = make_rule f a f aa;;
let f_sbot_f_sbot = make_rule f sbot f word_sbot;;



