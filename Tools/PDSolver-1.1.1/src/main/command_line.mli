
exception Argument_error of string

type out_format = Text | Dot
type random_type = NoRandom | Mucalc | Pds | Pds_mucalc | Game;;
type check_type = ChkInput | ChkGame | ChkBoth | ChkMucalc | ChkNone;;

val read_args : unit -> unit

val get_input_files : unit -> string list
val get_output_file : unit -> string
val get_output_format : unit -> out_format
val get_verbose : unit -> bool
val get_help_requested : unit -> bool
val get_hard_optimise : unit -> bool
val get_optimise_product_union : unit -> bool
val get_translate_only : unit -> bool
val get_random_type : unit -> random_type
val get_num_props : unit -> int
val get_num_props : unit -> int
val get_prop_density : unit -> float
val get_max_depth : unit -> int
val get_num_controls : unit -> int
val get_num_chars : unit -> int
val get_max_push : unit -> int
val get_min_push : unit -> int
val get_tran_density_min : unit -> float
val get_tran_density_max : unit -> float
val get_min_fp_depth : unit -> int
val get_check_type : unit -> check_type
val get_gather_stats : unit -> bool
val get_abelard_density : unit -> float
val get_num_colours_min : unit -> int
val get_num_colours_max : unit -> int
val get_timeout : unit -> int
val get_show_progress : unit -> bool
val get_generate_data : unit -> bool
val get_data_min_n : unit -> int
val get_data_max_n : unit -> int
val get_data_step : unit -> int
val get_data_reps : unit -> int
val get_generate_infinite_data : unit -> bool
val get_restrict_reachable : unit -> bool
val get_init_p : unit -> string
val get_poststar : unit -> bool
val get_eclipse_mode : unit -> bool

