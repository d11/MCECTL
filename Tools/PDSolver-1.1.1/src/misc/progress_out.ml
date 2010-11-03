
let display = ref true;;

let set_display v = 
    display := v;;


Random.self_init ();;

let spin_choices = [| [|".";"o";"O";"o"|];
                      [|"|>  |";"| > |";"|  >|";"|  <|";"| < |";"|<  |"|];
                      [|"|";"(";"|";")"|];
                      [|":- ";":o ";":O ";":o "|];
                      (* Courtesy of Leopold Haller *)
                      [|"oo ";"Oo ";"oo "; "-o "|];
                      (* Courtesy of Leopold Haller *)
                      [|"lemme think "; 
                        "I got it ";
                        "no, wait ";
                        "I messed up ";
                        "mmmh ";
                        "but wait "|]
                  |];;

let spin_choice = ref spin_choices.(Random.int (Array.length spin_choices));;

let pick_spinner () =
    spin_choice := spin_choices.(Random.int (Array.length spin_choices));;





let backspace = char_of_int 8;;

let erase_string s = 
    if !display then
        let rec erase_chars n = 
            if n > 0 then (
                print_char backspace;
                print_char ' ';
                print_char backspace;
                erase_chars (n - 1)
            ) in
        erase_chars (String.length s);;

class spinner = 
    object 
        val mutable pos = 0
        val spins = !spin_choice

        method start_spinner =
            if !display then (
                pos <- 0;
                Pervasives.print_string spins.(pos);
                Pervasives.flush stdout
            )

        method update_spinner =
            if !display then (
                erase_string spins.(pos);
                pos <- (pos + 1) mod (Array.length spins);
                Pervasives.print_string spins.(pos);
                Pervasives.flush stdout
            )

        method clear_spinner = 
            erase_string spins.(pos)
    end;;


let print_string s = 
    if !display then (
        Pervasives.print_string s;
        Pervasives.flush stdout
    );;

let print_newline () = 
    if !display then
        Pervasives.print_newline ();;
