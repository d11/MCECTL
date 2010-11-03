

open Words;;


val reset_module : unit -> unit

type control_state
type rule_internals 

type config = (control_state * word)

val config_to_string : config -> string


module ControlStateSet : Extset.S with type elt = control_state
module HeadSet : Extset.S with type elt = control_state * character

type headset_filter = HeadSet.elt -> bool

val eloise_stuck : control_state
val abelard_stuck : control_state

val make_control_state : string -> control_state 
val control_state_to_string : control_state -> string
val control_states_equal : control_state -> control_state -> bool

type rule 

val make_rule : control_state -> character -> control_state -> word -> rule 
val rule_p : rule -> control_state
val rule_a : rule -> character
val rule_p' : rule -> control_state
val rule_w : rule -> word
val rule_tuple : rule -> control_state * character * control_state * word
val rule_to_string : rule -> string
val rule_compare : rule -> rule -> int


module RuleSet : Extset.S with type elt = rule
module NextSet : Extset.S with type elt = control_state * word

class pds_stats :
    int -> int -> int -> 
    object
        method get_ncontrols : int
        method get_nchars : int
        method get_ntrans : int

        method to_string : string
        method to_row_string : string
    end;;

type pds_internals

class pds :
    object
        method add_rule : rule -> unit
        method get_next : control_state -> character -> NextSet.t
        method get_all_rules : RuleSet.t
        method rules_iter : (rule -> unit) -> unit
        method rules_state_iter : control_state -> (rule -> unit) -> unit
        method rules_heads_iter : HeadSet.t -> (rule -> unit) -> unit
        method rules_fold : 'a. (rule -> 'a -> 'a) -> 'a -> 'a
        method get_control_states : ControlStateSet.t
        method get_alphabet : CharacterSet.t
        method expand_alphabet : CharacterSet.t -> unit
        method to_channel : out_channel -> unit
        method to_string : string
        method get_stats : pds_stats
        method internals : pds_internals
        method equal : pds -> bool
        method self_map : (rule -> [ `None | `Rule of rule ]) -> unit
        method build_backwards_maps : unit 
        method get_pre_pop_filter : control_state -> headset_filter -> HeadSet.t 
        method get_pre_rew_filter : control_state -> character -> headset_filter -> HeadSet.t 
        method get_pre_push_filter : control_state -> character -> character -> headset_filter -> HeadSet.t 
        method pre_pop_iter_filter : (control_state -> HeadSet.t -> unit) -> headset_filter -> unit  
        method pre_rew_iter_filter : (control_state -> character -> HeadSet.t -> unit) -> headset_filter -> unit 
        method pre_push_iter_filter : (control_state -> character -> character -> HeadSet.t -> unit) -> headset_filter -> unit
        method has_pre_push_filter : control_state -> character -> headset_filter -> bool  
        method pre_push_chars_filter : control_state -> character -> headset_filter -> CharacterSet.t
    end;;








