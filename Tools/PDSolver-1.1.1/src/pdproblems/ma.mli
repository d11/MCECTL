
open Words;;
open Pds;;
open State;;


module type S =
    sig
        type state

        module MaState : State.S with type state = state

        type hyper_transition 

        type ma_internals

        val make_ht : MaState.state -> character -> MaState.SetOfStateSets.t -> hyper_transition 
        val make_ht_list : MaState.state -> character -> MaState.state list list -> hyper_transition
        val ht_q : hyper_transition -> MaState.state
        val ht_a : hyper_transition -> character
        val ht_ss : hyper_transition -> MaState.SetOfStateSets.t
        val ht_tuple : hyper_transition -> MaState.state * character * MaState.SetOfStateSets.t 
        val ht_cardinal : hyper_transition -> int 
        val ht_to_string : hyper_transition -> string 
        val ht_equal : hyper_transition -> hyper_transition -> bool 

        module MaHeadSet : Extset.S with type elt = MaState.state * character


        class ma :
            object ('a)
                method num_states : int
                method num_trans : int
                method clone : ma
                method selective_clone : (MaState.state -> bool) -> ma
                method add_hyper_transition : hyper_transition -> unit
                (* returns true if the transition already existed *)
                method add_hyper_transition_and_test : hyper_transition -> bool
                method get_char_image : MaState.state -> character -> MaState.SetOfStateSets.t
                method get_char_image_from_set : MaState.StateSet.t -> character -> bool -> MaState.SetOfStateSets.t 
                method char_image_from_hyper : MaState.SetOfStateSets.t -> character -> bool -> MaState.SetOfStateSets.t 
                method get_word_image : MaState.state -> word -> bool -> MaState.SetOfStateSets.t
                method to_channel : out_channel -> unit
                method to_channel_dot : out_channel -> unit
                method to_string : string
                method to_dot_string : string
                method get_internals : ma_internals
                method set_internals : ma_internals -> unit
                method set_internals_selective : (MaState.state -> bool) -> ma_internals -> unit
                method equals : ma -> bool
                method has_defaults : MaState.state -> bool
                method get_non_defaults : MaState.state -> CharacterSet.t
                (* returns all heads with a violation of A <= A' with respect to given alphabet (resolves default trans *)
                method less_than_heads : ma -> CharacterSet.t -> MaHeadSet.t
                method less_than : ma -> CharacterSet.t -> bool
                (* takes an image {Q1,...,Qn} and adds q -a-> Q for each Qi -a-> Q *)
                method copy_from : MaState.state -> MaState.SetOfStateSets.t -> bool -> unit 
                method copy_from_and_test : MaState.state -> MaState.SetOfStateSets.t -> bool -> bool 
                method inherit_from : MaState.StateSet.t -> ma -> unit
                method recursive_inherit_from : MaState.StateSet.t -> (MaState.state -> bool) -> ma -> unit
                method recursive_move_from : MaState.StateSet.t -> (MaState.state -> bool) -> ma -> unit
                method remove_head : MaState.state -> character -> unit
                method self_map : (hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition]) -> unit
                method self_map_with_state_filter : (MaState.state -> [ `Remove | `Unchanged | `Iter | `IterRemove ]) -> (hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition | `AddTran of hyper_transition]) -> unit
                method self_iter : (hyper_transition -> unit) -> unit
                method self_iter_by_state : MaState.state -> (hyper_transition -> unit) -> unit
                method tran_fold : 'a. (hyper_transition -> 'a -> 'a) -> 'a -> 'a 
                method set_inits : MaState.StateSet.t -> unit 
                method get_inits : MaState.StateSet.t
                method is_init : MaState.state -> bool
                method set_finals : MaState.StateSet.t -> unit 
                method get_finals : MaState.StateSet.t
                method add_states : MaState.StateSet.t -> unit
                method remove_states : MaState.StateSet.t -> unit
                method delete_state : MaState.state -> unit
                method get_states : MaState.StateSet.t
                method self_map_states : (MaState.state -> [ `None | `State of MaState.state]) -> unit
                method remove_unreachable : unit
                method get_reachable : MaState.StateSet.t -> MaState.StateSet.t
                method get_accepting_heads : MaHeadSet.t
                method accepting_heads_to_string : string
                method accepting_heads_to_channel : out_channel -> unit
                method accepts : MaState.state -> word -> bool
            end;;
    end






module Make (BasicS : BasicState) :
    sig
        type state = BasicS.t

        module MaState : State.S with type state = state

        type hyper_transition 

        val make_ht : MaState.state -> character -> MaState.SetOfStateSets.t -> hyper_transition 
        val make_ht_list : MaState.state -> character -> MaState.state list list -> hyper_transition
        val ht_q : hyper_transition -> MaState.state
        val ht_a : hyper_transition -> character
        val ht_ss : hyper_transition -> MaState.SetOfStateSets.t
        val ht_tuple : hyper_transition -> MaState.state * character * MaState.SetOfStateSets.t 
        val ht_cardinal : hyper_transition -> int 
        val ht_to_string : hyper_transition -> string 
        val ht_equal : hyper_transition -> hyper_transition -> bool 

        type ma_internals

        module MaHeadSet : Extset.S with type elt = MaState.state * character

        class ma :
            object 
                method num_states : int
                method num_trans : int
                method clone : ma
                method selective_clone : (MaState.state -> bool) -> ma
                method add_hyper_transition_and_test : hyper_transition -> bool
                method add_hyper_transition : hyper_transition -> unit
                method get_char_image : MaState.state -> character -> MaState.SetOfStateSets.t
                method get_char_image_from_set : MaState.StateSet.t -> character -> bool -> MaState.SetOfStateSets.t 
                method char_image_from_hyper : MaState.SetOfStateSets.t -> character -> bool -> MaState.SetOfStateSets.t 
                method get_word_image : MaState.state -> word -> bool -> MaState.SetOfStateSets.t
                method to_channel : out_channel -> unit
                method to_channel_dot : out_channel -> unit
                method to_string : string
                method to_dot_string : string
                method get_internals : ma_internals
                method set_internals : ma_internals -> unit
                method set_internals_selective : (MaState.state -> bool) -> ma_internals -> unit
                method equals : ma -> bool
                method has_defaults : MaState.state -> bool
                method get_non_defaults : MaState.state -> CharacterSet.t
                (* returns all heads with a violation of A <= A' with respect to given alphabet (resolves default trans *)
                method less_than_heads : ma -> CharacterSet.t -> MaHeadSet.t
                method less_than : ma -> CharacterSet.t -> bool
                (* takes an image {Q1,...,Qn} and adds q -a-> Q for each Qi -a-> Q *)
                method copy_from : MaState.state -> MaState.SetOfStateSets.t -> bool -> unit 
                method copy_from_and_test : MaState.state -> MaState.SetOfStateSets.t -> bool -> bool 
                method inherit_from : MaState.StateSet.t -> ma -> unit
                method recursive_inherit_from : MaState.StateSet.t -> (MaState.state -> bool) -> ma -> unit
                method recursive_move_from : MaState.StateSet.t -> (MaState.state -> bool) -> ma -> unit
                method remove_head : MaState.state -> character -> unit
                method self_map : (hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition]) -> unit
                method self_map_with_state_filter : (MaState.state -> [ `Remove | `Unchanged | `Iter | `IterRemove ]) -> (hyper_transition -> [ `Remove | `Unchanged | `NewTran of hyper_transition | `AddTran of hyper_transition]) -> unit
                method self_iter : (hyper_transition -> unit) -> unit
                method self_iter_by_state : MaState.state -> (hyper_transition -> unit) -> unit
                method tran_fold : 'a. (hyper_transition -> 'a -> 'a) -> 'a -> 'a 
                method set_inits : MaState.StateSet.t -> unit 
                method get_inits : MaState.StateSet.t
                method is_init : MaState.state -> bool
                method set_finals : MaState.StateSet.t -> unit 
                method get_finals : MaState.StateSet.t
                method add_states : MaState.StateSet.t -> unit
                method remove_states : MaState.StateSet.t -> unit
                method delete_state : MaState.state -> unit
                method get_states : MaState.StateSet.t
                method self_map_states : (MaState.state -> [ `None | `State of MaState.state]) -> unit
                method remove_unreachable : unit 
                method get_reachable : MaState.StateSet.t -> MaState.StateSet.t
                method get_accepting_heads : MaHeadSet.t
                method accepting_heads_to_string : string
                method accepting_heads_to_channel : out_channel -> unit
                method accepts : MaState.state -> word -> bool
            end;;
    end




