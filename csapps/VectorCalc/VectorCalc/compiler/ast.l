/****************************************************************************
** Copyright (c) 2018, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

%option never-interactive nounistd noyywrap
%option 8bit
%option bison-bridge
%option extra-type="struct AST::ParserState*"
%option prefix="ast"
%option reentrant

%{
#include <cstring>

#include <csUtil/csCharConv.h>
#include <csUtil/csFormat.h>
#include <csUtil/csILogger.h>

#include "internal/ast/parser_p.h"
#include "ast.tab.h"

#define YYSTYPE ASTSTYPE

#define YY_INPUT(buf,result,max_size)                                \
  {                                                                  \
    AST::ParserState *extra = yyget_extra(yyscanner);                  \
    const AST::ParserState::Size numRead = extra->read(buf, max_size); \
    result = numRead < 1  ?  YY_NULL : numRead;                      \
  }
%}

IDENT  [_a-zA-Z][_a-zA-Z0-9]*

DSEQ      [0-9]+
DSEQ_OPT  [0-9]*
FRAC      ({DSEQ_OPT}\.{DSEQ})|({DSEQ}\.)
EXP       [eE][-+]?{DSEQ}
EXP_OPT   {EXP}?

%%

  AST::ParserState *extra = yyget_extra(yyscanner);
  *yylval = nullptr;

{DSEQ} {
  AST::UIntType value(0);
  const cs::from_chars_result result =
      cs::from_chars(yytext, yytext + std::strlen(yytext), value);
  if( result.ec != std::errc() ) {
    extra->logger->logError(csFormat("Unable to convert number \"%1\"!")
                            % yytext);
    return YY_NULL;
  }
  *yylval = new AST::UIntConstant(value);
  return AST_TOKEN_UINT;
}

({FRAC}{EXP_OPT})|({DSEQ}{EXP}) {
  AST::RealType value;
  const cs::from_chars_result result =
      cs::from_chars(yytext, yytext + std::strlen(yytext), value);
  if( result.ec != std::errc() ) {
    extra->logger->logError(csFormat("Unable to convert number \"%1\"!")
                            % yytext);
    return YY_NULL;
  }
  *yylval = new AST::RealConstant(value);
  return AST_TOKEN_REAL;
}

{IDENT} {
  *yylval = new AST::Identifier(yytext);
  return AST_TOKEN_IDENTIFIER;
}

"&&" {
  return AST_TOKEN_OP_LOG_AND;
}

"||" {
  return AST_TOKEN_OP_LOG_OR;
}

"==" {
  return AST_TOKEN_OP_REL_EQ;
}

">=" {
  return AST_TOKEN_OP_REL_GEQ;
}

"<=" {
  return AST_TOKEN_OP_REL_LEQ;
}

"!=" {
  return AST_TOKEN_OP_REL_NEQ;
}

[-+*/%=<>()!,] {
  return int(yytext[0]);
}

[ \f\n\r\t\v]+ {
  // skip whitespace
}

. {
  extra->logger->logError(csFormat("Invalid character '%1' (0x%2)!")
                          .arg(yytext).arg(yytext[0], 2, 16, '0'));
  return YY_NULL;
}

%%
