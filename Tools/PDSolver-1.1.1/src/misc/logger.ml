

let enable_logs = ref false;;


let print_string s = 
    if !enable_logs then
        Pervasives.print_string s;;

let print_newline () = 
    if !enable_logs then
        Pervasives.print_newline ();;
    
