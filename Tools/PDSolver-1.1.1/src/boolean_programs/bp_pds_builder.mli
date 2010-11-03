
open Pds;;
open Bp;;
open Pds_mucalc;;



(* Two phases: first add global declarations, then get into the definition of methods and what not.
 * Once methods are being defined, the global declarations cannot change.  The
 * change is automatic when the first method is added.
 *)

class bp_pds_builder : 
    boolean_program ->
    [ `AllProps | `Props of id list ] ->
    object
        method get_preprocessed_bp : boolean_program
        method convert : pds * proposition_map
    end
