

open Mucalc;;
open Words;;
open Bp;;


class bp_mucalc :
    boolean_program -> mu_formula -> 
    object 
        method get_bp : boolean_program 
        method get_fmla : mu_formula

        method to_string : string

        method equal : bp_mucalc -> bool 
    end


