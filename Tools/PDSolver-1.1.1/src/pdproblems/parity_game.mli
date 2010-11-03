
open Pds;;
open Words;;

type owner = A | E;;

val owner_to_string : owner -> string

type property_map_internals

class property_map : 
    (control_state * owner * int) list ->
    object
        method get_colour : control_state -> int
        method get_owner : control_state -> owner
        method get_max_colour : int
        method get_min_colour : int

        method add_prop : control_state -> owner -> int -> unit

        method to_channel : out_channel -> unit
        method to_string : string

        method get_internals : property_map_internals

        method equal : property_map -> bool
    end;;

class pdg_stats :
    pds_stats -> int -> 
    object
        method get_pds_stats : pds_stats
        method get_ncols : int

        method to_string : string
        method to_row_string : string
    end;;

class parity_game : 
    pds -> property_map -> config list -> 
    object 
        method get_pds : pds
        method get_props : property_map
        method get_interesting_configs : config list

        method has_interesting_configs : bool

        method to_string : string
        method get_stats : pdg_stats

        method to_channel : out_channel -> unit

        method equal : parity_game -> bool
    end



