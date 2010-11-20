D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*
RB                      }

%{

#define YY_NO_UNPUT
void count(void);

%}

%option noyywrap

%%

"//"[^\n]+\n            { count (); }

"{"[^ }\n]+"}"          { count(); 
                          bplval.c = strdup(bptext);
                          return(IDENTIFIER); }

"return"		{ count(); return(RETURN); }
"goto"		        { count(); return(GOTO); }
"skip"		        { count(); return(SKIP); }
"do"			{ count(); return(DO); }
"od"			{ count(); return(OD); }
"else"			{ count(); return(ELSE); }
"elsif"			{ count(); return(ELSIF); }
"if"			{ count(); return(IF); }
"then"			{ count(); return(THEN); }
"fi"			{ count(); return(FI); }
"decl"		        { count(); return(DECL); }
"while"			{ count(); return(WHILE);} 
"begin"			{ count(); return(BEG); }
"end"			{ count(); return(END); }
"bool"			{ count(); return(BOOL); }
"void"			{ count(); return(VOID); }
"assert"		{ count(); return(ASSERT); }
"assume"		{ count(); return(ASSUME); }
"print"		        { count(); return(PRINT); }
"enforce"               { count(); return(ENFORCE); }
"constrain"             { count(); return(CONSTRAIN); }
"schoose"		{ count(); return(CHOOSE); }
"*"			{ count(); return(NONDET); }
"?"		   	{ count(); return(NONDET); }
"_"			{ count(); bplval.c = strdup(bptext); return(IDENTIFIER); }
({D})+                  { count(); bplval.i = atoi(bptext); return(INTEGER); }
{L}({L}|{D})*	        { count(); bplval.c = strdup(bptext); return(IDENTIFIER); }

":="		        { count(); return(ASSIGN); }
"="			{ count(); return(EQ_OP); }
"!="		        { count(); return(NE_OP); }
"=>"		        { count(); return(IMPLIES); }
";"			{ count(); return(';'); }

"'"                     { count(); return('\''); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
"["			{ count(); return('['); }
"]"			{ count(); return(']'); }
"&"			{ count(); return(AND); }
"&&"			{ count(); return(AND); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"^"			{ count(); return('^'); }
"|"			{ count(); return(OR); }
"||"			{ count(); return(OR); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }

[ \t\v\n\f]		{ count(); }
.			{ /* ignore bad characters */ }

%%

int column, line;

void count(void)
{
	int i;

	for (i = 0; bptext[i] != '\0'; i++) 
		if (bptext[i] == '\n') {
			column = 0;
                        line++;
		} else if (bptext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;
}

