
open Pds;;
open Mucalc;;
open Words;;


module PropSet : Extset.S with type elt = id


type property_map_internals 

class proposition_map :
    (control_state * character * (id list)) list ->
    object 
        method get_heads : id -> HeadSet.t
        method get_props : PropSet.t

        method add_proposition : control_state -> character -> id -> unit

        method head_sat : control_state -> character -> id -> bool

        method to_channel : out_channel -> unit
        method to_string : string

        method get_internals : property_map_internals

        method equal : proposition_map -> bool
    end

class pdmu_stats :
    pds_stats -> mu_stats ->
    object
        method get_pds_stats : pds_stats
        method get_mu_stats : mu_stats

        method to_string : string
        method to_row_string : string
    end;;


class pds_mucalc :
    pds -> config list -> mu_formula -> proposition_map ->
    object 
        method get_pds : pds
        method get_props : proposition_map
        method get_fmla : mu_formula
        method get_interesting_configs : config list

        method has_interesting_configs : bool

        method to_channel : out_channel -> unit
        method to_string : string
        method get_stats : pdmu_stats

        method equal : pds_mucalc -> bool 
    end


