
open Pds;;
open Words;;
open Mucalc;;
open Pds_mucalc;;
open Parity_game;;

(* random_char n generates one of n random characters *)
val random_char : int -> character
(* random_control n generates one of n random control states *)
val random_control : int -> control_state
(* random_word nchar len generates a word from nchar characters of length len *)
val random_word : int -> int -> word
(* random_tran ncontrol nchar min_push max_push generates a tran from ncontrol control states
 * nchar characters and with words of up to lengh max_push min len min_push *)
val random_tran : int -> int -> int -> int -> rule 
(* random_pds ncontrol nchar min_push max_push tran_density_min tran_density_max generates a pds with ntran rules
   from nchar characters, ncontrol control states and with words of up to length max_push, where ntran 
   is a random tran density times controls times chars *)
val random_pds : int -> int -> int -> int -> float -> float -> pds
(* a default filter for below: rejects formulas that do not use all of their bound variables, 
 * and insists at least one occurence is within a [] or <> operator (otherwise
 * fixed points don't say much) *)
val default_mu_filter : mu_formula -> bool
(* random_mu_calc props max_depth produces any random formula in positive normal
 * form with maximum operator depth over the given propositions in Prop(id) format
 * (or more general if you wish) *)
val random_mu_calc : mu_formula list -> int -> mu_formula
(* random_mu_calc_filter filter props max_depth -- as above, but the filter can reject nonsensical formulas (true accepts) *)
val random_mu_calc_filter : (mu_formula -> bool) -> mu_formula list -> int -> mu_formula 
(* as above but applies the default filter (described above) *)
val default_random_mu_calc_filter : mu_formula list -> int -> mu_formula 
(* as above, but first arg is number of props rather than a list of props *)
val random_mu_calc_nprops : int -> int -> mu_formula
val random_mu_calc_filter_nprops : (mu_formula -> bool) -> int -> int -> mu_formula 
val default_random_mu_calc_filter_nprops : int -> int -> mu_formula
(* random_pds_mucalc nprops prop_density max_depth mu_filter ncontrols nchars min_push max_push tran_density_min tran_density_max 
 * creates a pds and mucalc formula, randomly assigning nprops propositions to control state
 * char pairs, each with a prob of prop_density *)
val random_pds_mucalc : int -> float -> int -> (mu_formula -> bool) -> int -> int -> int -> int -> float -> float -> pds_mucalc
(* as above, but uses default filter *)
val default_random_pds_mucalc : int -> float -> int -> int -> int -> int -> int -> float -> float -> pds_mucalc
(* random_pdg ncontrol nchar min_push max_push tran_density_min tran_density_max min_colour max_colour abelard_density generates 
 * a random pushdown game with args as above, each state having probability
 * abelard_density of belonging to abelard, and colours between min and max *)
val random_pdg : int -> int -> int -> int -> float -> float -> int -> int -> float -> parity_game
(* random_pdg ncontrol nchar min_push max_push tran_density_min tran_density_max ncolours_min ncolours_max abelard_density generates 
 * a random pushdown game with args as above, each state having probability
 * abelard_density of belonging to abelard, and random num of colours / tran
 * density between stated vals *) 
val random_pdg_ncols : int -> int -> int -> int -> float -> float -> int -> int -> float -> parity_game
