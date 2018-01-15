%top{
#include "binary.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
}
%option noyywrap nounput batch noinput
%x comment
%x commentml
ws      [ \t\n]+

alpha   [A-Za-z]
dig     [0-9]
name    ({alpha}|[_])({alpha}|{dig}|[_])*

%%


"--[["           { BEGIN(commentml); }
<commentml>"]]" { BEGIN(INITIAL); }
<commentml>.    { }

"--"            { BEGIN(comment); }
<comment>"\n" { BEGIN(INITIAL); }
<comment>.    { }


{ws}									{}

for										{ return yy::parser::make_FOR(yytext);}
end										{ return yy::parser::make_ENDS(yytext);}
do										{ return yy::parser::make_DO(yytext);}
if										{ return yy::parser::make_IF(yytext);}
then									{ return yy::parser::make_THEN(yytext);}
else 									{ return yy::parser::make_ELSE(yytext);}
elseif 									{ return yy::parser::make_ELSEIF(yytext);}
repeat									{ return yy::parser::make_REPEAT(yytext);}
until									{ return yy::parser::make_UNTIL(yytext);}
while 									{ return yy::parser::make_WHILE(yytext);}
in 										{ return yy::parser::make_IN(yytext);}

function 								{ return yy::parser::make_FUNCTION(yytext);}

local									{ return yy::parser::make_LOCAL(yytext);}

return 									{ return yy::parser::make_RETURN(yytext);}
break 									{ return yy::parser::make_BREAK(yytext);}

or										{ return yy::parser::make_OR(yytext);}
and										{ return yy::parser::make_AND(yytext);}

not 									{ return yy::parser::make_NOT(yytext);}

(?i:nil)	 							{ return yy::parser::make_NIL(yytext);}		
(?i:false)								{ return yy::parser::make_FALSE(yytext);}
(?i:true)								{ return yy::parser::make_TRUE(yytext);}

{name}									{ return yy::parser::make_NAME(yytext); }

\"[^'"]*\"								{ std::string con(yytext+1, strlen(yytext)-2); return yy::parser::make_STRING(con);}


\( 										{ return yy::parser::make_OPENPAR(yytext); }
\) 										{ return yy::parser::make_ENDPAR(yytext); }

\{										{ return yy::parser::make_OPENCURLB(yytext); }
\} 										{ return yy::parser::make_ENDCURLB(yytext); }

\[										{ return yy::parser::make_OPENBRACK(yytext); }
\]										{ return yy::parser::make_ENDBRACK(yytext); }

[0-9]+									{ return yy::parser::make_INTEGER(yytext); }

\,										{ return yy::parser::make_COMA(yytext);}

\;										{ return yy::parser::make_SEMI(yytext);}

\+										{ return yy::parser::make_PLUS(yytext);}
\-										{ return yy::parser::make_MINUS(yytext);}
\*										{ return yy::parser::make_MULT(yytext);}
\/										{ return yy::parser::make_DIVIDE(yytext);}
\%										{ return yy::parser::make_MOD(yytext);}
\<=										{ return yy::parser::make_LESSEQUAL(yytext);}	
\<										{ return yy::parser::make_LESS(yytext);}	
\>=										{ return yy::parser::make_GREATEQUAL(yytext);}
\>										{ return yy::parser::make_GREAT(yytext);}
(\=\=)									{ return yy::parser::make_EQUAL(yytext);}
(\~\=)									{ return yy::parser::make_NEQUAL(yytext);}
\#										{ return yy::parser::make_HASH(yytext);}
\.\.\. 									{ return yy::parser::make_TRIPDOT(yytext);}
\.\. 									{ return yy::parser::make_DBDOT(yytext);}
\.										{ return yy::parser::make_DOT(yytext);}
\^										{ return yy::parser::make_POW(yytext);}

\=										{ return yy::parser::make_ASSIGN(yytext);}
	


<<EOF>>      							{ return yy::parser::make_END();}
%%