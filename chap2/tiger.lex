%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"
#include "comment.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}

digit [0-9]
letter [a-zA-Z]
%Start INITIALS COMMENTS

%%
"/*"  {adjust(); RemallocCommentBufIfNeeded(yyleng); StartNewComment(); AppendToCommentBuf(yytext, yyleng); BEGIN COMMENTS;}
<COMMENTS>"*/" {adjust(); AppendToCommentBuf(yytext, yyleng); EndComment(); if (commentLevel == 0) BEGIN INITIALS; return COMMENT;}
<COMMENTS>. {adjust(); RemallocCommentBufIfNeeded(yyleng); AppendToCommentBuf(yytext, yyleng);}
<INITIALS>" "	 {adjust(); continue;}
<INITIALS>\n	 {adjust(); EM_newline(); continue;}
<INITIALS>\t {adjust(); continue;}
<INITIALS>","	 {adjust(); return COMMA;}
<INITIALS>":"  {adjust(); return COLON;}
<INITIALS>";"  {adjust(); return SEMICOLON;}
<INITIALS>"("  {adjust(); return LPAREN;}
<INITIALS>")"  {adjust(); return RPAREN;}
<INITIALS>"["  {adjust(); return LBRACK;}
<INITIALS>"]"  {adjust(); return RBRACK;}
<INITIALS>"{"  {adjust(); return LBRACE;}
<INITIALS>"}"  {adjust(); return RBRACE;}
<INITIALS>"."  {adjust(); return DOT;}
<INITIALS>"+"  {adjust(); return PLUS;}
<INITIALS>"-"  {adjust(); return MINUS;}
<INITIALS>"*"  {adjust(); return TIMES;}
<INITIALS>"/"  {adjust(); return DIVIDE;}
<INITIALS>"="  {adjust(); return EQ;}
<INITIALS>"<>" {adjust(); return NEQ;}
<INITIALS>"<"  {adjust(); return LT;}
<INITIALS>"<=" {adjust(); return LE;}
<INITIALS>">"  {adjust(); return GT;}
<INITIALS>">=" {adjust(); return GE;}
<INITIALS>"&"  {adjust(); return AND;}
<INITIALS>"|"  {adjust(); return OR;}
<INITIALS>":=" {adjust(); return ASSIGN;}
<INITIALS>while {adjust(); return WHILE;}
<INITIALS>for  	 {adjust(); return FOR;}
<INITIALS>to {adjust(); return TO;}
<INITIALS>break {adjust(); return BREAK;}
<INITIALS>let {adjust(); return LET;}
<INITIALS>in {adjust(); return IN;}
<INITIALS>end {adjust(); return END;}
<INITIALS>function {adjust(); return FUNCTION;}
<INITIALS>var {adjust(); return VAR;}
<INITIALS>type {adjust(); return TYPE;}
<INITIALS>array {adjust(); return ARRAY;}
<INITIALS>if {adjust(); return IF;}
<INITIALS>then {adjust(); return THEN;}
<INITIALS>else {adjust(); return ELSE;}
<INITIALS>do {adjust(); return DO;}
<INITIALS>of {adjust(); return OF;}
<INITIALS>nil {adjust(); return NIL;}
<INITIALS>{letter}[_0-9a-zA-Z]* {adjust(); yylval.sval = yytext; return ID;}
<INITIALS>[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
<INITIALS>.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
.   {BEGIN INITIALS; yyless(1);}
