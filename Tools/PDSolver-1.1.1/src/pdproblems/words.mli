
type character
type word 

module CharacterSet : Extset.S with type elt = character

val sbot : character
val cdefault : character

val reset_module : unit -> unit

val make_character : string -> character
val make_word : character list -> word
val word_map : (character -> character) -> word -> word

val word_to_list : word -> character list
val word_get_characters : word -> CharacterSet.t
val word_to_string : word -> string
val character_to_string : character -> string

val characters_equal : character -> character -> bool

val word_ends_with : word -> character -> bool
val word_contains : word -> character -> bool
val word_is_empty : word -> bool


