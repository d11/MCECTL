
open Mc_parity;;
open Mc_pds_mucalc;;

type result_type = PdmuStats of mc_pdmu_stats |
                   PdgStats of mc_parity_stats |
                   TimedOut |
                   OutOfMemory |
                   Error of string;;

type datum = int * result_type * result_type;;


class stats_compare :
    mc_pdmu_stats -> mc_parity_stats ->
    object
        method get_time_diff : float
        method get_max_states_diff : float
        method get_max_trans_diff : float

        method to_string : string
        method to_row_string : string
    end;;


class stats_trials :
    string -> [`Ignore | `Use of float * int * int ] ->
    object 
        method begin_record : unit
        method add_datum : datum -> unit
    end;;
        

