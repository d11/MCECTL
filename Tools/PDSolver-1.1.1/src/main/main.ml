
open Pds;;
open Words;;
open Mc_parity;;
open Mc_pds_mucalc;;
open Parity_game;;
open Parser;;
open Lexer;;
open Command_line;;
open Bp_pds_builder;;
open Bp;;
open Pds_mucalc;;
open Mucalc;;
open Random_prob;;
open Conversions;;
open Stats;;


exception Unrecognised_file_type;;
exception Timeout;;

type filetype = FT_PDG | FT_BOOL | FT_MU | FT_PDMU | FT_BPMU;;

Command_line.read_args ();;


let global_reset () = 
    Words.reset_module ();
    Pds.reset_module ();;

let channel_open = ref false;;

let out_channel = ref stdout;;

let close_out_channel () = 
    if !channel_open then (
        close_out !out_channel;
        channel_open := false;
        out_channel := stdout
    );;

let open_out_channel file_name =
    if !channel_open then
        close_out_channel ();
    out_channel := open_out file_name;
    channel_open := true;;


let get_result_string win_reg = 
    if Command_line.get_eclipse_mode () then
        win_reg#accepting_heads_to_string
    else 
        match Command_line.get_output_format () with
           Text -> win_reg#to_string
        | Dot -> win_reg#to_dot_string;;



let result_string_to_channel c win_reg = 
    if Command_line.get_eclipse_mode () then (
        win_reg#accepting_heads_to_channel c
    ) else
        match Command_line.get_output_format () with
          Text -> win_reg#to_channel c
        | Dot -> win_reg#to_channel_dot c;;



let check_pdg pdg = 
    let verbose = Command_line.get_verbose () in
    let gather_stats = Command_line.get_gather_stats () in
    let mc = new mc_parity pdg in
    mc#set_disable_hard_opt (not (Command_line.get_hard_optimise ()));
    mc#set_prod_union_opt (Command_line.get_optimise_product_union ());
    mc#set_gather_stats gather_stats;
    try 
        if verbose then (
            pdg#to_channel !out_channel; 
            flush !out_channel
        );
        let win_reg = mc#construct_winning_region in
        if verbose then
            output_string !out_channel "\nWinning Region: \n\n";
        result_string_to_channel !out_channel win_reg;
        if (pdg#has_interesting_configs) then (
            output_string !out_channel "\n\nInteresting Configurations:\n\n";
            let config_results = mc#get_config_results win_reg in
            output_string !out_channel (config_results#to_string);
        );
        let stats = mc#get_stats win_reg in
        output_string !out_channel ("\n\n" ^ stats#to_string);
        PdgStats(stats)
    with x ->
        match x with
          Mc_parity.Bad_input(es) -> 
            let errors = mc#input_error_list_to_string es in
            prerr_string ("Input errors: \n\n" ^ errors ^ "\n\n");
            Error(errors)
        | Sys_error(e) -> prerr_string ("System error: " ^ e);
                          Error(e)
        | Timeout -> output_string !out_channel "Game checking timed out.\n";
                     TimedOut
        | Out_of_memory -> output_string !out_channel "Game checking ran short of memory.\n";
                           OutOfMemory
        | _ -> prerr_string "Unknown error during checking.\n\n";
               Error("Unknown.");;


let check_pdmu pdmu = 
    let poststar = Command_line.get_poststar () in
    let verbose = Command_line.get_verbose () in
    let gather_stats = Command_line.get_gather_stats () in
    let mc = new mc_pds_mucalc pdmu in
    mc#set_disable_hard_opt (not (Command_line.get_hard_optimise ()));
    mc#set_prod_union_opt (Command_line.get_optimise_product_union ());
    mc#set_gather_stats gather_stats;
    if Command_line.get_restrict_reachable () then (
        let init_p = (make_control_state (Command_line.get_init_p ())) in
        mc#set_restrict_reachable true;
        mc#set_init_p init_p;
    ) else if poststar then (
        let init_p = (make_control_state (Command_line.get_init_p ())) in
        mc#set_init_p init_p;
    );
    try 
        if verbose && (not poststar) then (
            pdmu#to_channel !out_channel;
            output_string !out_channel "\nChecked Formula: ";
            output_string !out_channel (mu_formula_to_string mc#get_normed_fmla);
            flush !out_channel
        );
        let result = if poststar then
                         mc#construct_poststar 
                     else
                         mc#construct_denotation in
        if verbose then (
            if poststar then
                output_string !out_channel "\n\nPost*:\n\n"
            else 
                output_string !out_channel "\n\nDenotation:\n\n"
        );
        result_string_to_channel !out_channel result;
        if (pdmu#has_interesting_configs) then (
            output_string !out_channel "\n\nInteresting Configurations:\n\n";
            let config_results = mc#get_config_results result in
            output_string !out_channel (config_results#to_string);
        );
        let stats = mc#get_stats result in
        output_string !out_channel ("\n\n" ^ stats#to_string);
        PdmuStats(stats)
    with x ->
        match x with
          Bad_input(es) -> 
            let errors = mc#input_error_list_to_string es in
            prerr_string ("Input errors: \n\n" ^ errors ^ "\n\n");
            Error(errors)
        | Sys_error(e) -> prerr_string ("System error: " ^ e);
                          Error(e)
        | Timeout -> output_string !out_channel "Mu calculus checking timed out.\n";
                     TimedOut
        | Out_of_memory -> output_string !out_channel "Mu calculus checking ran short of memory.\n";
                           OutOfMemory
        | _ -> prerr_string "Unknown error during checking.\n\n";
               Error("Unknown.");;


(* timeout code based on http://caml.inria.fr/pub/docs/oreilly-book/html/book-ora168.html *)
let sigalrm_handler = 
    Sys.Signal_handle (fun _ -> raise Timeout);;

let run_check check_f check_arg = 
    let timeout = Command_line.get_timeout () in
    if timeout = 0 then
        check_f check_arg
    else (
        let old_behavior = Sys.signal Sys.sigalrm sigalrm_handler in
        let reset_sigalrm () = Sys.set_signal Sys.sigalrm old_behavior 
        in ignore (Unix.alarm (timeout * 60)) ;
        let res = check_f check_arg in
        ignore (Unix.alarm 0);
        reset_sigalrm ();
        res
    );;


let process_game pdg = 
    let check_type = Command_line.get_check_type () in
    if check_type = ChkInput || check_type = ChkGame || check_type = ChkBoth then
        let _ = run_check check_pdg pdg in ();
    if check_type = ChkMucalc || check_type = ChkBoth then
        let pdmu = pdg_to_pdmu pdg in
        if Command_line.get_translate_only () then
            pdmu#to_channel !out_channel
        else
            let _ = run_check check_pdmu pdmu in ();;




let process_boolean_program bp = 
    let verbose = Command_line.get_verbose () in
    let converter = new bp_pds_builder bp (`AllProps) in
    let (pds, _) = converter#convert in 
    if Command_line.get_translate_only () then (
        if verbose then (
            output_string !out_channel "Boolean Program:\n\n";
            bp_to_channel !out_channel (converter#get_preprocessed_bp);
            output_string !out_channel "Pushdown System:\n\n";
        );
        pds#to_channel !out_channel
    ) else
        output_string !out_channel "Program has no specification."


let process_pdmu pdmu = 
    let check_type = Command_line.get_check_type () in
    if check_type = ChkInput || check_type = ChkMucalc || check_type = ChkBoth then
        let _ = run_check check_pdmu pdmu in ();
    if check_type = ChkGame || check_type = ChkBoth then
        let pdg = pdmu_to_pdg pdmu in
        if Command_line.get_translate_only () then
            pdg#to_channel !out_channel
        else
            let _ = run_check check_pdg pdg in ();;


let process_bpmu bpmu = 
    let verbose = Command_line.get_verbose () in
    let start_time = Sys.time () in
    let props = mu_formula_prop_ids (bpmu#get_fmla) in
    let converter = new bp_pds_builder (bpmu#get_bp) (`Props(props)) in
    let convert_time = Sys.time () -. start_time in
    let (pds, props) = converter#convert in 
    if Command_line.get_translate_only () then (
        if verbose then (
            output_string !out_channel "Boolean Program:\n\n";
            bp_to_channel !out_channel converter#get_preprocessed_bp;
            output_string !out_channel "Pushdown System:\n\n";
        );
        pds#to_channel !out_channel
    ) else (
        let pdmu = new pds_mucalc pds [] (bpmu#get_fmla) props in
        if verbose then (
            output_string !out_channel "Boolean Program: \n\n";
            bp_to_channel !out_channel converter#get_preprocessed_bp;
            output_string !out_channel "\n\n"; 
        );
        process_pdmu pdmu;
    );
    output_string !out_channel ("Conversion time: " ^ (string_of_float convert_time) ^ "\n")







let get_filetype filename = 
    try 
        let dot_pos = String.rindex filename '.' in
        let ext_len = (String.length filename) - dot_pos - 1 in
        let extension = String.sub filename (dot_pos + 1) ext_len in
        if extension = "pdg" then
            FT_PDG
        else if extension = "bp" then
            FT_BOOL
        else if extension = "mu" then
            FT_MU
        else if extension = "pdmu" then
            FT_PDMU
        else if extension = "bpmu" then
            FT_BPMU
        else
            raise Unrecognised_file_type
    with Not_found ->
        raise Unrecognised_file_type;;



let process_file filename = 
    global_reset ();
    try 
        let file_in = open_in filename in
        let lexbuf = Lexing.from_channel file_in in
        match get_filetype filename with
            FT_PDG -> let pdg = Parser.parity_game Lexer.token lexbuf in 
                      process_game pdg
          | FT_BOOL -> let bp = Parser.boolean_program Lexer.token lexbuf in
                       process_boolean_program bp
          | FT_MU -> let fmla = Parser.mu_formula Lexer.token lexbuf in
                     print_string (Mucalc.mu_formula_to_string fmla)
          | FT_PDMU -> let pdmu = Parser.pds_mucalc Lexer.token lexbuf in
                       process_pdmu pdmu
          | FT_BPMU -> let bpmu = Parser.bp_mucalc Lexer.token lexbuf in
                       process_bpmu bpmu
    with x ->
        match x with
          Parsing.Parse_error -> 
            prerr_string ("Error parsing " ^ filename ^ ".\n");
        | Unrecognised_file_type ->
            prerr_string ("Unrecognised file extension in " ^ filename);
        | Sys_error(e) ->
            prerr_string ("File " ^ filename ^ " could not be opened.  Error: " ^ e)
        | Failure(msg) -> 
            prerr_string ("Failure with " ^ filename ^ ": " ^ msg)
        | x -> 
            prerr_string ("Unknown error with " ^ filename ^ ".");;

let process_files input_files =
    List.iter process_file input_files;;


let mucalc_filter fmla = 
    (default_mu_filter fmla) &&
    ((mu_fixed_point_depth fmla) >= Command_line.get_min_fp_depth ());;

let process_random_mucalc () = 
    let nprops = Command_line.get_num_props () in
    let max_depth = Command_line.get_max_depth () in 
    let fmla = random_mu_calc_filter_nprops mucalc_filter nprops max_depth in
    output_string !out_channel (mu_formula_to_string fmla);;

let process_random_pds () =
    let ncontrols = Command_line.get_num_controls () in
    let nchars = Command_line.get_num_chars () in
    let min_push = Command_line.get_min_push () in
    let max_push = Command_line.get_max_push () in
    let tran_d_min = Command_line.get_tran_density_min () in
    let tran_d_max = Command_line.get_tran_density_max () in
    let pds = random_pds ncontrols nchars min_push max_push tran_d_min tran_d_max in
    pds#to_channel !out_channel;;
    

let process_random_pds_mucalc () = 
    let tran_only = Command_line.get_translate_only () in
    let ncontrols = Command_line.get_num_controls () in
    let nchars = Command_line.get_num_chars () in
    let min_push  = Command_line.get_min_push () in
    let max_push = Command_line.get_max_push () in
    let tran_d_min = Command_line.get_tran_density_min () in
    let tran_d_max = Command_line.get_tran_density_max () in
    let nprops = Command_line.get_num_props () in
    let prop_density = Command_line.get_prop_density () in
    let max_depth = Command_line.get_max_depth () in
    let pdmu = random_pds_mucalc nprops 
                                 prop_density 
                                 max_depth 
                                 mucalc_filter 
                                 ncontrols 
                                 nchars 
                                 min_push 
                                 max_push 
                                 tran_d_min
                                 tran_d_max in
    if tran_only then 
        pdmu#to_channel !out_channel
    else 
        process_pdmu pdmu;;
        
let process_random_game () =         
    let tran_only = Command_line.get_translate_only () in
    let ncontrol = Command_line.get_num_controls () in
    let nchar = Command_line.get_num_chars () in
    let min_push = Command_line.get_min_push () in
    let max_push = Command_line.get_max_push () in
    let tran_d_min = Command_line.get_tran_density_min () in
    let tran_d_max = Command_line.get_tran_density_max () in
    let abelard_density = Command_line.get_abelard_density () in
    let ncols_min = Command_line.get_num_colours_min () in
    let ncols_max = Command_line.get_num_colours_max () in
    let pdg = random_pdg_ncols ncontrol 
                               nchar 
                               min_push
                               max_push 
                               tran_d_min
                               tran_d_max
                               ncols_min
                               ncols_max
                               abelard_density in
    if tran_only then 
        pdg#to_channel !out_channel
    else 
        process_game pdg;;



let process_random () = 
    match Command_line.get_random_type () with
      NoRandom -> ()
    | Mucalc -> process_random_mucalc () 
    | Pds -> process_random_pds () 
    | Pds_mucalc -> process_random_pds_mucalc ()
    | Game -> process_random_game ();;




let process_stdin () = 
    let lexbuf = Lexing.from_channel stdin in
    while true do
        let pdg = Parser.parity_game Lexer.token lexbuf in
        process_game pdg;
    done;;


let process_instance () = 
    let out_file = Command_line.get_output_file () in
    if not (out_file = "") then
        open_out_channel out_file;
    let input_files = Command_line.get_input_files () in
    if input_files = [] && 
       not (Command_line.get_help_requested ()) &&
       (Command_line.get_random_type () = NoRandom) then
        process_stdin ()
    else if ((Command_line.get_random_type ()) = NoRandom) then
        process_files input_files
    else
        process_random ();
    if not (out_file = "") then
        close_out_channel ();;



let do_mu_test () = 
    not ((Command_line.get_check_type ()) = ChkGame);;

let do_g_test () = 
    not ((Command_line.get_check_type ()) = ChkMucalc);;


let do_trial_pdmu count n i = 
    global_reset ();
    let file_name = Command_line.get_output_file () ^ 
                    "_" ^ (string_of_int count) ^
                    "_" ^ (string_of_int n) ^
                    "_" ^ (string_of_int i) in
    let ncontrols = n in
    let nchars = n in
    let min_push = Command_line.get_min_push () in
    let max_push = Command_line.get_max_push () in
    let tran_d_min = Command_line.get_tran_density_min () in
    let tran_d_max = Command_line.get_tran_density_max () in
    let nprops = Command_line.get_num_props () in
    let prop_density = Command_line.get_prop_density () in
    let max_depth = Command_line.get_max_depth () in
    let pdmu = random_pds_mucalc nprops 
                                 prop_density 
                                 max_depth 
                                 mucalc_filter 
                                 ncontrols 
                                 nchars 
                                 min_push
                                 max_push 
                                 tran_d_min
                                 tran_d_max in
    let mures = if do_mu_test () then (
        open_out_channel (file_name ^ "_pdmu");
        (* force a garbage collection before running a trial *)
        Gc.compact (); 
        let res = run_check check_pdmu pdmu in
        close_out_channel ();
        res
    ) else (
         Error("Test disabled on command line.") 
    ) in
    let gres = if do_g_test () then (
        let pdg = pdmu_to_pdg pdmu in
        open_out_channel (file_name ^ "_pdg");
        Gc.compact (); 
        let res = run_check check_pdg pdg in
        close_out_channel ();
        res
    ) else (
        Error("Test disabled on command line.") 
    ) in
    (n, mures, gres);;


let do_trial_pdg count n i = 
    global_reset ();
    let file_name = Command_line.get_output_file () ^ 
                    "_" ^ (string_of_int count) ^
                    "_" ^ (string_of_int n) ^
                    "_" ^ (string_of_int i) in
    let ncontrol = n in
    let nchar = n in
    let min_push = Command_line.get_min_push () in
    let max_push = Command_line.get_max_push () in
    let tran_d_min = Command_line.get_tran_density_min () in
    let tran_d_max = Command_line.get_tran_density_max () in
    let abelard_density = Command_line.get_abelard_density () in
    let ncols_min = Command_line.get_num_colours_min () in
    let ncols_max = Command_line.get_num_colours_max () in
    let pdg = random_pdg_ncols ncontrol 
                               nchar 
                               min_push
                               max_push 
                               tran_d_min
                               tran_d_max
                               ncols_min
                               ncols_max
                               abelard_density in
    let gres = if do_g_test () then (
        open_out_channel (file_name ^ "_pdg");
        (* force a garbage collection before running a trial *)
        Gc.compact (); 
        let res = run_check check_pdg pdg in
        close_out_channel ();
        res
    ) else (
        Error("Test disabled on command line.")
    ) in
    let mures = if do_mu_test () then (
        let pdmu = pdg_to_pdmu pdg in
        open_out_channel (file_name ^ "_pdmu");
        Gc.compact (); 
        let res = run_check check_pdmu pdmu in
        close_out_channel ();
        res
    ) else (
        Error("Test disabled on command line.")
    ) in
    (n, mures, gres);;



let trial_mucalc () = 
    match Command_line.get_random_type () with
      Pds_mucalc -> true
    | Game -> false
    | _ -> failwith "Random trials must either be random type pds_mucalc or game.";;













let generate_data_main count summary = 
    let min_n = Command_line.get_data_min_n () in
    let max_n = Command_line.get_data_max_n () in
    let step = Command_line.get_data_step () in
    let reps = Command_line.get_data_reps () in
    let mucalc = trial_mucalc () in
    let rec get_data n = 
        if n > max_n then
            ()
        else (
            let rec do_reps rep = 
                if rep > 0 then (
                    Progress_out.print_string ("Count = " ^ (string_of_int count) ^
                                               "; N = " ^ (string_of_int n) ^ 
                                               "; Rep = " ^ (string_of_int rep) ^ ": ");
                    if not (Command_line.get_gather_stats ()) then
                        Progress_out.print_string (" (Warning: mid-computation stats are not being acquired.)");
                    Progress_out.print_newline ();
                    try 
                        let res = if mucalc then 
                                      do_trial_pdmu count n rep
                                  else
                                      do_trial_pdg count n rep in
                        summary#add_datum res;
                        do_reps (rep - 1)
                    with _ -> summary#add_datum (n, Error("Unknown, serious."), Error("Unknown, serious."));
                              do_reps (rep - 1)
                ) else
                    get_data (n + step) in
            do_reps reps
        ) in
    get_data min_n



let generate_data () = 
    let summary = new stats_trials (Command_line.get_output_file ()) (`Ignore) in
    summary#begin_record;
    generate_data_main 0 summary;
    let rec do_data n = 
        generate_data_main n summary;
        do_data (n+1) in
    if Command_line.get_generate_infinite_data () then
        do_data 1;;

let main () = 
    Progress_out.set_display (Command_line.get_show_progress ());
    if Command_line.get_generate_data () then
        generate_data ()
    else
        process_instance ();;


main ();;
