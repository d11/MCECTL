
open Pds_mucalc;;
open Parity_game;;
open Mucalc;;
open Pds;;

val pdmu_to_pdg : pds_mucalc -> parity_game
val pdg_to_pdmu : parity_game -> pds_mucalc

val mu_to_pdg_control : control_state -> mu_formula -> control_state 


