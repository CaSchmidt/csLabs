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

%{
#include "internal/ast/parser_p.h"
%}

%file-prefix "ast"
%language "C"
%lex-param {void *scanner}
%parse-param {void *scanner}

%define api.prefix {ast}
%define api.pure full
%define api.token.prefix {AST_TOKEN_}
%define api.value.type {AST::Node*}

%token IDENTIFIER
%token OP_LOG_AND
%token OP_LOG_OR
%token OP_REL_EQ
%token OP_REL_GEQ
%token OP_REL_LEQ
%token OP_REL_NEQ
%token REAL
%token UINT

%destructor { delete $$; $$ = nullptr; } <>

%start entry

%%

entry: %empty
       {
         AST::ParserState *extra = astget_extra(scanner);
         extra->result.reset(nullptr);
         $$ = nullptr;
       }
     | expression
       {
         AST::ParserState *extra = astget_extra(scanner);
         extra->result.reset($1);
         $$ = nullptr;
         $1 = nullptr;
       }
     | IDENTIFIER '=' expression
       {
         AST::ParserState *extra = astget_extra(scanner);
         extra->result.reset(new AST::Assignment(dynamic_cast<AST::Identifier*>($1), $3));
         $$ = nullptr;
         $1 = nullptr;
         $3 = nullptr;
       }
     ;

expression: logical_or_expr
            {
              $$ = $1;
            }
          ;

logical_or_expr: logical_and_expr
                 {
                   $$ = $1;
                 }
               | logical_or_expr OP_LOG_OR logical_and_expr
                 {
                   $$ = new AST::Or($1, $3);
                 }
               ;

logical_and_expr: relational_expr
                  {
                    $$ = $1;
                  }
                | logical_and_expr OP_LOG_AND relational_expr
                  {
                    $$ = new AST::And($1, $3);
                  }
                ;

relational_expr: additive_expr
                 {
                   $$ = $1;
                 }
               | relational_expr OP_REL_EQ additive_expr
                 {
                   $$ = new AST::Equal($1, $3);
                 }
               | relational_expr OP_REL_NEQ additive_expr
                 {
                   $$ = new AST::NotEqual($1, $3);
                 }
               | relational_expr '>' additive_expr
                 {
                   $$ = new AST::Greater($1, $3);
                 }
               | relational_expr OP_REL_GEQ additive_expr
                 {
                   $$ = new AST::GreaterEqual($1, $3);
                 }
               | relational_expr '<' additive_expr
                 {
                   $$ = new AST::Less($1, $3);
                 }
               | relational_expr OP_REL_LEQ additive_expr
                 {
                   $$ = new AST::LessEqual($1, $3);
                 }
               ;

additive_expr: multiplicative_expr
               {
                 $$ = $1;
               }
             | additive_expr '+' multiplicative_expr
               {
                 $$ = new AST::Addition($1, $3);
               }
             | additive_expr '-' multiplicative_expr
               {
                 $$ = new AST::Subtraction($1, $3);
               }
             ;

multiplicative_expr: unary_expr
                     {
                       $$ = $1;
                     }
                   | multiplicative_expr '*' unary_expr
                     {
                       $$ = new AST::Multiplication($1, $3);
                     }
                   | multiplicative_expr '/' unary_expr
                     {
                       $$ = new AST::Division($1, $3);
                     }
                   | multiplicative_expr '%' unary_expr
                     {
                       $$ = new AST::Modulo($1, $3);
                     }
                   ;

unary_expr: primary_expr
            {
              $$ = $1;
            }
          | '+' primary_expr
            {
              $$ = new AST::Plus($2);
            }
          | '-' primary_expr
            {
              $$ = new AST::Minus($2);
            }
          | '!' primary_expr
            {
              $$ = new AST::Negation($2);
            }
          ;

primary_expr: constant
              {
                $$ = $1;
              }
            | '(' expression ')'
              {
                $$ = new AST::Expression($2);
              }
            | IDENTIFIER '(' expression_list_opt ')'
              {
                $$ = new AST::Function(dynamic_cast<AST::Identifier*>($1),
                                       dynamic_cast<AST::ExpressionList*>($3));
              }
            | IDENTIFIER
              {
                $$ = $1;
              }
            ;

expression_list_opt: %empty
                     {
                       $$ = new AST::ExpressionList();
                     }
                   | expression
                     {
                       AST::ExpressionList *list = new AST::ExpressionList();
                       list->append($1);
                       $$ = list;
                     }
                   | expression_list_opt ',' expression
                     {
                       AST::ExpressionList *list = dynamic_cast<AST::ExpressionList*>($1);
                       list->append($3);
                       $$ = list;
                     }
                   ;

constant: REAL
          {
            $$ = $1;
          }
        | UINT
          {
            $$ = $1;
          }
        ;

%%
