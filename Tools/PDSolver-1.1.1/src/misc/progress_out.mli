
val set_display : bool -> unit

val pick_spinner : unit -> unit

class spinner : 
    object 
        method start_spinner : unit
        method update_spinner : unit 
        method clear_spinner : unit
    end;;


val erase_string : string -> unit
val print_string : string -> unit
val print_newline : unit -> unit
