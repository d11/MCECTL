 
open Pds;;

class interesting_config_results (results : (config * bool) list) = 
    object
        val results = results

        method to_string = 
            let do_case s (c, truth) =
                (s ^ 
                 (config_to_string c) ^
                 " : " ^
                 (string_of_bool truth) ^
                 "\n") in
            List.fold_left do_case "" results
    end;;




