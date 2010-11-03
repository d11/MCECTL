
open Bp;;
open Mucalc;;

class bp_mucalc (bp : boolean_program) (fmla : mu_formula) = 
    object 
        val bp = bp 
        val fmla = fmla

        method get_bp = bp 
        method get_fmla = fmla

        method to_string = 
            "Boolean Program:\n\n" ^ (bp_to_string bp) ^ 
            "\n\nMu Formula:\n\n" ^ (mu_formula_to_string fmla)

        method equal (bpmu : bp_mucalc) =
            ((compare bp (bpmu#get_bp)) = 0) &&
            ((compare fmla (bpmu#get_fmla)) = 0)
    end;;


