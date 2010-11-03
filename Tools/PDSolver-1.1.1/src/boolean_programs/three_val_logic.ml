

type three_val = T | F | N;;


let or3 v1 v2 = 
    match (v1, v2) with
      (T, T) -> T
    | (T, F) -> T
    | (T, N) -> T
    | (F, T) -> T
    | (F, F) -> F
    | (F, N) -> N
    | (N, T) -> T
    | (N, F) -> N
    | (N, N) -> N;;
    
let and3 v1 v2 = 
    match (v1, v2) with
      (T, T) -> T
    | (T, F) -> F
    | (T, N) -> N
    | (F, T) -> F
    | (F, F) -> F
    | (F, N) -> F
    | (N, T) -> N
    | (N, F) -> F
    | (N, N) -> N;;


let xor3 v1 v2 = 
    match (v1, v2) with
      (T, T) -> F
    | (T, F) -> T
    | (T, N) -> N
    | (F, T) -> T
    | (F, F) -> F
    | (F, N) -> N
    | (N, T) -> N
    | (N, F) -> N
    | (N, N) -> N;;
 
 
 
let eq3 v1 v2 = 
    match (v1, v2) with
      (T, T) -> T
    | (T, F) -> F
    | (T, N) -> N
    | (F, T) -> F
    | (F, F) -> T
    | (F, N) -> N
    | (N, T) -> N
    | (N, F) -> N
    | (N, N) -> N;;

let neq3 v1 v2 = 
    match (v1, v2) with
      (T, T) -> N
    | (T, F) -> T
    | (T, N) -> N
    | (F, T) -> T
    | (F, F) -> F
    | (F, N) -> N
    | (N, T) -> N
    | (N, F) -> N
    | (N, N) -> N;;

let imp3 v1 v2 = 
    match (v1, v2) with
      (T, T) -> T
    | (T, F) -> F
    | (T, N) -> N
    | (F, T) -> T
    | (F, F) -> T
    | (F, N) -> T
    | (N, T) -> T
    | (N, F) -> N
    | (N, N) -> N;;


let not3 v = 
    match v with
      T -> F
    | F -> T
    | N -> N;;

(* if then else *)
let ite3 i t e = 
    match (i, t, e) with
      (T, x, _) -> x
    | (F, _, x) -> x
    | (N, T, T) -> T
    | (N, T, F) -> N
    | (N, T, N) -> N
    | (N, F, T) -> N
    | (N, F, F) -> F
    | (N, F, N) -> N
    | (N, N, _) -> N
