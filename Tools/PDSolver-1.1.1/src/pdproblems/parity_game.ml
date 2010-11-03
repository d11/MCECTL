
open Pds;;
open Words;;

type owner = A | E;;

let owner_to_string o = 
    match o with 
      A -> "A"
    | E -> "E";;

module OrderedControlState =
    struct
        type t = control_state
        let compare = compare
    end;;

module PropertyMap = Map.Make(OrderedControlState);;

type property_map_internals = (owner * int ) PropertyMap.t;;


class property_map (vals : (control_state * owner * int) list) = 
    object
        val mutable map = List.fold_left (fun m (p, o, c) -> PropertyMap.add p (o, c) m) 
                                          PropertyMap.empty
                                          vals
        val mutable max_colour = List.fold_left (fun max_c (_, _, c) -> max max_c c)
                                                 0
                                                 vals
                                        
        val mutable min_colour = List.fold_left (fun min_c (_, _, c) -> if min_c < 0 then c
                                                                else min min_c c) 
                                                 (-1)
                                                 vals


        method get_colour p = snd (PropertyMap.find p map)
        method get_owner p = fst (PropertyMap.find p map)
        method get_max_colour = max_colour
        method get_min_colour = min_colour

        method add_prop (p : control_state) (o : owner) (c : int) =
            map <- PropertyMap.add p (o, c) map;
            min_colour <- min min_colour c;
            max_colour <- max max_colour c
 
 
        method to_channel ch = 
            let do_row p (o, c) = 
                output_string ch "control state = ";
                output_string ch ((control_state_to_string p) ^ "; owner = "); 
                output_string ch ((owner_to_string o) ^ "; colour = ");
                output_string ch ((string_of_int c) ^ "\n") in
            PropertyMap.iter do_row map 
           

        method to_string = 
            let do_row p (o, c) s = 
                s ^ "control state = " ^
                (control_state_to_string p) ^ "; owner = " ^
                (owner_to_string o) ^ "; colour = " ^
                (string_of_int c) ^ "\n" in
            PropertyMap.fold do_row map ""

        method get_internals = map

        method equal (props : property_map) = 
            let pair_eq (o1, c1) (o2, c2) = 
                (o1 = o2) && (c1 = c2) in
            PropertyMap.equal pair_eq map (props#get_internals) 
    end;;


class pdg_stats (pds_stats : pds_stats) (ncols : int) =
    object
        method get_pds_stats = pds_stats
        method get_ncols = ncols

        method to_string =
            pds_stats#to_string ^ 
            "Colours: " ^ (string_of_int ncols) ^ "\n"

        method to_row_string =
            pds_stats#to_row_string ^ " " ^
            (string_of_int ncols)

    end;;

class parity_game (pds : pds) (props : property_map) (interesting_configs : config list) = 
    object 
        val map = props
        val pds = pds
        val interesting_configs = interesting_configs

        method get_pds = pds
        method get_props = map
        method get_interesting_configs = interesting_configs

        method has_interesting_configs = 
            not (interesting_configs = [])

        method to_string = 
            "Pds:\n\n" ^ pds#to_string ^ "\n\nControl State Properties:\n\n" ^ map#to_string ^ "\n"

        method get_stats = 
            new pdg_stats (pds#get_stats) (map#get_max_colour - map#get_min_colour + 1)

        method to_channel c = 
            output_string c "Pds:\n\n";
            pds#to_channel c;
            output_string c "\n\nControl State Properties:\n\n";
            map#to_channel c;
            output_string c "\n"

        method equal (pdg : parity_game) =
            pds#equal (pdg#get_pds) &&
            map#equal (pdg#get_props)



    end;;
