
(* The states of a multi automaton *)

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



module Make = 
    functor (BasicS : BasicState) -> 
        struct
            type state = BasicS.t

            let state_to_string = BasicS.to_string;;
            let state_to_dot_string = BasicS.to_dot_string

            let states_equal s s' = ((BasicS.compare s s') = 0)

            module StateSet = Extset.Make(BasicS);;
            module SetOfStateSets = Extset.Make(StateSet);;

            let setofstatesets_from_list_list ll = 
                SetOfStateSets.from_list (List.map StateSet.from_list ll);;


            (* S <~= S' for the similation op in the paper *)
            let set_easier_than s s' =
                let check_ele q =
                    StateSet.exists (BasicS.can_simulate q) s' in
                StateSet.for_all check_ele s;;


            let hyper_set_less_than ss ss' = 
                let set_simulated s s' = set_easier_than s' s in
                let check_set s = SetOfStateSets.exists (set_simulated s) ss' in
                SetOfStateSets.for_all check_set ss;;


            (* remove all s \in ss such that there is s' != s with s' <~= s *)
            let optimise_hyper_state_set ss = 
                let not_redundant s = 
                    let redundifies s s' = 
                        if not (StateSet.equal s s') then (
                            let can_replace_s = set_easier_than s' s in
                            if can_replace_s then (
                                let can_be_replaced_by_s = set_easier_than s s' in
                                if can_be_replaced_by_s then (
                                    let size_s = StateSet.cardinal s in
                                    let size_s' = StateSet.cardinal s' in
                                    if size_s' < size_s then
                                        true
                                    else if size_s < size_s' then
                                        false
                                    else
                                        (compare s s') > 0
                                ) else
                                    true
                            ) else
                                false
                        ) else
                            false in
                    not (SetOfStateSets.exists (redundifies s) ss) in
                SetOfStateSets.filter not_redundant ss;;


            (* For ss = {s1, ..., sn} and ss' = {S1, ..., Sm} computes
             * { si u Sj | for all i, j }
             * If you can figure out what this code i wrote is up to, you're a better man
             * than me . . .
             *)
            let setofstatesets_prod_union ss ss' = 
                let union_with_ss' s prod_u = 
                    let do_set s' prod_u' = 
                        SetOfStateSets.add (StateSet.union s s') prod_u' in
                    SetOfStateSets.fold do_set ss' prod_u in
                SetOfStateSets.fold union_with_ss' ss SetOfStateSets.empty;;


            let setofstatesets_prod_union_opt ss ss' do_opt = 
                let res = setofstatesets_prod_union ss ss' in                
                if do_opt then
                    optimise_hyper_state_set res
                else
                    res;;

        end;; 
