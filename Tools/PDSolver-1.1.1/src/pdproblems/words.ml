

type character = int;;


exception Unknown_char_lookup of character;;

type word = character list;;

module OrderedChar =
struct
    type t = character
    let compare = compare
end;;

module OrderedString = 
struct 
    type t = string
    let compare = compare
end;;

module CharNameMap = Map.Make(OrderedChar);;
module CharNumMap = Map.Make(OrderedString);;

let sbot = 0;;
let cdefault = 1;;

let next_id = ref 2;;
let get_next_id () = 
let id = !next_id in
next_id := !next_id + 1;
id;;

let init_name_map = let map1 = CharNameMap.add 0 "#" (CharNameMap.empty) in
                    CharNameMap.add 1 "[default]" map1;;
let init_num_map = let map1 = CharNumMap.add "#" 0 (CharNumMap.empty) in
                   CharNumMap.add "[default]" 1 map1;;

let char_name_map = ref init_name_map;;
let char_num_map = ref init_num_map;;


let reset_module () = 
    next_id := 2;
    char_name_map := init_name_map;
    char_num_map := init_num_map;;



let make_character ch = 
    if CharNumMap.mem ch !char_num_map then
        CharNumMap.find ch !char_num_map
    else (
        let id = get_next_id () in
        char_num_map := CharNumMap.add ch id !char_num_map;
        char_name_map := CharNameMap.add id ch !char_name_map;
        id
    );;


let character_to_string ch = 
    if CharNameMap.mem ch !char_name_map then
        CharNameMap.find ch !char_name_map
    else
        raise (Unknown_char_lookup(ch));;

let make_word w = w;;

let word_to_list word = word;;

let word_to_string word = 
    String.concat " " (List.map character_to_string (word_to_list word));;


let characters_equal a b = (a = b)

let rec word_ends_with (w : word) (a : character) = 
    match w with 
      [] -> false
    | [last] -> characters_equal a last
    | h::t -> word_ends_with t a

let rec word_contains (w : word) (a : character) = 
    match w with
      [] -> false
    | h::t -> (characters_equal h a) || (word_contains t a)

let word_is_empty (w : word) = (w = [])

let word_map f w = List.map f w


module OrderedCharacter = 
    struct
        type t = character
        let compare = compare
        let to_string = character_to_string
    end;;


module CharacterSet = Extset.Make(OrderedCharacter);;

let word_get_characters w = 
    let do_char chars c = 
        CharacterSet.add c chars in 
    List.fold_left do_char CharacterSet.empty (word_to_list w);;


