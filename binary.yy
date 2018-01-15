%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
	#include "node.hh"
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
   Node* root = nullptr;
  YY_DECL;
}

%type <Chunk*> chunk
%type <Chunk*> block
%type <Chunk*> anynumstatements

%type <FunctionCall*> functioncall
%type <Statement*> statement
%type <Statement*> laststatement
%type <Statement*> forstatement
%type <If*> ifstatement
%type <If*> elseifconstructor
%type <If*> elseif

%type <Expression*> args
%type <FuncName*> funcname
%type <FunctionBody*> funcbody

%type <Field*> field
%type <Field*> fieldsep
%type <Field*> fieldlist
%type <Table*> tableconstructor
%type <Variable*> varlist
%type <Expression*> explist
%type <NameList*> namelist

%type <Expression*> prefixexp
%type <Variable*> var
%type <Expression*> exp


%type <Field*> optfieldlist
%type <Node*> optassign
%type <Node*> optsemi
%type <Expression*> optexplist
%type <NameList*> parlist

%token <std::string> NAME

%token <std::string> NEWLINE

%token <std::string> OPENPAR
%token <std::string> ENDPAR
%token <std::string> OPENCURLB
%token <std::string> ENDCURLB
%token <std::string> OPENBRACK
%token <std::string> ENDBRACK

%token <std::string> COMA
%token <std::string> DOT
%token <std::string> SEMI

%token <std::string> PLUS
%token <std::string> MINUS
%token <std::string> MULT
%token <std::string> DIVIDE
%token <std::string> MOD
%token <std::string> LESS
%token <std::string> LESSEQUAL
%token <std::string> GREAT
%token <std::string> GREATEQUAL
%token <std::string> EQUAL
%token <std::string> NEQUAL
%token <std::string> DBDOT
%token <std::string> TRIPDOT
%token <std::string> AND
%token <std::string> OR
%token <std::string> POW

%token <std::string> NOT
%token <std::string> HASH

%token <std::string> ASSIGN

%token <std::string> INTEGER
%token <std::string> STRING
%token <std::string> NIL
%token <std::string> TRUE
%token <std::string> FALSE

%token <std::string>  FOR
%token <std::string>  ENDS
%token <std::string>  DO
%token <std::string>  IF
%token <std::string>  THEN
%token <std::string>  ELSE
%token <std::string>  ELSEIF
%token <std::string>  REPEAT
%token <std::string>  UNTIL
%token <std::string>  WHILE
%token <std::string>  IN
%token <std::string>  FUNCTION

%token <std::string>  LOCAL

%token <std::string>  RETURN
%token <std::string>  BREAK

%token END 0 "end of file"


%left OR AND 
%left LESS GREAT LESSEQUAL GREATEQUAL NEQUAL EQUAL 
%right DBDOT 
%left PLUS MINUS 
%left MULT DIVIDE MOD 
%left NOT HASH NEGATIVE 
%right POW
%%


chunk 			: block											{ root = $1; std::cout << "block: anynumstatements laststatement" << std::endl;}
				;

block 			: anynumstatements laststatement optsemi		{ $$ = $1; $$->AddStatement($2); std::cout << "block: anynumstatements laststatement" << std::endl;}
				| laststatement optsemi							{ $$ = new Chunk($1); std::cout << "block: laststatement" << std::endl;}
     			;

anynumstatements: statement	optsemi								{ $$ = new Chunk($1); std::cout << "anynumstatements: statement" << std::endl;}
				| anynumstatements statement optsemi			{ $$ = $1; $$->AddStatement($2); std::cout << "anynumstatements: anynumstatements statement" << std::endl;}
				;

laststatement 	: /*empty*/										{ $$ = nullptr; std::cout << "laststatement: empty" << std::endl;}
				| RETURN explist								{ $$ = new Return($2); std::cout << "laststatement: return explist" << std::endl;}
				| BREAK											{ $$ = nullptr; std::cout << "laststatement: break" << std::endl;}
				;

functioncall	: prefixexp args								{ $$ = new FunctionCall($1, $2); std::cout << "functioncall: prefixexp args" << std::endl;}	
				;

varlist 		: varlist COMA var								{ $1->AddExpression($3); $$ = $1; std::cout << "varlist: varlist COMA var" << std::endl;}	
				| var											{ $$ = $1; std::cout << "varlist: var" << std::endl;}	
				;

statement		: varlist ASSIGN explist 						{ $$ = new Assignment($1, $3); std::cout << "statement: varlist = explist" << std::endl;}	
				| functioncall 									{ $$ = $1; std::cout << "statement: functioncall" << std::endl;}
				| DO block ENDS									{ $$ = new DoEnd($2); std::cout << "statement: DO chunk ENDS" << std::endl;}
				| WHILE exp DO block ENDS						{ $$ = new WhileDoEnd($4, $2); std::cout << "statement: WHILE exp DO chunk ENDS" << std::endl;}
				| forstatement									{ $$ = $1; std::cout << "statement: forstatement" << std::endl;}
				| ifstatement 									{ $$ = $1; std::cout << "statement: ifstatement" << std::endl;}
				| REPEAT block UNTIL exp 						{ $$ = new RepeatUntil($2, $4); std::cout << "statement: REPEAT stream UNTIL exp" << std::endl;}
				| FUNCTION funcname funcbody  					{ $$ = new Function($2, $3); std::cout << "statement: FUNCTION funcname funcbody" << std::endl;}
				| LOCAL FUNCTION funcname funcbody  			{ $$ = new Function($3, $4); std::cout << "statement: LOCAL FUNCTION funcname funcbody" << std::endl;}
				| LOCAL namelist optassign						{ $$ = nullptr; std::cout << "statement: LOCAL namelist optassign" << std::endl;}
				;

forstatement	: FOR NAME ASSIGN exp COMA exp DO block ENDS			{ $$ = new For(new NameVariable($2), $4, $6, $8); std::cout << "forstatement: FOR " << $2 << " = exp COMA exp DO stream ENDS" << std::endl;}
				| FOR NAME ASSIGN exp COMA exp COMA exp DO block ENDS	{ $$ = new For(new NameVariable($2), $4, $6, $8, $10); std::cout << "forstatement: FOR " << $2 << " = exp COMA exp COMA exp DO stream ENDS" << std::endl;}
				| FOR namelist IN explist DO block ENDS 				{ $$ = new ForIn($6); std::cout << "forstatement: FOR namelist IN explist DO chunk ENDS" << std::endl;}
				;

ifstatement   	: IF exp THEN block ENDS									{ $$ = new If($2, $4); std::cout << "ifstatement: IF exp THEN stream ENDS" << std::endl;}
				| IF exp THEN block ELSE block ENDS							{ $$ = new If($2, $4, $6); std::cout << "ifstatement: IF exp THEN stream ELSE stream ENDS" << std::endl;}
				| IF exp THEN block elseifconstructor ENDS					{ $$ = new If($2, $4, $5); std::cout << "ifstatement: IF exp THEN stream elseifconstructor ENDS" << std::endl;}
				| IF exp THEN block elseifconstructor ELSE block ENDS		{ $$ = new If($2, $4, $7, $5); std::cout << "ifstatement: IF exp THEN stream elseifconstructor ELSE stream ENDS" << std::endl;}
				;

elseifconstructor: elseif 										{ $$ = $1; std::cout << "elseifconstructor: elseif" << std::endl;}
				| elseifconstructor elseif 						{ $1->AddElseIf($2); $$ = $1; std::cout << "elseifconstructor: elseifconstructor elseif" << std::endl;}		
				;

elseif 			: ELSEIF exp THEN block 						{ $$ = new If($2, $4); std::cout << "elseif: ELSEIF exp THEN stream" << std::endl;}
				; 

funcname		: NAME											{ $$ = new FuncName($1); std::cout << "funcname: "<< $1 << std::endl;}
				| funcname DOT NAME								{ $$ = new FuncName($1, $3); std::cout << "funcname: funcname DOT " << $3 << std::endl;}
				;

funcbody		: OPENPAR parlist ENDPAR block ENDS      		{ $$ = new FunctionBody($2, $4); std::cout << "funcbody: ( optnamelist ) stream ENDS" << std::endl;}
				;

parlist			: /*empty*/										{ $$ = nullptr; std::cout << "parlist: empty" << std::endl;}
				| namelist										{ $$ = $1; std::cout << "parlist: namelist" << std::endl;}
				| TRIPDOT										{ $$ = new NameList($1); std::cout << "parlist: TRIPDOT"<< $1 << std::endl;}
				| namelist COMA TRIPDOT 						{ $$ = new NameList($1, $3); std::cout << "parlist: namelist COMA TRIPDOT" << std::endl;}
				;

namelist		: NAME 											{ $$ = new NameList($1); std::cout << "namelist: "<< $1 << std::endl;}
				| namelist COMA NAME 							{ $$ = $1; $$->AddName(new NameList($3)); std::cout << "namelist: namelist COMA "<< $3 << std::endl;}
				;


args			: OPENPAR optexplist ENDPAR						{ $$ = $2; std::cout << "args: ( optexplist ) " << std::endl;}
				| tableconstructor 								{ $$ = $1; std::cout << "args: exp " << std::endl;}
				| STRING 										{ $$ = new String($1); std::cout << "args: "<< $1 << std::endl;}
				;

optassign		: /*empty*/										{ $$ = nullptr; std::cout << "optassign: empty " << std::endl;}
				| ASSIGN explist								{ $$ = nullptr; std::cout << "optassign: = explist " << std::endl;}
				;
optexplist		: /*empty*/										{ $$ = nullptr; std::cout << "optexplist: empty" << std::endl;}
				| explist										{ $$ = $1; std::cout << "optexplist: explist" << std::endl;}
				;

explist			: explist COMA exp	 							{ $$ = $1; $$->AddExpression($3); std::cout << "explist: exp COMA explist " << std::endl;}
				| exp											{ $$ = $1; std::cout << "explist: exp" << std::endl;}
				;

exp				: INTEGER 										{ $$ = new Integer($1); std::cout << "INTEGER: " << $1 << std::endl;}
				| INTEGER DOT INTEGER 							{ $$ = new Float($1, $3); std::cout << "FLOAT: " << $1 << std::endl;}
				| FALSE 										{ $$ = new Boolean(false); std::cout << "FALSE:" << std::endl;}
				| TRUE 											{ $$ = new Boolean(true); std::cout << "TRUE:" << std::endl;}
				| prefixexp 									{ $$ = $1; std::cout << "exptype: prefixexp" << std::endl;}
				| tableconstructor 								{ $$ = $1; std::cout << "exptype: tableconstructor "<< std::endl;}
				| STRING 										{ $$ = new String($1); std::cout << "STRING:" << $1 << std::endl;}
				| NIL 											{ $$ = new Nil; std::cout << "NIL:" << std::endl;}
				| FUNCTION funcbody								{ $$ = $2; std::cout << "exptype: FUNCTION funcbody" << std::endl;}		
				| exp PLUS exp									{ $$ = new Binop($1, $3, Operators::Add); std::cout << "exp: exp PLUS exp" << std::endl;}
				| exp MINUS exp									{ $$ = new Binop($1, $3, Operators::Minus); std::cout << "exp: exp MINUS exp" << std::endl;}
				| exp MULT exp									{ $$ = new Binop($1, $3, Operators::Mult); std::cout << "exp: exp MULT exp" << std::endl;}
				| exp DIVIDE exp								{ $$ = new Binop($1, $3, Operators::Divide); std::cout << "exp: exp DIVIDE exp" << std::endl;}
				| exp EQUAL	exp									{ $$ = new Binop($1, $3, Operators::Equal); std::cout << "exp: exp EQUAL exp" << std::endl;}
				| exp MOD exp									{ $$ = new Binop($1, $3, Operators::Mod); std::cout << "exp: exp MOD exp" << std::endl;}
				| exp LESS exp 									{ $$ = new Binop($1, $3, Operators::Less); std::cout << "exp: exp LESS exp" << std::endl;}
				| exp LESSEQUAL exp								{ $$ = new Binop($1, $3, Operators::LessEqual); std::cout << "exp: exp LESSEQUAL exp" << std::endl;}	
				| exp GREAT exp 								{ $$ = new Binop($1, $3, Operators::Greater); std::cout << "exp: exp GREAT exp" << std::endl;}
				| exp GREATEQUAL exp							{ $$ = new Binop($1, $3, Operators::GreaterEqual); std::cout << "exp: exp GREATEQUAL exp" << std::endl;}
				| exp NEQUAL exp								{ $$ = new Binop($1, $3, Operators::NotEqual); std::cout << "exp: exp NEQUAL exp" << std::endl;}
				| exp AND exp									{ $$ = new Binop($1, $3, Operators::And); std::cout << "exp: exp AND exp" << std::endl;}
				| exp OR exp									{ $$ = new Binop($1, $3, Operators::Or); std::cout << "exp: exp OR exp" << std::endl;}
				| exp DBDOT exp									{ $$ = new Binop($1, $3, Operators::DBDot); std::cout << "exp: exp DBDOT exp" << std::endl;}
				| exp POW exp									{ $$ = new Binop($1, $3, Operators::Pow); std::cout << "exp: exp POW exp" << std::endl;}
				| HASH exp										{ $$ = new Unop($2, Operators::Hash); std::cout << "exp: HASH exp" << std::endl;}	
				| NOT exp										{ $$ = new Unop($2, Operators::Not); std::cout << "exp: NOT exp" << std::endl;}	
				| MINUS	exp										{ $$ = new Unop($2, Operators::Negative); std::cout << "exp: NEGATIVE exp" << std::endl;}	
				;	

prefixexp 		: var 											{ $$ = $1; std::cout << "prefixexp: var" << std::endl;}
				| OPENPAR exp ENDPAR 							{ $$ = $2; std::cout << "prefixexp: ( exp )" << std::endl;}
				| functioncall 									{ $$ = $1; std::cout << "prefixexp: functioncall" << std::endl;}
				;

var 			: NAME 											{ $$ = new NameVariable($1); std::cout << "var: " << $1 << std::endl;}
				| prefixexp OPENBRACK exp ENDBRACK 				{ $$ = new PrefixVariable($1, $3); std::cout << "var: prefixexp [ exp ]" << std::endl;}
				| prefixexp DOT NAME 							{ $$ = new PrefixVariable($1, new NameVariable($3)); std::cout << "var: prefixexp DOT " << $3 << std::endl;}
				;

tableconstructor: OPENCURLB optfieldlist ENDCURLB				{ $$ = new Table($2); std::cout << "tableconstructor: { optfieldlist }" << std::endl;}			
				;

optfieldlist   	: /*empty*/										{ $$ = nullptr; std::cout << "optfieldlist: empty" << std::endl;}
				| fieldlist										{ $$ = $1; std::cout << "optfieldlist: fieldlist" << std::endl;}
				;

fieldlist		: field 										{ $$ = $1; std::cout << "fieldlist: field" << std::endl;}			
				| fieldlist fieldsep field 						{ $1->AddExpression($3); $$ = $1; std::cout << "fieldlist: fieldlist fieldsep field " << std::endl;}			
				;

field 			: exp 											{ $$ = new Field($1); std::cout << "field: exp " << std::endl;}		
				| NAME ASSIGN exp 								{ $$ = new NameIndexField($1, $3); std::cout << "field: NAME ASSIGN exp" << std::endl;}					
				| OPENBRACK exp ENDBRACK ASSIGN exp 			{ $$ = new ExpIndexField($2, $5); std::cout << "field: [ exp ] ASSIGN exp" << std::endl;}		
				;

fieldsep		: COMA											{ $$ = nullptr; std::cout << "fieldsep: COMA" << std::endl;}			
				| SEMI											{ $$ = nullptr; std::cout << "fieldsep: SEMI" << std::endl;}			
				;

optsemi			: /*empty*/										{ $$ = nullptr; }
				| SEMI											{ $$ = nullptr; }
				;