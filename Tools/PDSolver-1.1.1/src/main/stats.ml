
open Mc_pds_mucalc;;
open Mc_parity;;
open Pds;;
open Mucalc;;
open Pds_mucalc;;
open Parity_game;;
open Big_int;;

type result_type = PdmuStats of mc_pdmu_stats |
                   PdgStats of mc_parity_stats |
                   TimedOut |
                   OutOfMemory |
                   Error of string;;


let fail_to_string f = 
    match f with
      TimedOut -> "timed out"
    | OutOfMemory -> "out of memory"
    | Error(s) -> "error (" ^ s ^ ")"
    | _ -> "didn't fail...?";;

(* Note: for now, we're making the informal restriction that all datum are
 *
 *               n * pdmu_stats * pdg_stats
 *)
type datum = int * result_type * result_type;;

let percentage_change_float a b = 
    if a < 0.0 || b < 0.0 then
        (-1.0)
    else
        (a -. b) /. b *. 100.0;;

let percentage_change_int a b = 
    if a < 0 || b < 0 then 
        (-1.0)
    else
        ((float_of_int a) -. (float_of_int b)) /. (float_of_int b) *. 100.0;;
    

class stats_compare (smu : mc_pdmu_stats) (spdg : mc_parity_stats) =  
    object
        val time_diff = percentage_change_float spdg#get_check_time smu#get_check_time
        val max_states_diff = percentage_change_int spdg#get_ma_max_states smu#get_ma_max_states
        val max_trans_diff = percentage_change_int spdg#get_ma_max_trans smu#get_ma_max_trans

        method get_time_diff = time_diff
        method get_max_states_diff = max_states_diff
        method get_max_trans_diff = max_trans_diff

        method to_string =
            "Time diff: " ^ (string_of_float time_diff) ^ " percent.\n" ^
            "Max states diff: " ^ (string_of_float max_states_diff) ^ " percent.\n" ^
            "Max trans diff: " ^ (string_of_float max_trans_diff) ^ " percent.\n"

        method to_row_string =
            (string_of_float time_diff) ^ " " ^
            (string_of_float max_states_diff) ^ " " ^
            (string_of_float max_trans_diff) ^ " "

    end;;







class stats_trials (file_prefix : string) (failval : [`Ignore | `Use of float * int * int ]) =
    let fail_time = match failval with `Ignore -> -1.0 | `Use(t, _, _) -> t in
    let fail_states = match failval with `Ignore -> -1 | `Use(_, n, _) -> n in
    let fail_trans = match failval with `Ignore -> -1 | `Use(_, _, n) -> n in
    let def_pds_stats = (new pds)#get_stats in
    let def_mu_stats = mu_get_stats (Prop("p")) in
    let def_pdmu_stats = new pdmu_stats def_pds_stats def_mu_stats in
    let def_pdg_stats = new pdg_stats def_pds_stats 0 in
    object (self)
        val raw_suffix = "_raw"
        val mu_suffix = "_mu"
        val g_suffix = "_g"
        val diff_suffix = "_diff"
        val xy_t_suffix = "_xy_t"
        val xy_ms_suffix = "_xy_ms"
        val xy_mt_suffix = "_xy_mt"
        val row_suffix = "_row"
        val nt_suffix = "_nt"
        val nms_suffix = "_nms"
        val nmt_suffix = "_nmt"
        val fail_suffix = "_fail"
        val file_perm = 755

        val mu_fail_stats = new mc_pdmu_stats fail_states fail_trans
                                              fail_states fail_trans
                                              0.0 fail_time 0.0
                                              def_pdmu_stats
        val g_fail_stats = new mc_parity_stats fail_states fail_trans
                                               fail_states fail_trans
                                               0.0 fail_time 0.0
                                               def_pdg_stats


        val file_prefix = file_prefix

        val mutable mu_fail = 0
        val mutable g_fail = 0
        val mutable both_fail = 0



        method begin_record = 
            let make_clean_file name = 
                let c = open_out name in
                close_out c in
            let make_three_files name =
                make_clean_file (name ^ mu_suffix);
                make_clean_file (name ^ g_suffix);
                make_clean_file (name ^ diff_suffix) in
            make_clean_file (file_prefix ^ raw_suffix);
            make_clean_file (file_prefix ^ xy_t_suffix);
            make_clean_file (file_prefix ^ xy_ms_suffix);
            make_clean_file (file_prefix ^ xy_mt_suffix);
            make_three_files (file_prefix ^ row_suffix);
            make_three_files (file_prefix ^ nt_suffix);
            make_three_files (file_prefix ^ nms_suffix);
            make_three_files (file_prefix ^ nmt_suffix);
            make_clean_file (file_prefix ^ fail_suffix);
            make_clean_file (file_prefix ^ fail_suffix ^ mu_suffix);
            make_clean_file (file_prefix ^ fail_suffix ^ g_suffix)


        method private open_append f =
            open_out_gen [Open_append] file_perm f 

        method private write_rawdata d =
            let raw_channel = self#open_append (file_prefix ^ raw_suffix) in
            let write_single stats = 
                output_string raw_channel (stats#to_string ^ "\n") in
            let write_compare smu spdg = 
                output_string raw_channel (smu#to_string  ^ "\n" ^ spdg#to_string ^ "\n");
                let comp = new stats_compare smu spdg in
                output_string raw_channel comp#to_string in
            (match d with
               (_, PdmuStats(smu), PdgStats(spdg)) -> write_compare smu spdg
             | (_, PdmuStats(smu), res) -> output_string raw_channel 
                                                         ("Game failed with " ^ (fail_to_string res) ^ "\n");
                                           write_single smu
             | (_, res, PdgStats(sg)) -> output_string raw_channel 
                                                       ("Mucalc failed with " ^ (fail_to_string res) ^ "\n");
                                         write_single sg
             | (_, res1, res2) -> output_string raw_channel
                                                ("Mucalc failed with " ^ (fail_to_string res1) ^ "\n" ^
                                                "Game failed with " ^ (fail_to_string res2) ^ "\n"));
            output_string raw_channel "\n\n---------------------------------------\n\n\n";
            close_out raw_channel


        method private add_row row file = 
            let c = self#open_append file in
            output_string c (row ^ "\n");
            close_out c


        method private write_mu_stats n smu = 
            if smu#get_check_time >= 0.0 then (
                let sn = string_of_int n in
                self#add_row (sn ^ smu#to_row_string) 
                             (file_prefix ^ row_suffix ^ mu_suffix); 
                self#add_row (sn ^ " " ^ (string_of_float smu#get_check_time))  
                             (file_prefix ^ nt_suffix ^ mu_suffix);
                self#add_row (sn ^ " " ^ (string_of_int smu#get_ma_max_states))
                             (file_prefix ^ nms_suffix ^ mu_suffix);
                self#add_row (sn ^ " " ^ (string_of_int smu#get_ma_max_trans))
                             (file_prefix ^ nmt_suffix ^ mu_suffix)
            )

           

        method private write_g_stats n sg = 
            if sg#get_check_time >= 0.0 then (
                let sn = string_of_int n in
                self#add_row (sn ^ sg#to_row_string) 
                             (file_prefix ^ row_suffix ^ g_suffix); 
                self#add_row (sn ^ " " ^ (string_of_float sg#get_check_time))  
                             (file_prefix ^ nt_suffix ^ g_suffix);
                self#add_row (sn ^ " " ^ (string_of_int sg#get_ma_max_states))
                             (file_prefix ^ nms_suffix ^ g_suffix);
                self#add_row (sn ^ " " ^ (string_of_int sg#get_ma_max_trans))
                             (file_prefix ^ nmt_suffix ^ g_suffix)
            )

 
        method private write_pair_stats n smu sg = 
            if (smu#get_check_time >= 0.0) && (sg#get_check_time >= 0.0) then (
                let sn = string_of_int n in
                self#add_row ((string_of_float smu#get_check_time) ^ " " ^ (string_of_float sg#get_check_time))
                             (file_prefix ^ xy_t_suffix);
                self#add_row ((string_of_int smu#get_ma_max_states) ^ " " ^ (string_of_int sg#get_ma_max_states))
                             (file_prefix ^ xy_ms_suffix);
                self#add_row ((string_of_int smu#get_ma_max_trans) ^ " " ^ (string_of_int sg#get_ma_max_trans))
                             (file_prefix ^ xy_mt_suffix);
                let comp = new stats_compare smu sg in
                self#add_row (sn ^ " " ^ (string_of_float comp#get_time_diff))  
                             (file_prefix ^ nt_suffix ^ diff_suffix);
                self#add_row (sn ^ " " ^ (string_of_float comp#get_max_states_diff))
                             (file_prefix ^ nms_suffix ^ diff_suffix);
                self#add_row (sn ^ " " ^ (string_of_float comp#get_max_trans_diff))
                             (file_prefix ^ nmt_suffix ^ diff_suffix);
                self#add_row (sn ^ " " ^ comp#to_row_string)
                             (file_prefix ^ row_suffix ^ diff_suffix)
            )

        method private write_faildata = 
            let c = open_out (file_prefix ^ fail_suffix) in
            let total = mu_fail + g_fail + both_fail in
            output_string c ("Total failures: " ^ (string_of_int total) ^ "\n" ^
                             "Just mu failures: " ^ (string_of_int mu_fail) ^ "\n" ^
                             "Just g failures: " ^ (string_of_int g_fail) ^ "\n" ^
                             "Both failures: " ^ (string_of_int both_fail) ^ "\n");
            close_out c
           
        method private write_mu_fail n = 
            self#add_row (string_of_int n) (file_prefix ^ fail_suffix ^ mu_suffix)

        method private write_g_fail n = 
            self#add_row (string_of_int n) (file_prefix ^ fail_suffix ^ g_suffix)


        method add_datum d = 
            let do_pair n smu sg = 
                self#write_mu_stats n smu;
                self#write_g_stats n sg;
                self#write_pair_stats n smu sg in
            self#write_rawdata d;
            match d with
              (_, PdgStats(_), _) -> prerr_string ("PdgStats found where Pdmu should be in stats add datum.\n")
            | (_, _, PdmuStats(_)) -> prerr_string ("PdmuStats found where Pdg should be in stats add datum.\n")
            | (n, PdmuStats(smu), PdgStats(sg)) -> do_pair n smu sg
            | (n, res1, PdgStats(sg)) -> mu_fail <- mu_fail + 1;  
                                                    self#write_mu_fail n; 
                                                    do_pair n mu_fail_stats sg
            | (n, PdmuStats(smu), res2) -> g_fail <- g_fail + 1; 
                                                     self#write_g_fail n; 
                                                     do_pair n smu g_fail_stats
            | (n, _, _) -> both_fail <- both_fail + 1; 
                           self#write_mu_fail n; 
                           self#write_g_fail n; 
                           do_pair n mu_fail_stats g_fail_stats;
            self#write_faildata;






    end;;
        

