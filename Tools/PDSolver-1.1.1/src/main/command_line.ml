
open Extset;;


exception Argument_error of string;;

type out_format = Text | Dot;;

type random_type = NoRandom | Mucalc | Pds | Pds_mucalc | Game;;

type check_type = ChkInput | ChkGame | ChkBoth | ChkMucalc | ChkNone;;

module Argument = 
    struct
        type t = string * string * string * (int -> int)
        let compare = compare
        let to_string (long, short, desc, _) = "\n  " ^ long ^ ", " ^ short ^ ":\n\n      " ^ desc ^ "\n"
    end;;

(* Map from arg to handler function.
 * Each function should take the position of the arg, and return the next
 * position after it has eaten up its required parameters. 
 * E.g. -o maps to output_file i, which reads the filename at position i+1 and
 * returns i+2 (pos i is -o, pos i+1 is filename, pos i+2 is nothing to do with
 * us.)
 *)
module ArgSet = Extset.Make(Argument);;

let arg_handlers = ref ArgSet.empty;;

let add_handler long short desc handler = 
    arg_handlers := ArgSet.add (long, short, desc, handler) !arg_handlers;;

let get_handler switch =
    let check_entry (long, short, _, _) = 
        (switch = long || switch = short) in
    let matches = ArgSet.filter check_entry !arg_handlers in
    if matches = ArgSet.empty then
        raise Not_found 
    else
        let (_, _, _, handler) = ArgSet.choose matches in
        handler;;


let output_file = ref "";;
let output_format = ref Text;;
let input_files = ref [];; 
let verbose = ref false;;
let help_requested = ref false;;
let hard_optimise = ref false;;
let optimise_prod_union = ref true;;
let translate_only = ref false;;
let random_type = ref NoRandom;;
let num_props = ref 5;;
let prop_density = ref 0.1;;
let max_depth = ref 5;;
let num_controls = ref 50;;
let num_chars = ref 50;;
let min_push = ref 0;;
let max_push = ref 2;;
let tran_density_min = ref 1.0;;
let tran_density_max = ref 2.0;;
let min_fp_depth = ref 2;;
let check_type = ref ChkInput;;
let gather_stats = ref false;;
let abelard_density = ref 0.5;;
let num_colours_min = ref 2;;
let num_colours_max = ref 4;;
let timeout = ref 0;;
let show_progress = ref true;;
let generate_data = ref false;;
let data_min_n = ref 5;;
let data_max_n = ref 150;;
let data_step = ref 5;;
let data_reps = ref 10;;
let generate_infinite_data = ref false;;
let restrict_reachable = ref false;;
let init_p = ref "p0";;
let poststar = ref false;;
let eclipse_mode = ref false;;

let get_output_file () = !output_file;;
let get_input_files () = !input_files;;
let get_output_format () = !output_format;;
let get_verbose () = !verbose;;
let get_help_requested () = !help_requested;;
let get_hard_optimise () = !hard_optimise;;
let get_optimise_product_union () = !optimise_prod_union;;
let get_translate_only () = !translate_only || (!check_type = ChkNone);;
let get_random_type () = !random_type;;
let get_num_props () = !num_props;;
let get_num_props () = !num_props;;
let get_prop_density () = !prop_density;;
let get_max_depth () = !max_depth;;
let get_num_controls () = !num_controls;;
let get_num_chars () = !num_chars;;
let get_max_push () = !max_push;;
let get_min_push () = !min_push;;
let get_tran_density_min () = !tran_density_min;;
let get_tran_density_max () = !tran_density_max
let get_min_fp_depth () = !min_fp_depth;;
let get_check_type () = !check_type;;
let get_gather_stats () = !gather_stats;;
let get_abelard_density () = !abelard_density;;
let get_num_colours_min () = !num_colours_min;;
let get_num_colours_max () = !num_colours_max;;
let get_timeout () = !timeout;;
let get_show_progress () = !show_progress;;
let get_generate_data () = !generate_data;;
let get_data_min_n () = !data_min_n;;
let get_data_max_n () = !data_max_n;;
let get_data_step () = !data_step;;
let get_data_reps () = !data_reps;;
let get_generate_infinite_data () = !generate_infinite_data;;
let get_restrict_reachable () = !restrict_reachable;;
let get_init_p () = !init_p;;
let get_poststar () = !poststar;;
let get_eclipse_mode () = !eclipse_mode;;


let read_output_file i = 
    output_file := Sys.argv.(i+1);
    i + 2;;

let read_output_format i = 
    let arg = Sys.argv.(i+1) in 
    if arg = "text" then
        output_format := Text
    else if arg = "dot" then
        output_format := Dot
    else
        raise (Argument_error("Output format " ^ arg ^ " not recognised."));
    i + 2;;

let set_verbose i = 
    verbose := true;
    i + 1;;

let display_help i = 
    help_requested := true;
    print_string "Usage: pds_parity [options] filename1 filename2 . . .\n\n";
    print_string "Iterates through filename1 filename2 . . . where each \n";
    print_string "file contains a pushdown parity game description.  \n";
    print_string "If no filenames are supplied, a game is read from stdin,\n";
    print_string "unless a random game is to be generated.\n\n";
    print_string "Options:\n\n";
    print_string (ArgSet.to_string !arg_handlers);
    print_string "\n\n";
    i + 1;;

let set_nohard_optimise i =
    hard_optimise := false;
    i + 1;;

let set_noopt_prod_union i = 
    optimise_prod_union := false;
    i + 1;;
    
let set_translate_only i =
    translate_only := true;
    i + 1;;

let set_random_type i = 
    let arg = Sys.argv.(i+1) in
    if arg = "mucalc" then
        random_type := Mucalc 
    else if arg = "pds" then
        random_type := Pds
    else if arg = "pds_mucalc" then
        random_type := Pds_mucalc
    else if arg = "game" then
        random_type := Game
    else
        raise (Argument_error("Random type " ^ arg ^ " not recognised."));
    i + 2;;

let set_num_props i = 
    let arg = Sys.argv.(i+1) in
    num_props := int_of_string arg;
    i + 2;;

let set_prop_density i = 
    let arg = Sys.argv.(i+1) in
    prop_density := float_of_string arg;
    if not (!prop_density >= 0.0 && !prop_density <= 1.0) then
        raise (Argument_error("Proposition density of " ^ arg ^ "is out of range [0..1]."));
    i + 2;;

let set_max_depth i = 
    let arg = Sys.argv.(i+1) in
    max_depth := int_of_string arg;
    i + 2;;

let set_num_controls i = 
    let arg = Sys.argv.(i+1) in
    num_controls := int_of_string arg;
    i + 2;;

let set_num_chars i = 
    let arg = Sys.argv.(i+1) in
    num_chars := int_of_string arg;
    i + 2;;

let set_max_push i = 
    let arg = Sys.argv.(i+1) in
    max_push := int_of_string arg;
    i + 2;;

let set_min_push i = 
    let arg = Sys.argv.(i+1) in
    min_push := int_of_string arg;
    i + 2;;


let set_tran_density i = 
    let arg = Sys.argv.(i+1) in
    let arg2 = Sys.argv.(i+2) in
    tran_density_min := float_of_string arg;
    tran_density_max := float_of_string arg2;
    i + 3;;

let set_min_fp_depth i = 
    let arg = Sys.argv.(i+1) in
    min_fp_depth := int_of_string arg;
    i + 2;;

let set_check_type i = 
    let arg = Sys.argv.(i+1) in
    if arg = "input" then
        check_type := ChkInput
    else if arg = "game" then
        check_type := ChkGame
    else if arg = "both" then
        check_type := ChkBoth
    else if arg = "mucalc" then
        check_type := ChkMucalc
    else if arg = "none" then
        check_type := ChkNone
    else
        raise (Argument_error("Check type " ^ arg ^ " not recognised."));
    i + 2;;

let set_gather_stats i =
    gather_stats := true;
    i + 1;;

let set_abelard_density i = 
    let arg = Sys.argv.(i+1) in
    abelard_density := float_of_string arg;
    i + 2;;

let set_num_colours i = 
    let arg = Sys.argv.(i+1) in
    let arg2 = Sys.argv.(i+2) in
    num_colours_min := int_of_string arg;
    num_colours_max := int_of_string arg2;
    i + 3;;

let set_timeout i = 
    let arg = Sys.argv.(i+1) in
    timeout := int_of_string arg;
    i + 2;;

let set_noprogress i =
    show_progress := false;
    i + 1;;

let set_generate_data i =
    generate_data := true;
    i + 1;;

let set_data_min_n i = 
    let arg = Sys.argv.(i+1) in
    data_min_n := int_of_string arg;
    i + 2;;

let set_data_max_n i = 
    let arg = Sys.argv.(i+1) in
    data_max_n := int_of_string arg;
    i + 2;;

let set_data_step i = 
    let arg = Sys.argv.(i+1) in
    data_step := int_of_string arg;
    i + 2;;

let set_data_reps i = 
    let arg = Sys.argv.(i+1) in
    data_reps := int_of_string arg;
    i + 2;;

let set_generate_infinite_data i = 
    generate_data := true;
    generate_infinite_data := true;
    i + 1;;

let set_restrict_reachable i = 
    restrict_reachable := true;
    init_p := Sys.argv.(i+1); 
    i + 2;;

let set_poststar i = 
    poststar := true;
    init_p := Sys.argv.(i+1); 
    i + 2;;


let set_eclipse_mode i = 
    eclipse_mode := true;
    i + 1;;




add_handler "--output-file" "-o" 
            "'-o filename' writes the result to filename." 
            read_output_file;;
add_handler "--output-format" "-f"
            "'-f text' writes in text format, '-f dot' writes in dot format.  Text is default." 
            read_output_format;;
add_handler "--verbose" "-v" 
            "Verbose output to stdout." 
            set_verbose;;
add_handler "--help" "-h"
            "Prints usage information."
            display_help;;
add_handler "--no-hard-optimise" "-no"
            "By default the constructed automaton is optimised at every step.  This flag prevents this."
            set_nohard_optimise;;
add_handler "--translate" "-t"
            "Translate input to a pushdown system and output the result.  Do not model check."
            set_translate_only;;
add_handler "--random-type" "-rt"
            "Generates a random instance.  Options are 'mucalc', 'pds', 'game' and 'pds_mucalc'.  The latter two options are complete instances that will be model checked unless --translate is specified.  None is default.  Also see random options.  Finally, if -rt is set, all input files will be ignored."
            set_random_type;;
add_handler "--num_props" "-npr"
            "The number of propositions to use in a random mucalc formula.  Default is 10"
            set_num_props;;
add_handler "--prop_density" "-pd"
            "A value between 0 and 1 indicating the probability of a particular control state character pair satisfying a proposition.  Default is .1."
            set_prop_density;;
add_handler "--max_depth" "-md"
            "The maximum connective depth of a randomly generated mucalc formula.  Default is 5."
            set_max_depth;;
add_handler "--num_controls" "-nco"
            "The number of control states a randomly generated pds should have.  Default is 50."
            set_num_controls;;
add_handler "--num_chars" "-nch"
            "The number of stack characters a randomly generated pds should have.  Default is 50.  The special value of 0 generates a finite state game."
            set_num_chars;;
add_handler "--max-push" "-mp"
            "The maximum length of w in a random pds rule pa -> p'w.  Default is 2."
            set_max_push;;
add_handler "--min-push" "-minp"
            "The minimum length of w in a random pds rule pa -> p'w.  Default is 0."
            set_min_push;;
add_handler "--tran-density" "-td"
            "The number of pushdown rules in a randomly generated pds as a multiple of nchars*ncontrols, given as min and max (-td min max).  Default is 1 2."
            set_tran_density;;
add_handler "--min-fp-depth" "-fd"
            "The minimum depth of fixed points in the random formula (to avoid too easy cases).  Default is 2."
            set_min_fp_depth;;
add_handler "--check-type" "-ct"
            "Whether to check as a game.  'input' means choose by file type, 'game' means always as a game, 'mucalc' means check as a mu-calculus instance, 'both' means check as mucalc and game.  Finally, 'none' means the same as --translate. Default is 'input'."
            set_check_type;;
add_handler "--gather-stats" "-gs"
            "Whether to compute information on the computation and display.  Default is false (as computation can take time)."
            set_gather_stats;;
add_handler "--abelard-density" "-ad"
            "The probability of a random game control state belonging to Abelard.  Default is .5."
            set_abelard_density;;
add_handler "--num-colour" "-ncol"
            "The number of colours in a random parity game, min and max (-ncol min max).  Default is 2 3."
            set_num_colours;;
add_handler "--no-opt-prod-union" "-nopu"
            "Product union occurs when pairs of states are conjunctioned.  The result is trans1 * trans2 which can grow quickly when several conjunctions occur, optimisation often reduces the size significantly.  This option disables optimisation."
            set_noopt_prod_union;;
add_handler "--timeout" "-to"
            "Kill a model check in this many minutes.  Zero means unlimited.  Default Zero."
            set_timeout;;
add_handler "--no-progress" "-np"
            "Disable the progress meter showing on stdout.  Default is false."
            set_noprogress;;
add_handler "--generate-data" "-gd"
            "Generate a pile of statistics for the journal paper.  Default is false."
            set_generate_data;;
add_handler "--data-min-n" "-dmin"
            "The minimum n to generate data with.  Default is 5."
            set_data_min_n;;
add_handler "--data-max-n" "-dmax"
            "The maximum n to generate data with.  Default is 150."
            set_data_max_n;;
add_handler "--data-step" "-dstep"
            "The step between sizes of n when generating data.  Default is 10."
            set_data_step;;
add_handler "--data-reps" "-dreps"
            "The number of tests to run for a particular n.  Default is 10."
            set_data_reps;;
add_handler "--generate-infinite-data" "-gd-inf"
            "Run the generate data step repeatedly -- to generate as much data as possible.  Data is written to output files as it is generated, so ctrl-c to quit.  Also sets -gd implicitely.  Default is false."
            set_generate_infinite_data;;
add_handler "--reachable" "-r"
            "Restrict backwards operators to configurations reachable from the initial state.  --reachable p0, sets initial state to p0, # is always the initial stack"
            set_restrict_reachable;;
add_handler "--poststar" "-p"
            "Just do poststar analysis -- input must be pdmu, although mu formula is ignored.  Sets init control: --poststar p0."
            set_poststar;;
add_handler "--eclipse-mode" "-em"
            "Instead of outputting automata, just output a list of the accepting control state, character pairs (there is some stack that can be put underneath to give a satisfying config).  Default is false."
            set_eclipse_mode;;


let read_args () =
    let i = ref 1 in
    while !i < Array.length Sys.argv do
        try 
            let handler = get_handler Sys.argv.(!i) in
            i := handler !i;
        with Not_found ->
            input_files := Sys.argv.(!i)::!input_files;
            i := !i + 1
    done;;
