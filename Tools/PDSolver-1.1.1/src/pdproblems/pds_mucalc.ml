

open Pds;;
open Mucalc;;
open Words;;

module OrderedProposition =
    struct
        type t = id
        let compare = compare
        let to_string x = x
    end;;

module PropSet = Extset.Make(OrderedProposition);;
module PropositionMap = Map.Make(OrderedProposition);;



type property_map_internals = HeadSet.t PropositionMap.t;;

let add_proposition_to_map p a map x = 
    try
        let headset = PropositionMap.find x map in
        PropositionMap.add x (HeadSet.add (p, a) headset) map
    with Not_found ->
        PropositionMap.add x (HeadSet.singleton (p, a)) map

let add_propositions_to_map p a map xs = 
    List.fold_left (add_proposition_to_map p a) map xs

 
class proposition_map (vals : (control_state * character * (id list)) list) = 
    object 
        val mutable map = List.fold_left (fun m (p, a, xs) -> 
                                              add_propositions_to_map p a m xs)
                                          PropositionMap.empty
                                          vals

        val mutable props = List.fold_left (fun props (p, a, xs) ->
                                                PropSet.union (PropSet.from_list xs) props)
                                           PropSet.empty
                                           vals



        method get_heads x = 
            if PropositionMap.mem x map then
                (PropositionMap.find x map)
            else
                HeadSet.empty

        method get_props = props

        method add_proposition (p : control_state) (a : character) (x : id) =
            map <- add_proposition_to_map p a map x;
            props <- PropSet.add x props


        method head_sat p a x = 
            try
                let heads = PropositionMap.find x map in
                HeadSet.mem (p, a) heads
            with Not_found -> false


        method to_channel ch = 
            let do_row x headset = 
                output_string ch (x ^ ": " ^ (HeadSet.to_string headset));
                output_string ch "\n\n" in
            PropositionMap.iter do_row map


        method to_string = 
            let do_row x headset s = 
                s ^ 
                x ^ ": " ^ (HeadSet.to_string headset) ^
                "\n\n" in
            PropositionMap.fold do_row map ""


        method get_internals = map

        method equal (props : proposition_map) = 
            PropositionMap.equal HeadSet.equal map (props#get_internals) 
    end;;


class pdmu_stats (pds_stats : pds_stats) (mu_stats : mu_stats) = 
    object
        method get_pds_stats = pds_stats
        method get_mu_stats = mu_stats

        method to_string =
            pds_stats#to_string ^ 
            mu_stats#to_string

        method to_row_string = 
            pds_stats#to_row_string ^ " " ^
            mu_stats#to_row_string
    end;;

class pds_mucalc (pds : pds) 
                 (interesting_configs : config list)
                 (fmla : mu_formula) 
                 (props : proposition_map) = 
    object 
        val map = props
        val pds = pds
        val fmla = fmla
        val interesting_configs = interesting_configs

        method get_pds = pds
        method get_props = map
        method get_fmla = fmla
        method get_interesting_configs = interesting_configs

        method has_interesting_configs = 
            not (interesting_configs = [])

        method to_channel ch = 
            output_string ch "Pds:\n\n";
            pds#to_channel ch;
            output_string ch "\n\nMu Formula:\n\n";
            output_string ch (mu_formula_to_string fmla);
            output_string ch "\n\nPropositions:\n\n";
            map#to_channel ch;
            output_string ch "\n"


        method get_stats = 
            new pdmu_stats (pds#get_stats) (mu_get_stats fmla)

        method to_string = 
            "Pds:\n\n" ^ pds#to_string ^ 
            "\n\nMu Formula:\n\n" ^ (mu_formula_to_string fmla) ^
            "\n\nPropositions:\n\n" ^
            map#to_string ^ "\n"

        method equal (pdmu : pds_mucalc) =
            pds#equal (pdmu#get_pds) &&
            map#equal (pdmu#get_props) &&
            ((compare fmla (pdmu#get_fmla)) = 0)



    end;;


