
module type OrderedType = 
    sig
        include Set.OrderedType
        val to_string : t -> string
    end

module type S =
    sig
        include Set.S
        val to_string : t -> string
        val from_list : elt list -> t
        val to_list : t -> elt list
        val map : (elt -> elt) -> t -> t
        val foreach : t -> (elt -> unit) -> unit
        val pair : elt -> elt -> t
    end

module Make (Ord : OrderedType) : 
    sig
        include Set.S with type elt = Ord.t
        val to_string : t -> string
        val from_list : elt list -> t
        val to_list : t -> elt list
        val map : (elt -> elt) -> t -> t
        val foreach : t -> (elt -> unit) -> unit
        val pair : elt -> elt -> t
    end
       

