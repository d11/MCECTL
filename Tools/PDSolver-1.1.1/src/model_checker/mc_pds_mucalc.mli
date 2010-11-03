



open Pds;;
open Words;;
open Pds_mucalc;;
open Ma;;
open State;;
open Mucalc;;
open Mc;;

open Str;;



type pdmu_error = BadBotUse of rule list | 
                  NeedsBot | 
                  NoInit | 
                  BadSubformula of mu_formula |
                  BadInterestingConfig of (config * CharacterSet.t) |
                  UnboundVariables of IdSet.t;;

type pdmu_errors = None | Errors of pdmu_error list;;


type state = QStar | 
             QEpsilon | 
             QFail |
             QVal of control_state * mu_formula * int |
             QInt of control_state * mu_formula * int * character;;


val state_to_string : state -> string

module BasicState :
    sig
        type t = state
        val compare : t -> t -> int
        val to_string : t -> string
        val to_dot_string : t -> string
        val can_simulate : t -> t -> bool
    end

module PdsMuMa : Ma.S with type state = state

class mc_pdmu_stats : 
    int -> int -> int -> int -> float -> float -> float -> pdmu_stats ->
    object
        method get_ma_nstates : int
        method get_ma_ntrans : int
        method get_ma_max_states : int
        method get_ma_max_trans : int
        method get_stats_comp_time : float
        method get_total_time : float
        method get_check_time : float
        method get_final_opt_time : float
        method get_pdmu_stats : pdmu_stats

        method to_string : string
        method to_row_string : string
    end


exception Bad_input of pdmu_error list



module BindMap : Map.S with type key = id;;

(* (bindings, last automaton, lowest changed bindlevel) *)
type ma_info = int BindMap.t * PdsMuMa.ma * bool * int
(* if -1 is proposition level, -2 is, well, lower *)
val init_ma_info : ma_info
val set_binding : id -> int -> ma_info -> ma_info
val get_binding : id -> ma_info -> int
val set_last_ma : PdsMuMa.ma -> ma_info -> ma_info 
val get_last_ma : ma_info -> PdsMuMa.ma 
val set_changed_c : int -> ma_info -> ma_info 
val get_changed_c : ma_info -> int

class mc_pds_mucalc : 
    pds_mucalc -> 
    object 
        method set_disable_hard_opt : bool -> unit
        method get_disable_hard_opt : bool
        method set_prod_union_opt : bool -> unit
        method get_prod_union_opt : bool

        method set_gather_stats : bool -> unit
        method set_restrict_reachable : bool -> unit
        method set_init_p : control_state -> unit

        method checkable_errors : pdmu_errors


        method construct_poststar : PdsMuMa.ma 
        method construct_denotation : PdsMuMa.ma 

        method input_error_to_string : pdmu_error -> string
        method input_error_list_to_string : pdmu_error list -> string              


        (* for testing purposes only *)
        method get_pdmu : pds_mucalc
        method get_normed_fmla : mu_formula

        method dispatch : mu_formula -> 
                          int -> 
                          PdsMuMa.ma -> 
                          ma_info ->
                          PdsMuMa.MaState.StateSet.t

        method project_rename_and_delete : PdsMuMa.MaState.StateSet.t -> 
                                           PdsMuMa.MaState.StateSet.t -> 
                                           int -> 
                                           PdsMuMa.ma -> 
                                           unit

        method optimise_automaton : PdsMuMa.ma -> unit 

        method get_stats : PdsMuMa.ma -> mc_pdmu_stats

        method get_config_results : PdsMuMa.ma -> interesting_config_results

    end;;





