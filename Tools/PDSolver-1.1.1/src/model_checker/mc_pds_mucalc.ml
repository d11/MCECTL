

open Pds;;
open Words;;
open Pds_mucalc;;
open Ma;;
open State;;
open Mucalc;;
open Mc;;

open Str;;
open Gc;;



(* TODO: unbound variables, reused variable names, fixed point of Z or x *)
type pdmu_error = BadBotUse of rule list | 
                  NeedsBot | 
                  NoInit | 
                  BadSubformula of mu_formula |
                  BadInterestingConfig of (config * CharacterSet.t) |
                  UnboundVariables of IdSet.t;;
type pdmu_errors = None | Errors of pdmu_error list;;


type state = QStar | 
             QEpsilon | 
             QFail |
             QVal of control_state * mu_formula * int | 
             QInt of control_state * mu_formula * int * character;;

let state_to_string q = 
    match (q) with
      QStar -> "q*"
    | QEpsilon -> "qe"
    | QFail -> "qfail"
    | QVal(p, f, c) ->
        String.concat "" ["("; control_state_to_string p; ","; 
                               mu_formula_to_string f; ",";
                               string_of_int c; ")"]
    | QInt(p, f, c, a) ->
        String.concat "" ["("; control_state_to_string p; ","; 
                               mu_formula_to_string f; ",";
                               string_of_int c; ",";
                               character_to_string a; ")"];;

let replace_spaces s = 
    let space = regexp "\\ " in
    global_replace space "_" s 


let state_to_dot_string n = 
    match n with
      QStar -> "qall"
    | QEpsilon -> "qe"
    | QFail -> "qfail"
    | QVal(p, f, c) -> "s_" ^
                       (control_state_to_string p) ^ 
                       (mu_formula_to_dot_string f) ^
                       (string_of_int c)
    | QInt(p, f, c, a) -> "s_" ^
                          (control_state_to_string p) ^ 
                          (mu_formula_to_dot_string f) ^
                          (string_of_int c) ^
                          (character_to_string a);;

let state_get_control q = 
    match (q) with
    | QVal(p, f, c) -> p
    | QInt(p, f, c, a) -> p
    | _ -> failwith "state_get_control was passed a state without a control..."


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

module PdsMuMa = Ma.Make(BasicState);;
module PdsMuState = PdsMuMa.MaState;;



class mc_pdmu_stats (ma_nstates : int) (ma_ntrans : int)  
                    (ma_max_states : int) (ma_max_trans : int)  
                    (stats_comp_time : float) (total_time : float) (final_opt_time : float) 
                    (pdmu_stats : pdmu_stats) =
    object
        method get_ma_nstates = ma_nstates
        method get_ma_ntrans = ma_ntrans
        method get_ma_max_states = ma_max_states
        method get_ma_max_trans = ma_max_trans
        method get_stats_comp_time = stats_comp_time
        method get_total_time = total_time
        method get_check_time = total_time -. stats_comp_time
        method get_final_opt_time = final_opt_time
        method get_pdmu_stats = pdmu_stats

        method to_string =
            "Total time: " ^ (string_of_float total_time) ^ "  " ^
            "Check time: " ^ (string_of_float (total_time -. stats_comp_time)) ^ "\n" ^
            "Gather stats time: " ^ (string_of_float stats_comp_time) ^ "  " ^
            "Final opt time: " ^ (string_of_float final_opt_time) ^ "\n" ^
            "Ma states: " ^ (string_of_int ma_nstates) ^ "  " ^
            "Ma trans: " ^ (string_of_int ma_ntrans) ^ "\n" ^
            "Max states: " ^ (string_of_int ma_max_states) ^ "  " ^
            "Max trans: " ^ (string_of_int ma_max_trans) ^ "\n" ^
            pdmu_stats#to_string

        method to_row_string =
            (string_of_float total_time) ^ " " ^
            (string_of_float (total_time -. stats_comp_time)) ^ " " ^
            (string_of_float stats_comp_time) ^ " " ^
            (string_of_float final_opt_time) ^ " " ^
            (string_of_int ma_nstates) ^ " " ^
            (string_of_int ma_ntrans) ^ " " ^
            (string_of_int ma_max_states) ^ " " ^
            (string_of_int ma_max_trans) ^ " " ^
            pdmu_stats#to_row_string
    end;;







 
exception Bad_input of pdmu_error list




module OrderedVariable =
    struct 
        type t = id
        let compare = compare
    end;;

module BindMap = Map.Make(OrderedVariable);;

module MemoFPKey = 
    struct
        type t = id * mu_formula * int * bool
        let compare = compare
    end;;

module MemoFP = Map.Make(MemoFPKey);;


(* (bindings, last automaton, has_last, lowest changed bindlevel) *)
type ma_info = int BindMap.t * PdsMuMa.ma * bool * int

(* if -1 is proposition level, -2 is, well, lower *)
let init_ma_info = (BindMap.empty, new PdsMuMa.ma, false, -2);;

let set_binding (z : id) (c : int) (bindings, old_ma, has_last, changed_c) = 
    (BindMap.add z c bindings, old_ma, has_last, changed_c);;

let get_binding (z : id) (bindings, _, _, _) = 
    let res = BindMap.find z bindings in
    res;;

let set_last_ma ma (bindings, _, _, changed_c) =
    (bindings, ma, true, changed_c);;

let get_last_ma (_, ma, _, _) = 
    ma;;

let get_has_last (_, _, b, _) = 
    b;;

let set_changed_c changed_c (bindings, old_ma, has_last, _) = 
    (bindings, old_ma, has_last, changed_c);;

let get_changed_c (_, _, _, changed_c) = 
    changed_c;;






(*************************************************)
(* Definitions for fast post* a la Schwoon
 *)

(* For maintaining a worklist of new ma transitions *)
module SimpleTran = 
    struct
        type t = PdsMuState.state * character * PdsMuState.state
        let compare = compare
        let to_string (q, a, q') = (state_to_string q) ^ " -" ^
                                   (character_to_string a) ^ "-> " ^
                                   (state_to_string q)
    end;;

module TranSet = Extset.Make(SimpleTran);;

(* epsmap(q) is set of states with e-tran *to* q *)
module OrderedState = 
    struct
        type t = PdsMuState.state
        let compare = compare
    end;;

module EpsilonMap = Map.Make(OrderedState);;


   



class mc_pds_mucalc (pdmu : pds_mucalc) =
    object (self)

        val proposition_level = 0
        val init_level = 1
        val normed_fmla = normalise_formula (pdmu#get_fmla)
        val reach_prop = Prop("reachable")
        val neg_reach_prop = NegProp("reachable")
        val mutable reach_inits = PdsMuState.StateSet.empty
        val mutable neg_reach_inits = PdsMuState.StateSet.empty
        val ss_qepsilon = PdsMuState.setofstatesets_from_list_list [[QEpsilon]]
        val ss_qstar = PdsMuState.setofstatesets_from_list_list [[QStar]]
        val ss_qfail = PdsMuState.setofstatesets_from_list_list [[QFail]]
        
        val pdmu = pdmu

        (* we're using a global and methods with side effects to prevent creating a new copy at every method call *)
        (* although we still have to create a shallow copy at each iteration of compute_fixed_point *)
        val mutable disable_hard_opt = false
        val mutable prod_union_opt = false
        val mutable gather_stats = false
        val mutable restrict_reachable = false

        (* eloise stuck is for the sake of a dummy value *)
        val mutable init_p = eloise_stuck

        (* for testing purposes only *)
        method get_pdmu = pdmu
        method get_normed_fmla = normed_fmla

        val mutable memo_fp = MemoFP.empty

        val mutable max_states = 0
        val mutable max_trans = 0
        val mutable stats_time = 0.0
        val mutable check_time = 0.0
        val mutable final_opt_time = 0.0

        method set_gather_stats v = 
            gather_stats <- v


        method private reset_memo_fp = 
            memo_fp <- MemoFP.empty

        method private get_memo_fp_ma z f c g = 
            try
                MemoFP.find (z, f, c, g) memo_fp
            with Not_found ->
                new PdsMuMa.ma

        method private set_memo_fp_ma z f c g ma = 
            memo_fp <- MemoFP.add (z, f, c, g) ma memo_fp


        method private reset_stats = 
            check_time <- 0.0;
            max_states <- 0;
            max_trans <- 0;
            stats_time <- 0.0;
            final_opt_time <- 0.0


        method private gather_stats ma = 
            if gather_stats then (
                let start_time = Sys.time () in
                let ns = ma#num_states in
                let nt = ma#num_trans in
                max_states <- max max_states ns;
                max_trans <- max max_trans nt;
                stats_time <- stats_time +. (Sys.time ()) -. start_time;
            )


        method get_stats (ma : PdsMuMa.ma) = 
            new mc_pdmu_stats ma#num_states
                              ma#num_trans
                              max_states
                              max_trans
                              stats_time
                              check_time
                              final_opt_time
                              pdmu#get_stats


        method set_disable_hard_opt ho = 
            disable_hard_opt <- ho
        method get_disable_hard_opt = disable_hard_opt

        method set_prod_union_opt o = 
            prod_union_opt <- o
        method get_prod_union_opt = prod_union_opt


        method set_restrict_reachable v = 
            restrict_reachable <- v

        method set_init_p p0 =
            init_p <- p0

        (* initial transitions should be restricted to reachable configs *)
        method private add_init_tran q a image ma = 
            if restrict_reachable (* and reach_inits not empty... *) then (
                let p = state_get_control q in
                let qp_reach = self#get_p_init p reach_inits in
                if not (characters_equal a cdefault) then (
                    let reach_image = ma#get_char_image qp_reach a in
                    let restrict_image = PdsMuState.setofstatesets_prod_union image reach_image in
                    ma#add_hyper_transition (PdsMuMa.make_ht q a restrict_image);
                ) else (
                    let reach_nondef = ma#get_non_defaults qp_reach in
                    let q_nondef = ma#get_non_defaults q in
                    let reach_only = CharacterSet.diff reach_nondef q_nondef in
                    CharacterSet.foreach reach_only (fun b ->
                        let reach_image = ma#get_char_image qp_reach b in
                        let restrict_image = PdsMuState.setofstatesets_prod_union image reach_image in
                        ma#add_hyper_transition (PdsMuMa.make_ht q b restrict_image);
                    );
                    let def_reach_image = ma#get_char_image qp_reach cdefault in
                    let def_restrict_image = PdsMuState.setofstatesets_prod_union image def_reach_image in
                    ma#add_hyper_transition (PdsMuMa.make_ht q cdefault def_restrict_image);
                )
            ) else (
                ma#add_hyper_transition (PdsMuMa.make_ht q a image);
            ) 

        (* internal transitions don't need to be restricted *)
        method private add_internal_tran q a image ma = 
            ma#add_hyper_transition (PdsMuMa.make_ht q a image);


        (* copied transitions don't need to be restricted *)
        method private add_copied_tran q a image ma = 
            ma#add_hyper_transition (PdsMuMa.make_ht q a image);




        method private assert_good_input =
            match self#checkable_errors with
              None -> ()
            | Errors(es) -> raise (Bad_input(es))
      


       method checkable_errors = 
            let game_rules = pdmu#get_pds#get_all_rules in
            let alphabet = pdmu#get_pds#get_alphabet in
            let controls = pdmu#get_pds#get_control_states in
            let fmla = pdmu#get_fmla in

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

            let noinit_errors = 
                if restrict_reachable && 
                   (not (ControlStateSet.mem init_p controls)) then
                    [NoInit]
                else [] in

            let bad_subformula_errors =
                let is_atomic f = 
                    match f with
                      True -> true
                    | False -> true
                    | Prop(_) -> true
                    | NegProp(_) -> true
                    | Var(_) -> true
                    | _ -> false in
                let rec catch_errors f = 
                    match f with
                      True -> [BadSubformula(True)]
                    | False -> [BadSubformula(False)]
                    | Prop(_) -> []
                    | NegProp(_) -> []
                    | Var(_) -> []
                    | Bin(f1,_,f2) -> List.append (catch_errors f1) (catch_errors f2)
                    | Un(_,f1) -> catch_errors f1
                    | ConstrainUn(_,_,f1) -> catch_errors f1 
                    | Mu(x,f1) ->  if is_atomic f1 then
                                       [BadSubformula(Mu(x,f1))]
                                   else
                                       catch_errors f1
                    | Nu(x,f1) ->  if is_atomic f1 then
                                       [BadSubformula(Nu(x,f1))]
                                   else
                                       catch_errors f1
                in
                catch_errors fmla in

            let bad_interesting_config_errors = 
                let do_config errs (p, w) = 
                    let w_chars = word_get_characters w in
                    let strays = CharacterSet.diff w_chars alphabet in
                    if CharacterSet.is_empty strays then
                        errs
                    else
                        (BadInterestingConfig((p, w), strays))::errs in
                List.fold_left do_config [] (pdmu#get_interesting_configs) in

            let unbound_var_errors = 
                let unbound = get_unbound_vars fmla in
                if not (IdSet.is_empty unbound) then
                    [UnboundVariables(unbound)]
                else
                    [] in
            let errors = List.concat [bottom_use_errors; 
                                      needs_bottom_errors;
                                      noinit_errors;
                                      bad_subformula_errors;
                                      bad_interesting_config_errors;
                                      unbound_var_errors] in
            if (errors = []) then None else (Errors(errors))


        (* q -a-> q* for all a, and q -#-> qe *)
        method private make_init_accept_all q ma =
            self#add_init_tran q sbot ss_qepsilon ma;
            self#add_init_tran q cdefault ss_qstar ma 

        (* q -a-> q* *)
        method private make_init_accept q a ma =
            if characters_equal a sbot then
                self#add_init_tran q sbot ss_qepsilon ma
            else
                self#add_init_tran q a ss_qstar ma
     
         (* q -a-> q* for all a, and q -#-> qe *)
        method private make_internal_accept_all q ma =
            self#add_internal_tran q sbot ss_qepsilon ma; 
            self#add_internal_tran q cdefault ss_qstar ma

        (* q -a-> q* *)
        method private make_internal_accept q a ma =
            if characters_equal a sbot then
                self#add_internal_tran q sbot ss_qepsilon ma
            else
                self#add_internal_tran q a ss_qstar ma
                

        method private init_ma ma = 
            let pds = pdmu#get_pds in
            let musubs = get_subformulas (normalise_formula (pdmu#get_fmla)) in
            let alphabet = pds#get_alphabet in
            let controls = pds#get_control_states in
            let all_heads = ControlStateSet.fold (fun p all_c -> 
                                CharacterSet.fold (fun a all_a ->
                                    HeadSet.add (p, a) all_a
                                ) alphabet all_c
                            ) controls HeadSet.empty in
            let init_qstar () = 
                CharacterSet.foreach alphabet (fun a -> 
                    if characters_equal a sbot then
                        self#add_internal_tran QStar sbot ss_qepsilon ma
                    else
                        self#add_internal_tran QStar a ss_qstar ma
                ) in           
            let init_tt_ff () = 
                ControlStateSet.foreach controls (fun p -> 
                    let qtt = self#make_init p tt_prop proposition_level in
                    let qff = self#make_init p ff_prop proposition_level in
                    ma#add_states (PdsMuState.StateSet.singleton qtt);
                    ma#add_states (PdsMuState.StateSet.singleton qff);
                    self#make_init_accept_all qtt ma;
                ) in
            let init_props () =
                PropSet.foreach (pdmu#get_props#get_props) (fun x -> 
                    let xfmla = Prop(x) in
                    let not_xfmla = NegProp(x) in
                    let sat_heads = (pdmu#get_props#get_heads x) in
                    let positive_occur = MuSet.mem xfmla musubs in
                    let negative_occur = MuSet.mem not_xfmla musubs in
                    if (positive_occur && negative_occur) then (
                        HeadSet.foreach all_heads (fun (p, a) -> 
                            if HeadSet.mem (p, a) sat_heads then (
                                let s = self#make_init p xfmla proposition_level in
                                self#make_init_accept s a ma
                            ) else (
                                let s = self#make_init p not_xfmla proposition_level in
                                self#make_init_accept s a ma
                            )
                        );
                    ) else if positive_occur then (
                        HeadSet.foreach sat_heads (fun (p, a) ->
                            let s = self#make_init p xfmla proposition_level in
                            self#make_init_accept s a ma
                        );
                    ) else if negative_occur then (
                        let unsat_heads = HeadSet.diff all_heads sat_heads in
                        HeadSet.foreach unsat_heads (fun (p, a) ->
                            let s = self#make_init p not_xfmla proposition_level in
                            self#make_init_accept s a ma
                        );
                    );
                    ControlStateSet.foreach (pdmu#get_pds#get_control_states) (fun p ->
                        if MuSet.mem xfmla musubs then (
                            let s = self#make_init p xfmla proposition_level in
                            ma#add_states (PdsMuState.StateSet.singleton s)
                        );
                        if MuSet.mem not_xfmla musubs then (
                            let s' = self#make_init p not_xfmla proposition_level in
                            ma#add_states (PdsMuState.StateSet.singleton s')
                        );
                    )
                ) in
            ma#add_states (PdsMuState.StateSet.from_list [QEpsilon;QStar]);
            init_qstar ();
            init_tt_ff ();
            init_props ();


        method construct_poststar = 
            Progress_out.pick_spinner ();
            Progress_out.print_string "Building backwards maps...";
            pdmu#get_pds#build_backwards_maps;
            Progress_out.erase_string "Building backwards maps...";
            Progress_out.print_string "Initialising...";
            self#reset_stats;
            self#assert_good_input;
            let ma = new PdsMuMa.ma in
            self#init_ma ma;
            self#reset_memo_fp;
            Progress_out.erase_string "Initialising...";
            Progress_out.print_string "Computing post* fast: ";
            let inits = self#compute_post_fast ma init_ma_info in
            let initsneg = self#compute_neg_post ma in
            Progress_out.erase_string "Computing post* fast: ";
            Progress_out.print_string "Done.";
            Progress_out.print_newline ();
            ma#set_inits (PdsMuState.StateSet.union inits initsneg);
            ma


        method construct_denotation =   
            Progress_out.pick_spinner ();
            Progress_out.print_string "Building backwards maps...";
            if restrict_reachable || (has_backwards_modalities normed_fmla) then
                pdmu#get_pds#build_backwards_maps;
            Progress_out.erase_string "Building backwards maps...";
            Progress_out.print_string "Initialising...";
            self#reset_stats;
            self#assert_good_input;
            let start_time = Sys.time () in
            let ma = new PdsMuMa.ma in
            if restrict_reachable then (
                Progress_out.erase_string "Initialising...";
                Progress_out.print_string "Computing post*: ";
                reach_inits <- self#compute_post_fast ma init_ma_info;
                neg_reach_inits <- self#compute_neg_post ma;
                Progress_out.erase_string "Computing post*: ";
                Progress_out.print_string "Initialising...";
            );
            self#init_ma ma;
            self#reset_memo_fp;
            Progress_out.erase_string "Initialising...";
            Progress_out.print_string "Computing denotation: ";
            let inits = self#dispatch normed_fmla init_level ma init_ma_info in
            ma#set_inits inits; 
            ma#set_finals (PdsMuState.StateSet.singleton QEpsilon);
            let done_time = Sys.time () in
            Progress_out.erase_string "Computing denotation: ";
            Progress_out.print_string "Optimising automaton...";
            ma#remove_unreachable;
            let opted_time = Sys.time () in
            check_time <- done_time -. start_time;
            final_opt_time <- opted_time -. done_time;
            Progress_out.erase_string "Optimising automaton...";
            Progress_out.print_string "Done.";
            Progress_out.print_newline ();
            ma


           




        method private dispatch_body f c ma ma_info = 
            let opt = ref true in
            let result = match f with
                  True -> failwith "tried to dispatch on fmla 'True'"
                | False -> failwith "tried to dispatch on fmla 'False'"
                | Prop(x) -> opt := false; self#compute_lit f c ma ma_info
                | NegProp(x) -> opt := false; self#compute_lit f c ma ma_info
                | Var(z) -> opt := false; self#compute_var f c ma ma_info
                | Bin(f1,And,f2) -> self#compute_and f1 f2 c ma ma_info
                | Bin(f1,Or,f2) -> self#compute_or f1 f2 c ma ma_info
                | Bin(f1,Implies,f2) -> failwith "Formula has implies: not positive normal form."
                | Un(Not,f1) -> failwith "Formula has negation: not positive normal form."
                | Un(Box,f1) -> self#compute_box f1 c ma ma_info
                | Un(Diamond,f1) -> self#compute_diamond f1 c ma ma_info
                | ConstrainUn(ConstrainBox, ids, f1) -> self#compute_constrain_box ids f1 c ma ma_info
                | ConstrainUn(ConstrainDiamond, ids, f1) -> self#compute_constrain_diamond ids f1 c ma ma_info
                | ConstrainUn(NegConstrainBox, ids, f1) -> self#compute_neg_constrain_box ids f1 c ma ma_info
                | ConstrainUn(NegConstrainDiamond, ids, f1) -> self#compute_neg_constrain_diamond ids f1 c ma ma_info
                | Un(BackBox,f1) -> self#compute_back_box f1 c ma ma_info
                | Un(BackDiamond,f1) -> self#compute_back_diamond f1 c ma ma_info
                | ConstrainUn(ConstrainBackBox, ids, f1) -> self#compute_constrain_back_box ids f1 c ma ma_info
                | ConstrainUn(ConstrainBackDiamond, ids, f1) -> self#compute_constrain_back_diamond ids f1 c ma ma_info
                | ConstrainUn(NegConstrainBackBox, ids, f1) -> self#compute_neg_constrain_back_box ids f1 c ma ma_info
                | ConstrainUn(NegConstrainBackDiamond, ids, f1) -> self#compute_neg_constrain_back_diamond ids f1 c ma ma_info
                | Mu(z,f1) -> self#compute_lfp z f1 c ma ma_info
                | Nu(z,f1) -> self#compute_gfp z f1 c ma ma_info in
            if !opt then
                self#optimise_automaton ma;
            result



        method dispatch f c ma ma_info = 
(*            print_string ("dispatch " ^ (mu_formula_to_string f));
            print_newline ();
*)            let result = self#dispatch_body f c ma ma_info in
            self#gather_stats ma;
(*            print_string ("done " ^ (mu_formula_to_string f));
            print_newline ();
*)            result

        method private get_changed_heads ma ma_old = 
            let alphabet = pdmu#get_pds#get_alphabet in
            let added = ma#less_than_heads ma_old alphabet in
            let removed = ma_old#less_than_heads ma alphabet in
            PdsMuMa.MaHeadSet.union added removed


        method private depends_on_change f ma_info =
            let change_c = get_changed_c ma_info in
            match f with
              True -> false
            | False -> false
            | Prop(x) -> (change_c <= proposition_level)
            | NegProp(x) -> (change_c <= proposition_level)
            | Var(z) -> (try 
                            (change_c <= get_binding z ma_info)
                         with Not_found ->
                             false)
            | Bin(f1,_,f2) -> (self#depends_on_change f1 ma_info || 
                               self#depends_on_change f2 ma_info)
            | Un(_,f1) -> self#depends_on_change f1 ma_info
            | ConstrainUn(_,_,f1) -> self#depends_on_change f1 ma_info
            | Mu(_,f1) -> self#depends_on_change f1 ma_info
            | Nu(_,f1) -> self#depends_on_change f1 ma_info

        (* from stateset s, get state corresponding to control state p *)
        (* assumes there is exactly one *)
        method private get_p_init p s = 
            let is_p q = 
                match q with
                    QVal(p', _, _) -> control_states_equal p p'
                | _ -> false in
            let singleton = PdsMuState.StateSet.filter is_p s in
            PdsMuState.StateSet.choose singleton

        (* get_inits retrieves the initials for a formula with respect to 
         * whether it's a variable or a proposition and the its binding level if
         * a var
         *)
        method private get_inits f c ma ma_info = 
            let pds = pdmu#get_pds in
            let controls = pds#get_control_states in
            let make_p_init p = 
                match f with
                  Prop(_) -> self#make_init p f proposition_level
                | NegProp(_) -> self#make_init p f proposition_level
                | Var(z) -> self#make_init p f (get_binding z ma_info)
                | _ -> self#make_init p f c in
            let do_control p inits =  
                PdsMuState.StateSet.add (make_p_init p) inits in
            ControlStateSet.fold do_control controls PdsMuState.StateSet.empty

        (* make_inits always returns states (p, f, c) for all p *)
        method private make_inits f c = 
            let pds = pdmu#get_pds in
            let controls = pds#get_control_states in
            let c_list = ControlStateSet.to_list controls in
            let make_p_init p = self#make_init p f c in
            let inits_list = List.map make_p_init c_list in
            PdsMuState.StateSet.from_list inits_list

        method private make_init p f c = 
            QVal(p, f, c)


        method private compute_lit f c ma ma_info =
            self#get_inits f c ma ma_info

        method private compute_var f c ma ma_info =
            self#get_inits f c ma ma_info 


        method private recursive_inherit_filter cutoff_c q = 
            match q with
              QStar -> false
            | QEpsilon -> false
            | QFail -> false
            | QVal(_, _, c) -> (c >= cutoff_c)
            | QInt(_, _, c, a) -> (c >= cutoff_c)

        method private can_inherit f res_inits ma_info =
            let ma_states = (get_last_ma ma_info)#get_states in
            (not (self#depends_on_change f ma_info)) &&
            (PdsMuState.StateSet.subset res_inits ma_states)



        method private compute_and f1 f2 c ma ma_info = 
            let pds = pdmu#get_pds in
            let controls = pds#get_control_states in
            let f = Bin(f1, And, f2) in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                let inits2 = self#dispatch f2 c ma ma_info in
                ControlStateSet.foreach controls (fun p ->
                    (* alternatively use copy_from in ma, but have to build image each time *)
                    let q = self#get_p_init p res_inits in 
                    let init1 = self#get_p_init p inits1 in
                    let init2 = self#get_p_init p inits2 in
                    let init_set = PdsMuState.StateSet.from_list [init1; init2] in
                    let q1chars = ma#get_non_defaults init1 in
                    let q2chars = ma#get_non_defaults init2 in
                    let q2only = CharacterSet.diff q2chars q1chars in
                    let do_char a = 
                        let image_set = ma#get_char_image_from_set init_set a prod_union_opt in
                        self#add_copied_tran q a image_set ma in
                    CharacterSet.foreach q1chars do_char;
                    CharacterSet.foreach q2only do_char;
                    do_char cdefault;
                );
            );
            res_inits
 


        method private compute_or f1 f2 c ma ma_info = 
            let pds = pdmu#get_pds in
            let controls = pds#get_control_states in
            let f = Bin(f1, Or, f2) in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                let inits2 = self#dispatch f2 c ma ma_info in
                ControlStateSet.foreach controls (fun p ->
                    (* alternatively replace with copy_from call in ma
                     * but this means building an image for each pair *)
                    let q = self#get_p_init p res_inits in 
                    let init1 = self#get_p_init p inits1 in
                    let init2 = self#get_p_init p inits2 in
                    let q1chars = ma#get_non_defaults init1 in
                    let q2chars = ma#get_non_defaults init2 in
                    let q2only = CharacterSet.diff q2chars q1chars in
                    let do_char a = 
                        let image1 = ma#get_char_image init1 a in
                        let image2 = ma#get_char_image init2 a in
                        self#add_copied_tran q a image1 ma;
                        self#add_copied_tran q a image2 ma in
                    CharacterSet.foreach q1chars do_char;
                    CharacterSet.foreach q2only do_char;
                    do_char cdefault;
                );
            );
            res_inits



        method private get_sat_heads ids = 
            List.fold_left (fun all x -> 
                                HeadSet.union all (pdmu#get_props#get_heads x))
                            HeadSet.empty
                            ids 


        method private compute_diamond f1 c ma ma_info = 
            let pds = pdmu#get_pds in
            let f = Un(Diamond, f1) in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                let do_rule r = 
                    let (p, a, _, _) = rule_tuple r in
                    let q = self#make_init p f c in
                    let image = self#rule_image r inits1 ma in
                    self#add_init_tran q a image ma in
                pds#rules_iter do_rule;
            );
            res_inits


        method private compute_constrain_diamond ids f1 c ma ma_info = 
            let pds = pdmu#get_pds in
            let f = ConstrainUn(ConstrainDiamond, ids, f1) in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                let do_rule r = 
                    let (p, a, _, _) = rule_tuple r in
                    let q = self#make_init p f c in
                    let image = self#rule_image r inits1 ma in
                    self#add_init_tran q a image ma in
                let sat_heads = self#get_sat_heads ids in 
                pds#rules_heads_iter sat_heads do_rule;
            );
            res_inits


        method private compute_neg_constrain_diamond ids f1 c ma ma_info =
            let pds = pdmu#get_pds in
            let f = ConstrainUn(NegConstrainDiamond, ids, f1) in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                let sat_heads = self#get_sat_heads ids in
                let do_rule r = 
                    let (p, a, _, _) = rule_tuple r in
                    if not (HeadSet.mem (p, a) sat_heads) then (
                        let q = self#make_init p f c in
                        let image = self#rule_image r inits1 ma in
                        self#add_init_tran q a image ma 
                    ) in
                pds#rules_iter do_rule;
            );
            res_inits


        method private compute_box f1 c ma ma_info = 
            let filter _ = true in
            let f = Un(Box, f1) in
            self#compute_box_filter f f1 c ma ma_info filter


        method private compute_constrain_box ids f1 c ma ma_info = 
            let f = ConstrainUn(ConstrainBox, ids, f1) in
            let sat_heads = self#get_sat_heads ids in 
            let sat_heads_filter head = HeadSet.mem head sat_heads in
            self#compute_box_filter f f1 c ma ma_info sat_heads_filter


        method private compute_neg_constrain_box ids f1 c ma ma_info = 
            let f = ConstrainUn(NegConstrainBox, ids, f1) in
            let sat_heads = self#get_sat_heads ids in 
            let sat_heads_filter head = not (HeadSet.mem head sat_heads) in
            self#compute_box_filter f f1 c ma ma_info sat_heads_filter



        method private compute_box_filter f f1 c ma ma_info filter = 
            let pds = pdmu#get_pds in
            let alphabet = pds#get_alphabet in
            let controls = pds#get_control_states in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                ControlStateSet.foreach controls (fun p ->
                    let q = self#make_init p f c in 
                    CharacterSet.foreach alphabet (fun a ->
                        let next_set = pds#get_next p a in
                        if (not (filter (p, a))) || (NextSet.is_empty next_set) then (
                            let image = if characters_equal a sbot then
                                            ss_qepsilon
                                        else 
                                            ss_qstar in
                            self#add_init_tran q a image ma;
                        ) else (
                            let init_image = PdsMuState.SetOfStateSets.singleton PdsMuState.StateSet.empty in
                            let do_next (p', w) image =
                                let rule_image = self#state_word_image p' w inits1 ma in
                                let new_set = PdsMuState.setofstatesets_prod_union rule_image image in
                                if not prod_union_opt then
                                    new_set 
                                else
                                    PdsMuState.optimise_hyper_state_set new_set in
                            let image = NextSet.fold do_next next_set init_image in
                            self#add_init_tran q a image ma
                        )
                    )
                );
            );
            res_inits






        method private compute_back_box f1 c ma ma_info = 
            let filter _ = true in
            let f = Un(BackBox, f1) in
            self#compute_back_box_filter f f1 c ma ma_info filter


        method private compute_constrain_back_box ids f1 c ma ma_info = 
            let f = ConstrainUn(ConstrainBackBox, ids, f1) in
            let sat_heads = self#get_sat_heads ids in 
            let sat_heads_filter head = HeadSet.mem head sat_heads in
            self#compute_back_box_filter f f1 c ma ma_info sat_heads_filter


        method private compute_neg_constrain_back_box ids f1 c ma ma_info = 
            let f = ConstrainUn(NegConstrainBackBox, ids, f1) in
            let sat_heads = self#get_sat_heads ids in 
            let sat_heads_filter head = not (HeadSet.mem head sat_heads) in
            self#compute_back_box_filter f f1 c ma ma_info sat_heads_filter





        method private compute_back_box_filter f f1 c ma ma_info filter = 
            Progress_out.print_string "~[]";
            (* back box when restricted is ~[](f or not reachable) *)
            let get_state_char_image_bb p a inits = 
                if restrict_reachable then (
                    let basic_image = self#state_char_image p a inits ma in
                    let not_reach_image = self#state_char_image p a neg_reach_inits ma in
                    let head_image = PdsMuState.SetOfStateSets.union basic_image not_reach_image in
                    PdsMuState.optimise_hyper_state_set head_image 
                ) else (
                    self#state_char_image p a inits ma 
                ) in

(*            let get_state_word_image_bb p w inits = 
                if restrict_reachable then (
                    let basic_image = self#state_word_image p w inits ma in
                    let not_reach_image = self#state_word_image p w neg_reach_inits ma in
                    PdsMuState.SetOfStateSets.union basic_image not_reach_image
                ) else (
                    self#state_word_image p w inits ma 
                ) in
*)
            let pds = pdmu#get_pds in
            let alphabet = pds#get_alphabet in
            let controls = pds#get_control_states in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
        
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in
                ControlStateSet.foreach controls (fun p ->
                    let q = self#make_init p f c in 
                    let pop_heads = pds#get_pre_pop_filter p filter in
                    let empty_image = PdsMuState.SetOfStateSets.singleton PdsMuState.StateSet.empty in 
                    let initted = ref false in
                    let qpop = ref (QVal(p, Prop("temp£$$£$%"), proposition_level)) in
                    let qpop2 = ref (QVal(p, Prop("temp2^$%^$%&"), proposition_level)) in
                    let qpoptmp = ref (QVal(p, Prop("temp^90729*&^("), proposition_level)) in
                    ma#add_states (PdsMuState.StateSet.singleton !qpop);
                    ma#add_states (PdsMuState.StateSet.singleton !qpop2);

                    let do_pop_head (p', a') = 
                        let head_image = get_state_char_image_bb p' a' inits1 in
                         let s_qpop = PdsMuState.StateSet.singleton !qpop in
                         let ss_qpop = PdsMuState.SetOfStateSets.singleton s_qpop in
                         if !initted then (
                            let copy_image = PdsMuState.setofstatesets_prod_union head_image 
                                                                                  ss_qpop in
                            (*print_string (PdsMuState.SetOfStateSets.to_string copy_image);
                            print_newline ();*)
                            ma#copy_from !qpop2 copy_image prod_union_opt;
                            ma#delete_state !qpop;
                            ma#add_states s_qpop;
                            qpoptmp := !qpop;
                            qpop := !qpop2;
                            qpop2 := !qpoptmp
                         ) else (
                            ma#copy_from !qpop head_image prod_union_opt;
                            initted := true;
                         ) in
                    HeadSet.iter do_pop_head pop_heads;
 
                    CharacterSet.foreach alphabet (fun a ->
                        let rew_heads = pds#get_pre_rew_filter p a filter in
                        let has_pre_push = pds#has_pre_push_filter p a filter in
                        if HeadSet.is_empty pop_heads &&
                           HeadSet.is_empty rew_heads &&
                           (not has_pre_push) then (
                            if characters_equal a sbot then
                                self#add_init_tran q a ss_qepsilon ma
                            else 
                                self#add_init_tran q a ss_qstar ma
                        ) else (
                            let qint = QInt(p, f, c, a) in
                            if has_pre_push then
                                ma#add_states (PdsMuState.StateSet.singleton qint);
                            (* Transitions from (p, f, c) *)
                            let init_image = if has_pre_push then 
                                                 PdsMuState.SetOfStateSets.singleton (PdsMuState.StateSet.singleton qint)    
                                             else 
                                                 empty_image in


                            let pop_image = ma#get_char_image !qpop a in
                            let pop_image = PdsMuState.setofstatesets_prod_union pop_image init_image in
                            
                            let do_rew_head (p', a') image = 
                                let head_image = get_state_char_image_bb p' a' inits1 in
                                PdsMuState.setofstatesets_prod_union_opt head_image 
                                                                         image 
                                                                         prod_union_opt in
                            let rew_image = HeadSet.fold do_rew_head rew_heads pop_image in
                            self#add_init_tran q a rew_image ma;

                            (* add transitions from (p, f, c, a) *)
                            if has_pre_push then (
                                let pre_push_chars = pds#pre_push_chars_filter p a filter in
                                (* assume most accept blindly *)
                                self#add_internal_tran qint cdefault ss_qstar ma;
                                if not (CharacterSet.mem sbot pre_push_chars) then 
                                    self#add_internal_tran qint sbot ss_qepsilon ma;
                                CharacterSet.foreach pre_push_chars (fun b ->
                                    let push_heads = pds#get_pre_push_filter p a b filter in
                                    let do_push_head (p', a') image =
                                        let head_image = get_state_char_image_bb p' a' inits1 in
                                        PdsMuState.setofstatesets_prod_union_opt head_image image prod_union_opt in
                                    let push_image = HeadSet.fold do_push_head push_heads empty_image in
                                    if PdsMuState.SetOfStateSets.is_empty push_image then 
                                        self#add_internal_tran qint b ss_qfail ma
                                    else
                                        self#add_internal_tran qint b push_image ma
                                );
                            )
                        )
                    );
                    ma#delete_state !qpop;
                    ma#delete_state !qpop2;
                )
            );
            Progress_out.erase_string "~[]";
            res_inits


        method private compute_back_diamond f1 c ma ma_info = 
            let f = Un(BackDiamond, f1) in
            let filter _ = true in
            self#compute_back_diamond_filter f f1 c ma ma_info filter




        method private compute_constrain_back_diamond ids f1 c ma ma_info = 
            let f = ConstrainUn(ConstrainBackDiamond, ids, f1) in
            let sat_heads = self#get_sat_heads ids in 
            let sat_heads_filter head = HeadSet.mem head sat_heads in
            self#compute_back_diamond_filter f f1 c ma ma_info sat_heads_filter



        method private compute_neg_constrain_back_diamond ids f1 c ma ma_info = 
            let f = ConstrainUn(NegConstrainBackDiamond, ids, f1) in
            let sat_heads = self#get_sat_heads ids in 
            let sat_heads_filter head = not (HeadSet.mem head sat_heads) in
            self#compute_back_diamond_filter f f1 c ma ma_info sat_heads_filter


        method private compute_back_diamond_filter f f1 c ma ma_info filter = 
            Progress_out.print_string "~<>";
            let pds = pdmu#get_pds in
            let res_inits = self#make_inits f c in
            ma#add_states res_inits;
            if (self#can_inherit f res_inits ma_info) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
            ) else (
                let inits1 = self#dispatch f1 c ma ma_info in

                let do_pop p a p' = 
                    let q = self#make_init p' f c in
                    let a_image = self#state_char_image p a res_inits ma in
                    ma#copy_from q a_image prod_union_opt in

                let do_pops p' heads = 
                    HeadSet.foreach heads (fun (p, a) -> 
                        do_pop p a p'
                    ) in

                let do_rew p a p' a' = 
                    let q = self#make_init p' f c in
                    let image = self#state_char_image p a inits1 ma in
                    self#add_init_tran q a' image ma in
                   
                let do_rews p' a' heads = 
                    HeadSet.foreach heads (fun (p, a) ->
                        do_rew p a p' a'
                    ) in

                let do_push p a p' a' a'' = 
                    let q' = self#make_init p' f c in
                    let qint = QInt(p, f, c, a') in
                    let set_qint = PdsMuState.StateSet.singleton (QInt(p', f, c, a')) in
                    let set_set_qint = PdsMuState.SetOfStateSets.singleton set_qint in
                    ma#add_states set_qint;
                    self#add_init_tran q' a' set_set_qint ma;
                    let image = self#state_char_image p a inits1 ma in
                    self#add_internal_tran qint a'' image ma in

                let do_pushes p a b heads = 
                    HeadSet.foreach heads (fun (p', a') -> 
                        do_push p' a' p a b
                    ) in


                pds#pre_pop_iter_filter do_pops filter;
                pds#pre_rew_iter_filter do_rews filter;
                pds#pre_push_iter_filter do_pushes filter;
(*
                if not (get_has_last ma_info) then (
                    pds#pre_pop_iter_filter do_pop filter;
                    pds#pre_rew_iter_filter do_rew filter;
                    pds#pre_push_iter_filter do_push filter;
                ) else (
                    let ma_old = get_last_ma ma_info in
                    let changed_heads = self#get_changed_heads ma ma_old in
                    (* Not sure what to do with default chars, so do it the old way *)
                    if PdsMuMa.MaHeadSet.exists (fun (_, a) -> characters_equal c cdefault)
                                                changed_heads then (
                        pds#pre_pop_iter_filter do_pop filter;
                        pds#pre_rew_iter_filter do_rew filter;
                        pds#pre_push_iter_filter do_push filter;
                    ) else (
                        ma#inherit_from res_inits ma_old;
                        PdsMuMa.MaHeadSet.foreach changed_heads (fun (q, a) ->
                            ma#remove_head q a;
                        );

                        PdsMuMa.MaHeadSet.foreach changed_heads (fun (q, a) ->

                        );
                    )
*)
            );
            Progress_out.erase_string "~<>";
            res_inits




        method private compute_lfp z f1 c ma ma_info = 
            self#compute_fixed_point z f1 c false ma ma_info


        method private compute_gfp z f1 c ma ma_info = 
            self#compute_fixed_point z f1 c true ma ma_info


        method project_rename_and_delete rename_inits new_inits m ma =
            let project_state q = 
                match q with
                  QVal(p, f, c) -> if c < m + 1 then
                                       q
                                   else if (PdsMuState.StateSet.mem q rename_inits) then
                                       self#get_p_init p new_inits
                                   else
                                       QVal(p, f, m)
                | QInt(p, f, c, a) -> if c < m + 1 then
                                          q
                                      else
                                          QInt(p, f, m, a)
                | _ -> q in
            let project_set_of_sets ss = 
                let do_set s = PdsMuState.StateSet.map project_state s in
                PdsMuState.SetOfStateSets.map do_set ss in
            let project_tran t =
                let (q, a, ss) = PdsMuMa.ht_tuple t in
                let new_ss = project_set_of_sets ss in
                let new_ss = if prod_union_opt then
                                 PdsMuState.optimise_hyper_state_set new_ss 
                             else
                                 new_ss in
                PdsMuMa.make_ht (project_state q) a new_ss in
            let do_tran t =
                match (PdsMuMa.ht_q t) with
                  QVal(q, f, c) -> if c = m then
                                      (`Remove)
                                   else if c = m + 1 then 
                                      (`AddTran(project_tran t))
                                   else
                                      (`Unchanged)
                | QInt(q, f, c, a) -> if c = m then
                                          (`Remove)
                                      else if c = m + 1 then 
                                          (`AddTran(project_tran t))
                                      else
                                          (`Unchanged)
                | _ -> (`Unchanged) in
            let q_filter q = 
                match q with
                  QVal(_, _, c) -> if c = m then
                                       (`Remove)
                                   else if c = m + 1 then
                                       (`IterRemove)
                                   else
                                       (`Unchanged) 
                | QInt(_, _, c, _) -> if c = m then
                                          (`Remove)
                                      else if c = m + 1 then
                                          (`IterRemove)
                                      else
                                          (`Unchanged) 
                | _ -> (`Unchanged) in
            ma#self_map_with_state_filter q_filter do_tran;
            ma#self_map_states (fun s -> (`State (project_state s)));
            if not disable_hard_opt then
                self#optimise_automaton ma;


        method private add_all_trans_init inits ma =
            PdsMuState.StateSet.foreach inits (fun q ->
                self#make_init_accept_all q ma
            )

        method private add_all_trans_internal inits ma =
            PdsMuState.StateSet.foreach inits (fun q ->
                self#make_internal_accept_all q ma
            )



        (* See compute_fixed_point *)
        method private drop_level c ma = 
            let project_state q = 
                match q with
                  QVal(p, f, c') -> if c' < c + 1 then
                                       q
                                   else
                                       QVal(p, f, c)
                | QInt(p, f, c', a) -> if c' < c + 1 then
                                           q
                                       else
                                           QInt(p, f, c, a)
                | _ -> q in
            let project_set_of_sets ss = 
                let do_set s = PdsMuState.StateSet.map project_state s in
                PdsMuState.SetOfStateSets.map do_set ss in
            let project_tran t =
                let (q, a, ss) = PdsMuMa.ht_tuple t in
                PdsMuMa.make_ht (project_state q) a (project_set_of_sets ss) in
            let do_tran t = (*(`NewTran(project_tran t)) in*)
                match (PdsMuMa.ht_q t) with
                  QVal(q, f, c') -> if c' >= c then
                                        (`NewTran(project_tran t))
                                    else 
                                        (`Unchanged)
                | QInt(q, f, c', a) -> if c' >= c then
                                           (`NewTran(project_tran t))
                                       else 
                                           (`Unchanged)
                | _ -> (`Unchanged) in
            ma#self_map do_tran;
            ma#self_map_states (fun s -> (`State (project_state s)))

        (* Note we use c+1 and c+2 here for layers.  This is because we're not recursively 
         * building automata and combining, but building a single automaton
         * shared between branches.  Hence, we don't want to interfere with
         * parallel branches working on level c, so we drop down to c+1
         * during computation and map back up to c when done.
         *
         * If the algorithm were to actually run in parallel, there'd have to be 
         * better separation to avoid projection deleting parallel threads'
         * states also of level c/c+1/c+2.
         *)
        method private compute_fixed_point z f1 c greatest ma ma_info =
            let f = if greatest then
                        Nu(z, f1) 
                    else
                        Mu(z, f1) in
            let unbound_vars = get_unbound_vars f in
            let res_inits = self#make_inits (Var(z)) c in
            let ma_states = (get_last_ma ma_info)#get_states in
            if (IdSet.is_empty unbound_vars) && 
               (PdsMuState.StateSet.subset res_inits ma_states) then (
                ma#recursive_inherit_from res_inits 
                                       (self#recursive_inherit_filter c)
                                       (get_last_ma ma_info);
                res_inits
            ) else (
                let pds = pdmu#get_pds in
                let alphabet = pds#get_alphabet in
                let spinner = new Progress_out.spinner in
                spinner#start_spinner;
                let ma_info = ref (set_binding z (c+1) ma_info) in
                ma_info := set_last_ma (self#get_memo_fp_ma z f1 c greatest) !ma_info;
                let res_inits = self#make_inits (Var(z)) (c+1) in
                ma#add_states res_inits;
                if greatest then 
                    self#add_all_trans_internal res_inits ma;
                let fixed = ref false in 
                while not !fixed do
                    let clone_filter q = 
                        match q with
                          QStar -> false
                        | QEpsilon -> false
                        | QFail -> false
                        | QVal(_, fq, c') -> (c' >= c+1)
                        | QInt(_, _, c', _) -> (c' >= c+1) in
                    let old_ma = ma#selective_clone clone_filter in

                    let next_inits = self#dispatch f1 (c+2) ma !ma_info in

                    (* Save value for next computation *)
                    ma_info := set_changed_c (c+1) !ma_info;
                    let clone_filter q = 
                        match q with
                          QStar -> false
                        | QEpsilon -> false
                        | QFail -> false
                        | QVal(_, fq, c') -> (c' > c+1) 
                        | QInt(_, fq, c', _) -> (c' > c+1) in
                    ma_info := set_last_ma (ma#selective_clone clone_filter) !ma_info;

                    self#project_rename_and_delete next_inits res_inits (c+1) ma;

                    if greatest then (
                        let res = old_ma#less_than ma alphabet in 
                        fixed := res
                    ) else (
                        let res = ma#less_than old_ma alphabet in
                        fixed := res
                    );
                    spinner#update_spinner;
                done;
                self#set_memo_fp_ma z f1 c greatest (get_last_ma !ma_info);
                self#drop_level c ma;
                spinner#clear_spinner;
                self#make_inits (Var(z)) c 
            )

        method private compute_post_fast ma ma_info = 
            let worklist = ref TranSet.empty in 
            let eps_map = ref EpsilonMap.empty in

            let add_tran q a q' = 
                let existed = ma#add_hyper_transition_and_test (PdsMuMa.make_ht_list q a [[q']]) in
                if (not existed) then
                    worklist := TranSet.add (q, a, q') !worklist in

            let add_eps q q' = 
                if EpsilonMap.mem q' !eps_map then (
                    let q'_set = EpsilonMap.find q' !eps_map in
                    let new_q'_set = PdsMuState.StateSet.add q q'_set in
                    eps_map := EpsilonMap.add q' new_q'_set !eps_map;
                ) else (
                    let q'_set = PdsMuState.StateSet.singleton q in
                    eps_map := EpsilonMap.add q' q'_set !eps_map;
                ) in

            let eps q = 
                if EpsilonMap.mem q !eps_map then 
                    EpsilonMap.find q !eps_map 
                else 
                    PdsMuState.StateSet.empty in

            let pds = pdmu#get_pds in
            let filter _ = true in

            (* initialise *)
            let f = reach_prop in
            let res_inits = self#make_inits f proposition_level in
            let q_init = self#get_p_init init_p res_inits in
            ma#add_states res_inits;
            add_tran q_init sbot QEpsilon;

            let spinner = new Progress_out.spinner in
            spinner#start_spinner;

            (* First preprocess all push rules *)
            let do_push_init p a b heads = 
                let q = self#get_p_init p res_inits in
                let qint = QInt(p, f, proposition_level, a) in
                if not (HeadSet.is_empty heads) then (
                    ma#add_states (PdsMuState.StateSet.singleton qint);
                    add_tran q a qint;
                ) in
            pds#pre_push_iter_filter do_push_init filter;


            (* Main body *)
            let last_spin_time = ref (Sys.time ()) in
            while (not (TranSet.is_empty !worklist)) do
                if (Sys.time ()) > (!last_spin_time +. 2.0) then (
                    spinner#update_spinner;
                    last_spin_time := Sys.time ();
                );


                (* Note differs from paper which is (p, a, q) -- careful with 's!!! *)
                let (qp, a, q) = TranSet.choose !worklist in
                let p = state_get_control qp in

                worklist := TranSet.remove (qp, a, q) !worklist;

                let do_pop_rule p' = 
                    let qeps = eps q in
                    let qp' = self#get_p_init p' res_inits in
                    if not (PdsMuState.StateSet.mem qp' qeps) then (
                        add_eps qp' q;
                        
                        let do_tran t = 
                            let (_, b, ss) = PdsMuMa.ht_tuple t in
                            PdsMuState.SetOfStateSets.foreach ss (fun s ->
                                (* All state sets will be singletons *)
                                let q' = PdsMuState.StateSet.choose s in 
                                add_tran qp' b q';
                            ) in
                        ma#self_iter_by_state q do_tran;
                    ) in

                let do_rew_rule p' b = 
                    let qp' = self#get_p_init p' res_inits in
                    add_tran qp' b q in

                let do_push_rule p' b c = 
                    let qint = QInt(p', f, proposition_level, b) in
                    ma#add_hyper_transition (PdsMuMa.make_ht_list qint c [[q]]);
                    let qint_eps = eps qint in
                    PdsMuState.StateSet.foreach qint_eps (fun qnext ->
                        add_tran qnext c q;
                    ) in


                let do_rule r = 
                    let (_, _, p', w) = rule_tuple r in
                    let w_list = word_to_list w in
                    let w_len = List.length w_list in
                    match w_len with
                      0 -> do_pop_rule p'
                    | 1 -> do_rew_rule p' (List.hd w_list)
                    | 2 -> do_push_rule p' (List.hd w_list) (List.hd (List.tl w_list))
                    | _ -> failwith ("pds#build_backwards_maps failed on rule " ^ 
                                     (rule_to_string r) ^ 
                                     " -- rule pushes too many characters (max = 2).") in

                let pa_headset = HeadSet.singleton (p, a) in
                pds#rules_heads_iter pa_headset do_rule
            done;
            spinner#clear_spinner;
            res_inits





        method private compute_post ma ma_info = 
            let pds = pdmu#get_pds in
            (* initialise *)
            let f = reach_prop in
            let res_inits = self#make_inits f proposition_level in
            let q_init = self#get_p_init init_p res_inits in
            ma#add_states res_inits;
            ma#add_hyper_transition (PdsMuMa.make_ht q_init sbot ss_qepsilon);

            (* post* *)
            let spinner = new Progress_out.spinner in
            spinner#start_spinner;
            let filter _ = true in
            let changed = ref true in 
            while !changed do
                changed := false;

                let do_pop p heads = 
                    let q = self#get_p_init p res_inits in
                    HeadSet.foreach heads (fun (p', a') -> 
                        let a'_image = self#state_char_image p' a' res_inits ma in
                        let existed = ma#copy_from_and_test q a'_image prod_union_opt in
                        changed := !changed || (not existed)
                    ) in

                let do_rew p a heads = 
                    let q = self#get_p_init p res_inits in
                    HeadSet.foreach heads (fun (p', a') ->
                        let image = self#state_char_image p' a' res_inits ma in
                        let existed = ma#add_hyper_transition_and_test (PdsMuMa.make_ht q a image) in
                        changed := !changed || (not existed);
                    ) in

                let do_push p a b heads = 
                    let q = self#get_p_init p res_inits in
                    let qint = QInt(p, f, proposition_level, a) in
                    if not (HeadSet.is_empty heads) then (
                        ma#add_states (PdsMuState.StateSet.singleton qint);
                        let new_tran = PdsMuMa.make_ht_list q a [[qint]] in
                        let existed = ma#add_hyper_transition_and_test new_tran in
                        changed := !changed || (not existed);
                    );
                    HeadSet.foreach heads (fun (p', a') -> 
                        let image = self#state_char_image p' a' res_inits ma in
                        let existed = ma#add_hyper_transition_and_test (PdsMuMa.make_ht qint b image) in
                        changed := !changed || (not existed);
                    ) in

                pds#pre_pop_iter_filter do_pop filter;
                pds#pre_rew_iter_filter do_rew filter;
                pds#pre_push_iter_filter do_push filter;
                spinner#update_spinner;
            done;
            spinner#clear_spinner;
            res_inits


        (* assumes post already computed and only post star states exist *)
        method private compute_neg_post ma = 
            (* initialise *)
            let ma_states = ma#get_states in
            let f = neg_reach_prop in
            let res_inits = self#make_inits f proposition_level in
            ma#add_states res_inits;

            let rename_state q = 
                match q with
                  QStar -> QStar
                | QEpsilon -> QEpsilon
                | QFail -> QFail
                | QVal(p, _, _) -> QVal(p, f, proposition_level)
                | QInt(p, _, _, a) -> QInt(p, f, proposition_level, a) in

            let rename_states qset = 
                let do_q q s = 
                    PdsMuState.StateSet.add (rename_state q) s in
                PdsMuState.StateSet.fold do_q qset PdsMuState.StateSet.empty in

            let rename_image ss = 
                let do_s s ss = 
                    PdsMuState.SetOfStateSets.add (rename_states s) ss in
                PdsMuState.SetOfStateSets.fold do_s ss PdsMuState.SetOfStateSets.empty in

            let is_reach q = 
                match q with
                  QVal(_, f, _) -> (compare f reach_prop) = 0
                | QInt(_, f, _, _) -> (compare f reach_prop) = 0
                | _ -> false in

            let spinner = new Progress_out.spinner in
            spinner#start_spinner;

            (* Main body *)
            let last_spin_time = ref (Sys.time ()) in
            PdsMuState.StateSet.foreach ma_states (fun q ->
                if (Sys.time ()) > (!last_spin_time +. 2.0) then (
                    spinner#update_spinner;
                    last_spin_time := Sys.time ();
                );
                if is_reach q then (
                    let non_defs = ma#get_non_defaults q in
                    let qneg = rename_state q in
                    CharacterSet.foreach non_defs (fun a ->
                        (* for post* all image sets will be singletons 
                         * and there are no default transitions *)
                        if not (characters_equal a sbot) then (
                            let image = rename_image (ma#get_char_image q a) in
                            let conj_image = 
                                PdsMuState.SetOfStateSets.fold (PdsMuState.StateSet.union) 
                                                               image
                                                               PdsMuState.StateSet.empty in
                            if PdsMuState.StateSet.mem QStar conj_image then
                                ma#add_hyper_transition (PdsMuMa.make_ht qneg a ss_qfail)
                            else
                                let ss_conj = PdsMuState.SetOfStateSets.singleton conj_image in
                                ma#add_hyper_transition (PdsMuMa.make_ht qneg a ss_conj);
                        ) 
                    );
                    (* if we accept sbot, reject, and vice versa *)
                    if CharacterSet.mem sbot non_defs then
                        ma#add_hyper_transition (PdsMuMa.make_ht qneg sbot ss_qfail)
                    else
                        ma#add_hyper_transition (PdsMuMa.make_ht qneg sbot ss_qepsilon);

                    (* since there are no default trans, we can deal with all stuck
                     * (i.e q, a with no q -a-> Q) by a default accept (and
                     * we've dealt with sbot above)
                     *)
                    ma#add_hyper_transition (PdsMuMa.make_ht qneg cdefault ss_qstar) 
                )
            );
            spinner#clear_spinner;
            res_inits




        method private state_char_image p a inits ma = 
            let init = self#get_p_init p inits in
            ma#get_char_image init a 

        method private state_word_image p w inits ma = 
            let init = self#get_p_init p inits in
            ma#get_word_image init w prod_union_opt

        method private rule_image r inits ma = 
            let (_, _, p', w) = rule_tuple r in
            self#state_word_image p' w inits ma


         method optimise_automaton ma = 
            if not disable_hard_opt then (
                let tran_opt t = 
                    let (q, a, ss) = PdsMuMa.ht_tuple t in
                    (`NewTran(PdsMuMa.make_ht q a (PdsMuState.optimise_hyper_state_set ss))) in
                ma#self_map tran_opt
            )


       method input_error_to_string e =
            match e with
              BadBotUse(rules) -> 
                List.fold_left (fun s r -> s ^ (rule_to_string r) ^ "\n") 
                               "Rules using stack bottom incorrectly:\n\n"
                               rules
            | NeedsBot -> "Pushdown system does not have the bottom of stack symbol"
            | NoInit -> "Initial control " ^ (control_state_to_string init_p) ^ " does not exist."
            | BadSubformula(f) -> ("Subformula not supported, please rewrite: " ^ (mu_formula_to_string f))
            | BadInterestingConfig(c, chars) -> ("Interesting config '" ^ (config_to_string c) ^ "' " ^
                                                 "has characters " ^ (CharacterSet.to_string chars) ^ " " ^
                                                 "that do not appear in the PDS.")
            | UnboundVariables(ids) -> ("Variables " ^ (IdSet.to_string ids) ^ " need to be bound in formula.")
                                                 



        method input_error_list_to_string es =
            List.fold_left (fun s e -> s ^ (self#input_error_to_string e) ^ "\n")
                           ""
                           es

             
           
        method get_config_results (ma : PdsMuMa.ma) = 
            let ma_inits = ma#get_inits in
            let do_config res (p, w) = 
                let q = self#get_p_init p ma_inits in
                let truth = ma#accepts q w in
                ((p, w), truth)::res in
            let res = List.fold_left do_config [] (pdmu#get_interesting_configs) in
            new interesting_config_results res

    end;;





