

module type OrderedType = 
    sig
        include Set.OrderedType
        val to_string : t -> string
    end;;

module type S =
    sig
        include Set.S
        val to_string : t -> string
        val from_list : elt list -> t
        val to_list : t -> elt list
        val map : (elt -> elt) -> t -> t
        val foreach : t -> (elt -> unit) -> unit
        val pair : elt -> elt -> t
    end;;

module Make = 
    functor (Ord : OrderedType) ->
        struct
            include Set.Make(Ord)
            let to_string set = 
                let do_ele e s = 
                    s ^ " " ^ (Ord.to_string e) in 
                "{" ^ (fold do_ele set "") ^ " }";;
            let to_list = elements
            let from_list l = 
                List.fold_right add l empty;;
            let map f set =
                let do_ele e new_set = add (f e) new_set in
                fold do_ele set empty
            let foreach set f = 
                iter f set
            let pair e1 e2 = 
                let s = singleton e1 in
                add e2 s
        end;;
       

