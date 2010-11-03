

type id = string;;

type bin_op = Or | And | XOr | Equal | NEqual | Implies;;

type un_op = Not;;

type expression = Bin of expression * bin_op * expression |
                  Ternary of expression * expression * expression |
                  Un of un_op * expression |
                  True |
                  False |
                  Id of id |
                  AposId of id |
                  Nondet |
                  Choose of expression * expression;;

type statement_body = (* id+ := expr+ constrain expr *)
                      Assign of id list * expression list * expression |
                      Skip |
                      Goto of id list |
                      (* return id* *)
                      Return of expression list |
                      (* if expr then stmts else stmts *)
                      If of expression * statement list * statement list |
                      Assert of expression |
                      Assume of expression |
                      (* id* := id( expression* ) *)
                      Call of id list * id * expression list |
                      UnsupportedStatement |
                      While of expression * statement list
(* label1: label2: label3: . . . labeln: stmt *)
and statement = id list * statement_body;;


                 
type return_type = Void | Bool of int;;

(* return_type id( id* ) begin locals enforce abortif stmts end *)
type bp_method = return_type * id * id list * id list * expression * expression * statement list

type boolean_program = id list * bp_method list



let op_to_string op = 
    match op with
      Or -> "|"
    | And -> "&" 
    | XOr -> "^"
    | Equal -> "="
    | NEqual -> "!="
    | Implies -> "=>";;

let un_op_to_string op = 
    match op with
      Not -> "!";;
      
let rec expr_to_string e = 
    match e with 
      Bin(e1, op, e2) -> "(" ^ (expr_to_string e1) ^ " " ^
                         (op_to_string op) ^ " " ^
                         (expr_to_string e2) ^ ")"
    | Ternary(e1, e2, e3) -> "(" ^ (expr_to_string e1) ^ " ? " ^
                             (expr_to_string e2) ^ " : " ^
                             (expr_to_string e3) ^ ")"
    | Un(op, e) -> (un_op_to_string op) ^ "(" ^ (expr_to_string e) ^ ")"
    | True -> "T"
    | False -> "F"
    | Id(id) -> id
    | AposId(id) -> "'" ^ id
    | Nondet -> "*"
    | Choose(e1, e2) -> "schoose[" ^ (expr_to_string e1) ^ ", " ^
                        (expr_to_string e2) ^ "]";;


let id_list_to_string id_list = 
    String.concat ", " id_list;;

let expr_list_to_string e_list = 
    String.concat ", " (List.map expr_to_string e_list);;

let rec stmt_body_to_string body = 
    match body with
      Assign(ids, es, e) -> (id_list_to_string ids) ^ " := " ^
                            (expr_list_to_string es) ^ " where " ^
                            (expr_to_string e) 
    | Skip -> "skip"
    | Goto(ids) -> "goto " ^ (id_list_to_string ids)
    | Return(es) -> "return " ^ (expr_list_to_string es)
    | If(e, ifs, elses) -> "if " ^ (expr_to_string e) ^ " then\n" ^
                           (stmt_list_to_string ifs) ^ "\nelse\n" ^
                           (stmt_list_to_string elses) ^
                           "\nend if"
    | Assert(e) -> "assert " ^ (expr_to_string e)
    | Assume(e) -> "assume " ^ (expr_to_string e)
    | Call(ids, name, args) -> (id_list_to_string ids) ^ " := " ^
                               name ^ "(" ^ (expr_list_to_string args) ^ ")"
    | UnsupportedStatement -> "[unsupported statement]"
    | While(e, ss) -> "while " ^ (expr_to_string e) ^ " do\n" ^
                      (stmt_list_to_string ss) ^ 
                      "\nod"
and stmt_to_string s =
    let (labs, body) = s in
    (id_list_to_string labs) ^ ": " ^ (stmt_body_to_string body) 
and stmt_list_to_string s_list =
    String.concat "\n" (List.map stmt_to_string s_list);;


let return_type_to_string rt =
    match rt with
      Void -> "void" 
    | Bool(i) -> "bool<" ^ (string_of_int i) ^ ">";;

let bp_method_to_string m =
    let (ret_type, name, args, locals, enforce, abortif, stmts) = m in
    (return_type_to_string ret_type) ^ " " ^
    name ^ "(" ^
    (id_list_to_string args) ^ ")\ndecl " ^
    (id_list_to_string locals) ^ "\nenforce " ^
    (expr_to_string enforce) ^ "\nabortif " ^
    (expr_to_string abortif) ^ "\n" ^
    (stmt_list_to_string stmts) ^ "\n" ^
    "end";;

let method_list_to_string m_list =
    String.concat "\n\n" (List.map bp_method_to_string m_list);;

let bp_to_string bp = 
    let (globals, methods) = bp in
    "decl " ^ (id_list_to_string globals) ^ "\n\n" ^
    (method_list_to_string methods) ^ "\n\n";;


(* TODO: This, properly, perhaps. *)
let bp_to_channel ch bp = 
    output_string ch (bp_to_string bp);;



