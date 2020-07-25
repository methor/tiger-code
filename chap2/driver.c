#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "tokens.h"
#include "comment.h"

YYSTYPE yylval;

tComment *gComment = NULL;
tComment *gLastComment = NULL;
int commentLevel = 0;
int commentBufCap = 0;
int commentBufPos = 0;
char *pCommentBuf = NULL;

void RemallocCommentBufIfNeeded(int count) {
  if (commentBufPos + count > commentBufCap || pCommentBuf == NULL) {
    if (commentBufCap == 0)
      commentBufCap = 50;
    pCommentBuf = realloc(pCommentBuf, commentBufCap * 2);
    commentBufCap = commentBufCap * 2;
  }
}

void AppendToCommentBuf(string yytext, int yyleng) {
  memcpy(pCommentBuf + commentBufPos, yytext, yyleng);
  commentBufPos += yyleng;
}

void StartNewComment() {
  tComment* pComment = malloc(sizeof(tComment));
  pComment->emTokPos = EM_tokPos;
  pComment->start = commentBufPos;
  pComment->prev = gComment;
  gComment = pComment;
  commentLevel++;
}

void EndComment() {
  gComment->length = commentBufPos - gComment->start;
  gLastComment = gComment;
  gComment = gComment->prev;
  --commentLevel;
}

void ResetCommentIfNeeded() {
  if (commentLevel == 0) {
    commentBufCap = 0;
    commentBufPos = 0;
    free(pCommentBuf);
    pCommentBuf = NULL;
  }
}



int yylex(void); /* prototype for the lexing function */

string toknames[] = {
    "ID", "STRING", "INT", "COMMA", "COLON", "SEMICOLON", "LPAREN",
    "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS",
    "MINUS", "TIMES", "DIVIDE", "EQ", "NEQ", "LT", "LE", "GT", "GE",
    "AND", "OR", "ASSIGN", "ARRAY", "IF", "THEN", "ELSE", "WHILE", "FOR",
    "TO", "DO", "LET", "IN", "END", "OF", "BREAK", "NIL", "FUNCTION",
    "VAR", "TYPE", "COMMENT"};

string tokname(tok)
{
  return tok < 257 || tok > 300 ? "BAD_TOKEN" : toknames[tok - 257];
}

int main(int argc, char **argv)
{
  string fname;
  int tok;
  if (argc != 2)
  {
    fprintf(stderr, "usage: a.out filename\n");
    exit(1);
  }
  fname = argv[1];
  EM_reset(fname);
  for (;;)
  {
    tok = yylex();
    if (tok == 0)
      break;
    switch (tok)
    {
    case ID:
    case STRING:
      printf("%10s %4d %s\n", tokname(tok), EM_tokPos, yylval.sval);
      break;
    case INT:
      printf("%10s %4d %d\n", tokname(tok), EM_tokPos, yylval.ival);
      break;
    case COMMENT:
      printf("%10s %4d %.*s\n", tokname(tok), gLastComment->emTokPos, gLastComment->length, gLastComment->start + pCommentBuf);
      free(gLastComment);
      ResetCommentIfNeeded();
      break;
    default:
      printf("%10s %4d\n", tokname(tok), EM_tokPos);
    }
  }
  return 0;
}
