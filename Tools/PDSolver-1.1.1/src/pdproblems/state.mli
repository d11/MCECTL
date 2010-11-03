

module type BasicState =
    sig
        type t 
        val compare : t -> t -> int
        val to_string : t -> string
        val to_dot_string : t -> string
        val can_simulate : t -> t -> bool
    end;;


module type S = 
    sig
        type state

        val state_to_string : state -> string
        val state_to_dot_string : state -> string

        val states_equal : state -> state -> bool

        module StateSet : Extset.S with type elt = state
        module SetOfStateSets : Extset.S with type elt = StateSet.t

        val setofstatesets_from_list_list : state list list -> SetOfStateSets.t
        val setofstatesets_prod_union : SetOfStateSets.t -> SetOfStateSets.t -> SetOfStateSets.t
        val setofstatesets_prod_union_opt : SetOfStateSets.t -> SetOfStateSets.t -> bool -> SetOfStateSets.t

        val set_easier_than : StateSet.t -> StateSet.t -> bool
        val hyper_set_less_than : SetOfStateSets.t -> SetOfStateSets.t -> bool
        val optimise_hyper_state_set : SetOfStateSets.t -> SetOfStateSets.t
    end;;


module Make (BasicS : BasicState) : 
    sig
        type state = BasicS.t

        val state_to_string : state -> string
        val state_to_dot_string : state -> string

        val states_equal : state -> state -> bool

        module StateSet : Extset.S with type elt = state
        module SetOfStateSets : Extset.S with type elt = StateSet.t

        val setofstatesets_from_list_list : state list list -> SetOfStateSets.t
        val setofstatesets_prod_union : SetOfStateSets.t -> SetOfStateSets.t -> SetOfStateSets.t
        val setofstatesets_prod_union_opt : SetOfStateSets.t -> SetOfStateSets.t -> bool -> SetOfStateSets.t

        val set_easier_than : StateSet.t -> StateSet.t -> bool
        val hyper_set_less_than : SetOfStateSets.t -> SetOfStateSets.t -> bool
        val optimise_hyper_state_set : SetOfStateSets.t -> SetOfStateSets.t
    end
       

