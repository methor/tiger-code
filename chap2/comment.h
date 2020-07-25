#pragma once
#include "util.h"

typedef struct _tComment tComment;
struct _tComment
{
    int emTokPos;
    int start;
    int length;
    tComment *prev;
};

extern int commentLevel;

void RemallocCommentBufIfNeeded(int count);
void AppendToCommentBuf(string yytext, int yyleng);
void StartNewComment();
void EndComment();
void ResetCommentIfNeeded();