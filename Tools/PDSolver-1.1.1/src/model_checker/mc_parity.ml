

open Pds;;
open Words;;
open Parity_game;;
open Ma;;
open State;;
open Mc;;




type pdg_error = BadBotUse of rule list | 
                 NeedsBot | 
                 NotComplete of (control_state * character) list | 
                 PropsMissing of control_state list |
                 BadControlStateName of control_state list |
                 BadInterestingConfig of (config * CharacterSet.t);;
type pdg_errors = None | Errors of pdg_error list;;







type state = QStar | QEpsilon | QVal of control_state * int;;

let state_to_string q = 
    match (q) with
      QStar -> "q*"
    | QEpsilon -> "qe"
    | QVal(p, c) ->
        String.concat "" ["("; control_state_to_string p; ","; string_of_int c; ")"];;

let state_to_dot_string n = 
    match n with
      QStar -> "qall"
    | QEpsilon -> "qe"
    | QVal(p, c) -> "s_" ^ (control_state_to_string p) ^ (string_of_int c);;


module BasicState =
    struct 
        type t = state
        let compare = compare
        let to_string = state_to_string
        let to_dot_string = state_to_dot_string
        let can_simulate q q' = 
            match q with
              QStar -> not (q' = QEpsilon)
            | _ -> (q = q')
    end;;

module ParityMa = Ma.Make(BasicState);;
module ParityState = ParityMa.MaState;;

class mc_parity_stats (ma_nstates : int) (ma_ntrans : int)  
                      (ma_max_states : int) (ma_max_trans : int)  
                      (stats_comp_time : float) (total_time : float) (final_opt_time : float) 
                      (pdg_stats : pdg_stats) =
    object
        method get_ma_nstates = ma_nstates
        method get_ma_ntrans = ma_ntrans
        method get_ma_max_states = ma_max_states
        method get_ma_max_trans = ma_max_trans
        method get_stats_comp_time = stats_comp_time
        method get_total_time = total_time
        method get_check_time = total_time -. stats_comp_time
        method get_final_opt_time = final_opt_time
        method get_pdg_stats = pdg_stats

        method to_string =
            "Total time: " ^ (string_of_float total_time) ^ "  " ^
            "Check time: " ^ (string_of_float (total_time -. stats_comp_time)) ^ "\n" ^
            "Gather stats time: " ^ (string_of_float stats_comp_time) ^ "  " ^
            "Final opt time: " ^ (string_of_float final_opt_time) ^ "\n" ^
            "Ma states: " ^ (string_of_int ma_nstates) ^ "  " ^
            "Ma trans: " ^ (string_of_int ma_ntrans) ^ "\n" ^
            "Max states: " ^ (string_of_int ma_max_states) ^ "  " ^
            "Max trans: " ^ (string_of_int ma_max_trans) ^ "\n" ^
            pdg_stats#to_string

        method to_row_string =
            (string_of_float total_time) ^ " " ^
            (string_of_float (total_time -. stats_comp_time)) ^ " " ^
            (string_of_float stats_comp_time) ^ " " ^
            (string_of_float final_opt_time) ^ " " ^
            (string_of_int ma_nstates) ^ " " ^
            (string_of_int ma_ntrans) ^ " " ^
            (string_of_int ma_max_states) ^ " " ^
            (string_of_int ma_max_trans) ^ " " ^
            pdg_stats#to_row_string

    end;;

   
    
(* (bindings, last automaton, lowest changed bindlevel) *)
type ma_info = ParityMa.ma * int

(* initially change level -1 *)
let init_ma_info = (new ParityMa.ma, -1);;

let set_last_ma ma (_, changed_m) =
    (ma, changed_m);;

let get_last_ma (ma, _) = 
    ma;;

let set_changed_m changed_m (old_ma, _) = 
    (old_ma, changed_m);;

let get_changed_m (_, changed_m) = 
    changed_m;;




exception Bad_input of pdg_error list

class mc_parity (pdg : parity_game) =
    object (self)
        val pdg = pdg

        (* we're using a global and methods with side effects to prevent creating a new copy at every method call *)
        (* although we still have to create a shallow copy at each iteration of compute_fixed_point *)
        val mutable disable_hard_opt = false
        val mutable prod_union_opt = false
        val mutable gather_stats = false

        val mutable last_phi_ma = new ParityMa.ma

        (* for testing purposes only *)
        method get_pdg = pdg

        method set_disable_hard_opt ho = 
            disable_hard_opt <- ho
        method get_disable_hard_opt = disable_hard_opt

        method set_prod_union_opt o = 
            prod_union_opt <- o
        method get_prod_union_opt = prod_union_opt


        val mutable max_states = 0
        val mutable max_trans = 0
        val mutable stats_time = 0.0
        val mutable check_time = 0.0
        val mutable final_opt_time = 0.0

        method private get_last_phi_ma =
            last_phi_ma

        method private set_last_phi_ma ma = 
            last_phi_ma <- ma


        method set_gather_stats v = 
            gather_stats <- v

        method private reset_stats = 
            check_time <- 0.0;
            max_states <- 0;
            max_trans <- 0;
            stats_time <- 0.0;
            final_opt_time <- 0.0


        method private reset_last_phi_ma = 
            last_phi_ma <- new ParityMa.ma


        method private gather_stats ma = 
            if gather_stats then (
                let start_time = Sys.time () in
                max_states <- max max_states ma#num_states;
                max_trans <- max max_trans ma#num_trans;
                stats_time <- stats_time +. (Sys.time ()) -. start_time
            )


        method get_stats (ma : ParityMa.ma) = 
            new mc_parity_stats ma#num_states
                                ma#num_trans
                                max_states
                                max_trans
                                stats_time
                                check_time
                                final_opt_time
                                pdg#get_stats


        method private even m = 
            (m mod 2) = 0

        method private assert_good_input =
            match self#checkable_errors with
              None -> ()
            | Errors(es) -> raise (Bad_input(es))

        method ensure_complete =
            let pds = pdg#get_pds in
            let props = pdg#get_props in
            let alphabet = pds#get_alphabet in
            let controls = pds#get_control_states in
            let min_col = props#get_min_colour in
            let odd_prior = if self#even min_col then min_col + 1 else min_col in
            let even_prior = if self#even min_col then min_col else min_col + 1 in
            let added_sinks = ref ControlStateSet.empty in
            let add_new_sink_state sink owner colour = 
                if not (ControlStateSet.mem sink !added_sinks) then
                    added_sinks := ControlStateSet.add sink !added_sinks;
                    let do_char a =
                        let word_a = make_word [a] in
                        pds#add_rule (make_rule sink a sink word_a) in
                    props#add_prop sink owner colour;
                    CharacterSet.iter do_char alphabet in
            let do_head p a = 
                if (pds#get_next p a) =  NextSet.empty then
                    let word_a = make_word [a] in
                    match pdg#get_props#get_owner p with
                      E -> pds#add_rule (make_rule p a eloise_stuck word_a);
                           add_new_sink_state eloise_stuck A odd_prior
                    | A -> pds#add_rule (make_rule p a abelard_stuck word_a);
                           add_new_sink_state abelard_stuck E even_prior in
            let do_p p = CharacterSet.iter (do_head p) alphabet in
            ControlStateSet.iter do_p controls 


        method checkable_errors = 
            let game_rules = pdg#get_pds#get_all_rules in
            let alphabet = pdg#get_pds#get_alphabet in
            let controls = pdg#get_pds#get_control_states in
            let missing_props = 
                let do_p p missed = 
                    try 
                        let _ = pdg#get_props#get_colour p in
                        let _ = pdg#get_props#get_owner p in
                        missed
                    with e ->
                        p::missed in
                let missed = ControlStateSet.fold do_p controls [] in
                if (missed = []) then [] else [PropsMissing(missed)] in
            let bottom_use_errors =
                let do_rule r errors = 
                    let (_, a, _, w) = rule_tuple r in
                    let bad = (* moves bottom from bottom *)
                              ((characters_equal a sbot) && (not (word_ends_with w sbot))) ||
                              (* adds an extra bottom *)
                              ((not (characters_equal a sbot)) && (word_contains w sbot)) in
                    if bad then r::errors else errors in
                let bad_rules = RuleSet.fold do_rule game_rules [] in
                if (bad_rules = []) then [] else [BadBotUse(bad_rules)] in
            let needs_bottom_errors = 
                if not (CharacterSet.exists (characters_equal sbot) alphabet) then
                    [NeedsBot]
                else [] in
(*            let complete_errors = 
                let do_head p a heads = 
                    if (pdg#get_pds#get_rules p a) =  RuleSet.empty then
                        (p, a)::heads 
                    else
                        heads in
                let do_p p heads = (CharacterSet.fold (do_head p) alphabet heads) in
                let incomplete_heads = ControlStateSet.fold do_p controls [] in
                if (incomplete_heads = []) then [] else [NotComplete(incomplete_heads)] in
*)
            let bad_control_state_name_errors = 
                let control_states = pdg#get_pds#get_control_states in
                let bad_states = ref [] in
                if ControlStateSet.mem abelard_stuck control_states then
                    bad_states := abelard_stuck::(!bad_states);
                if ControlStateSet.mem eloise_stuck control_states then
                    bad_states := eloise_stuck::(!bad_states);
                if not (!bad_states = []) then
                    [BadControlStateName(!bad_states)]
                else
                    [] in
            let bad_interesting_config_errors = 
                let do_config errs (p, w) = 
                    let w_chars = word_get_characters w in
                    let strays = CharacterSet.diff w_chars alphabet in
                    if CharacterSet.is_empty strays then
                        errs
                    else
                        (BadInterestingConfig((p, w), strays))::errs in
                List.fold_left do_config [] (pdg#get_interesting_configs) in
            let errors = List.concat [bottom_use_errors; 
                                      needs_bottom_errors; 
                                      (*complete_errors;*)
                                      missing_props;
                                      bad_control_state_name_errors;
                                      bad_interesting_config_errors] in
            if (errors = []) then None else (Errors(errors))

                

        method private init_ma ma = 
            CharacterSet.foreach pdg#get_pds#get_alphabet (fun a -> 
                if characters_equal a sbot then
                    ma#add_hyper_transition (ParityMa.make_ht_list QStar sbot [[QEpsilon]])
                else
                    ma#add_hyper_transition (ParityMa.make_ht_list QStar a [[QStar]])
            );
            ma#add_states (ParityState.StateSet.from_list [QStar; QEpsilon])



        method private get_inits c = 
            ControlStateSet.fold (fun p s -> ParityState.StateSet.add (QVal(p, c)) s) 
                                 pdg#get_pds#get_control_states
                                 ParityState.StateSet.empty 



        method construct_winning_region = 
            Progress_out.pick_spinner ();
            Progress_out.print_string "Initialising...";
            self#reset_stats;
            self#assert_good_input;
            (*self#ensure_complete;*)
            let min_c = pdg#get_props#get_min_colour in
            let start_time = Sys.time () in
            let ma = new ParityMa.ma in
            self#init_ma ma;
            self#reset_last_phi_ma;
            Progress_out.erase_string "Initialising...";
            Progress_out.print_string "Computing winning region: ";
            self#compute_fixed_point min_c ma init_ma_info;
            let inits = self#get_inits min_c in
            ma#set_inits inits; 
            ma#set_finals (ParityState.StateSet.singleton QEpsilon);
            let done_time = Sys.time () in
            Progress_out.erase_string "Computing winning region: ";
            Progress_out.print_string "Optimising automaton...";
            ma#remove_unreachable;
            let opted_time = Sys.time () in
            check_time <- done_time -. start_time;
            final_opt_time <- opted_time -. done_time;
            Progress_out.erase_string "Optimising automaton...";
            Progress_out.print_string "Done.";
            Progress_out.print_newline ();
            ma

        method private rule_image r ma = 
            let (p, _, p', w) = rule_tuple r in
            let c = pdg#get_props#get_colour p in
            ma#get_word_image (QVal(p', c)) w prod_union_opt 


         method optimise_automaton ma = 
            let tran_opt t = 
                let (q, a, ss) = ParityMa.ht_tuple t in
                (`NewTran(ParityMa.make_ht q a (ParityState.optimise_hyper_state_set ss))) in
            ma#self_map tran_opt
                

(*        method private compute_phi_E p a =
            let q = QVal(p, pdg#get_props#get_max_colour + 1) in 
            RuleSet.foreach (pdg#get_pds#get_rules p a) (fun r ->
                let new_tran = new ParityMa.hyper_transition q a (`Set(self#rule_image r)) in
                ma#add_hyper_transition new_tran 
            )
*)


        method private compute_phi_E p ma ma_info =
            let q = QVal(p, pdg#get_props#get_max_colour + 1) in 
            let do_rule r = 
                let new_tran = ParityMa.make_ht q (rule_a r) (self#rule_image r ma) in
                ma#add_hyper_transition new_tran in
            pdg#get_pds#rules_state_iter p do_rule;




        method private compute_phi_A p a ma ma_info =
            let q = QVal(p, pdg#get_props#get_max_colour + 1) in 
            let init_image = ParityState.SetOfStateSets.singleton ParityState.StateSet.empty in
            let nexts = pdg#get_pds#get_next p a in
            if NextSet.is_empty nexts then (
                let new_tran = if characters_equal a sbot then
                                   ParityMa.make_ht_list q a [[QEpsilon]]
                               else 
                                   ParityMa.make_ht_list q a [[QStar]] in
                ma#add_hyper_transition new_tran
            ) else (
                let do_next (p', w) image =
                    let r_image = self#rule_image (make_rule p a p' w) ma in
                    let new_set = ParityState.setofstatesets_prod_union_opt r_image image prod_union_opt in
                    if not prod_union_opt then
                        new_set 
                    else
                        ParityState.optimise_hyper_state_set new_set in
                let image = NextSet.fold do_next nexts init_image in
                let new_tran = ParityMa.make_ht q a image in
                ma#add_hyper_transition new_tran
            )

        method private recursive_inherit_filter cutoff_c q = 
            match q with
              QStar -> false
            | QEpsilon -> false
            | QVal(_, c) -> (c >= cutoff_c)


        method compute_phi ma ma_info = 
            let phi_c = pdg#get_props#get_max_colour + 1 in
            ma#add_states (self#get_inits phi_c);
            let last_phi_ma = self#get_last_phi_ma in
            let last_states = last_phi_ma#get_states in
            ControlStateSet.foreach (pdg#get_pds#get_control_states) (fun p ->
                let priority = pdg#get_props#get_colour p in
                let changed = get_changed_m ma_info in
                let q = QVal(p, phi_c) in 
                if (changed > priority) && (ParityState.StateSet.mem q last_states) then (
                    ma#recursive_move_from (ParityState.StateSet.singleton q) 
                                           (self#recursive_inherit_filter phi_c)
                                           (last_phi_ma);
                ) else (
                    match (pdg#get_props#get_owner p) with
                      E -> self#compute_phi_E p ma ma_info
                    | A -> CharacterSet.foreach (pdg#get_pds#get_alphabet) (fun a ->
                               self#compute_phi_A p a ma ma_info
                           )
                )
            );
            let clone_filter q = 
                match q with
                  QStar -> false
                | QEpsilon -> false
                | QVal(_, m') -> (m' >= phi_c) in
            self#set_last_phi_ma (ma#selective_clone clone_filter);
            self#gather_stats ma


        method project_and_delete m ma =
            let project_state q = 
                match q with
                  QVal(p, c) -> if c = m + 1 then QVal(p, m) else q
                | _ -> q in
            let project_set_of_sets ss = 
                let do_set s = ParityState.StateSet.map project_state s in
                ParityState.SetOfStateSets.map do_set ss in
            let project_tran t =
                let (q, a, ss) = ParityMa.ht_tuple t in
                match q with
                  QVal(p, c) -> (* low priorities won't have any projectable destinations *) 
                                if (pdg#get_props#get_colour p) < m then 
                                    ParityMa.make_ht (project_state q) a ss
                                else (
                                    let new_ss = project_set_of_sets ss in
                                    let new_ss = if prod_union_opt then
                                                     ParityState.optimise_hyper_state_set new_ss
                                                 else
                                                     new_ss in
                                    ParityMa.make_ht (project_state q) a new_ss
                                ) 
                | _ -> t in
            let do_tran t =
                match (ParityMa.ht_q t) with
                  QVal(q, c) -> if c < m then
                                    (`Unchanged)
                                else if c = m then
                                    (`Remove)
                                else
                                    (`AddTran(project_tran t))
                | _ -> (`Unchanged) in
            let q_filter q = 
                match q with
                  QVal(_, c) -> if c = m then
                                    (`Remove)
                                else if c = m + 1 then
                                    (`IterRemove)
                                else
                                    (`Unchanged) 
                | _ -> (`Unchanged) in
            ma#self_map_with_state_filter q_filter do_tran;
            ma#self_map_states (fun s -> (`State (project_state s)));
            if not disable_hard_opt then
                self#optimise_automaton ma


        method private add_all_trans m ma =
            ControlStateSet.foreach pdg#get_pds#get_control_states (fun p ->
                CharacterSet.foreach pdg#get_pds#get_alphabet (fun a ->
                    if (a = sbot) then
                        ma#add_hyper_transition (ParityMa.make_ht_list (QVal(p, m)) a [[QEpsilon]])
                    else
                        ma#add_hyper_transition (ParityMa.make_ht_list (QVal(p, m)) a [[QStar]])
                )
            )



        method compute_fixed_point m ma ma_info =
            let alphabet = pdg#get_pds#get_alphabet in
            let spinner = new Progress_out.spinner in
            spinner#start_spinner;
            let ma_info = ref ma_info in
            ma#add_states (self#get_inits m);
            let greatest = self#even m in
            if greatest then 
                self#add_all_trans m ma;
            let fixed = ref false in 
            while not !fixed do
                let clone_filter q = 
                    match q with
                      QStar -> false
                    | QEpsilon -> false
                    | QVal(_, m') -> (m' >= m) in
                let old_ma = ma#selective_clone clone_filter in

                if m = pdg#get_props#get_max_colour then
                    self#compute_phi ma !ma_info
                else
                    self#compute_fixed_point (m + 1) ma !ma_info;

                ma_info := set_changed_m m !ma_info;

                self#project_and_delete m ma;


                if greatest then (
                    fixed := old_ma#less_than ma alphabet
                ) else (
                    fixed := ma#less_than old_ma alphabet
                );
                spinner#update_spinner;
            done;
            spinner#clear_spinner


        method input_error_to_string e =
            match e with
              BadBotUse(rules) -> 
                List.fold_left (fun s r -> s ^ (rule_to_string r) ^ "\n") 
                               "Rules using stack bottom incorrectly:\n\n"
                               rules
            | NeedsBot -> "Pushdown system does not have the bottom of stack symbol"
            | NotComplete(ccs) -> 
                List.fold_left (fun s (p, a) -> s ^ (control_state_to_string p) ^ " " ^
                                                (character_to_string a) ^ "\n")
                               "The pushdown system is not complete for the following pairs:\n\n"
                               ccs
            | PropsMissing(cs) -> 
                List.fold_left (fun s p -> s ^ (control_state_to_string p) ^ " ")
                               "The following control states are not assigned owners and colours:\n\n"
                               cs
            | BadControlStateName(cs) ->
                List.fold_left (fun s p -> s ^ (control_state_to_string p) ^ " ")
                               "The following control states use names that are reserved for internal use:\n\n"
                               cs
            | BadInterestingConfig(c, chars) -> ("Interesting config '" ^ (config_to_string c) ^ "' " ^
                                                 "has characters " ^ (CharacterSet.to_string chars) ^ " " ^
                                                 "that do not appear in the PDG.")






        method input_error_list_to_string es =
            List.fold_left (fun s e -> s ^ (self#input_error_to_string e) ^ "\n")
                           ""
                           es

             
        (* from stateset s, get state corresponding to control state p *)
        (* assumes there is exactly one *)
        method private get_p_init p s = 
            let is_p q = 
                match q with
                    QVal(p', _) -> control_states_equal p p'
                | _ -> false in
            let singleton = ParityState.StateSet.filter is_p s in
            ParityState.StateSet.choose singleton


 
        method get_config_results (ma : ParityMa.ma) = 
            let ma_inits = ma#get_inits in
            let do_config res (p, w) = 
                let q = self#get_p_init p ma_inits in
                let truth = ma#accepts q w in
                ((p, w), truth)::res in
            let res = List.fold_left do_config [] (pdg#get_interesting_configs) in
            new interesting_config_results res

          
               


    end;;


