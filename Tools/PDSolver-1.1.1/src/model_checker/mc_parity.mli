
open Parity_game;;
open Ma;;
open State;;
open Words;;
open Pds;;
open Mc;;


type state = QStar | QEpsilon | QVal of control_state * int

val state_to_string : state -> string

module BasicState :
    sig
        type t = state
        val compare : t -> t -> int
        val to_string : t -> string
        val to_dot_string : t -> string
        val can_simulate : t -> t -> bool
    end

module ParityMa : Ma.S with type state = state





type pdg_error = BadBotUse of rule list | 
                 NeedsBot | 
                 NotComplete of (control_state * character) list |
                 PropsMissing of control_state list |
                 BadControlStateName of control_state list |
                 BadInterestingConfig of (config * CharacterSet.t);;
type pdg_errors = None | Errors of pdg_error list

exception Bad_input of pdg_error list


class mc_parity_stats : 
    int -> int -> int -> int -> float -> float -> float -> pdg_stats ->
    object
        method get_ma_nstates : int
        method get_ma_ntrans : int
        method get_ma_max_states : int
        method get_ma_max_trans : int
        method get_stats_comp_time : float
        method get_total_time : float
        method get_check_time : float
        method get_final_opt_time : float
        method get_pdg_stats : pdg_stats

        method to_string : string
        method to_row_string : string
    end



(* (bindings, last automaton, lowest changed bindlevel) *)
type ma_info = ParityMa.ma * int

val init_ma_info : ma_info

val set_last_ma : ParityMa.ma -> ma_info -> ma_info 
val get_last_ma : ma_info -> ParityMa.ma
val set_changed_m : int -> ma_info -> ma_info
val get_changed_m : ma_info -> int



class mc_parity : 
    parity_game ->
    object
        method construct_winning_region : ParityMa.ma

        method checkable_errors : pdg_errors
        method ensure_complete : unit

        method compute_phi : ParityMa.ma -> ma_info -> unit
        method project_and_delete : int -> ParityMa.ma -> unit
        method compute_fixed_point : int -> ParityMa.ma -> ma_info -> unit
        method optimise_automaton : ParityMa.ma -> unit

        method input_error_to_string : pdg_error -> string
        method input_error_list_to_string : pdg_error list -> string

        method get_pdg : parity_game

        method set_disable_hard_opt : bool -> unit
        method get_disable_hard_opt : bool
        method set_prod_union_opt : bool -> unit
        method get_prod_union_opt : bool

        method set_gather_stats : bool -> unit
        method get_stats : ParityMa.ma -> mc_parity_stats

        method get_config_results : ParityMa.ma -> interesting_config_results
    end;;




