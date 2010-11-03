
module type ResultType =
    sig
        type t
        val equal : t -> t -> bool
        val to_string : t -> string
    end

module Tester (RT : ResultType) : 
    sig
        type res_type = RT.t
        val add_test : string -> (unit -> res_type) -> res_type -> unit
        val run_tests : unit -> unit
    end

