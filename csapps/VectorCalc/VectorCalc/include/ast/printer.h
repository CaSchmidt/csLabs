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

#ifndef ASTPRINTER_H
#define ASTPRINTER_H

#include <sstream>

#include "ast/ast.h"

namespace AST {

  class Printer {
  public:
    Printer(const int indent = 2);
    ~Printer();

    std::string print(const Node *node, int col = 0, const AST::Node *caret = nullptr);

    void setFloatFlags(const std::ios_base::fmtflags flags);
    void setFloatPrecision(const std::streamsize precision);

  private:
    std::string caretString(const AST::Node *node, const AST::Node *caret);
    void dispatch(const Node *node, const int col, const AST::Node *caret);
    void indentation(const int col);

    void print(const Assignment *node, const int col, const AST::Node *caret);
    void print(const BinaryExpression *node, const int col, const AST::Node *caret);
    void print(const RealConstant *node, const int col, const AST::Node *caret);
    void print(const UIntConstant *node, const int col, const AST::Node *caret);
    void print(const Expression *node, const int col, const AST::Node *caret);
    void print(const ExpressionList *node, const int col, const AST::Node *caret);
    void print(const Function *node, const int col, const AST::Node *caret);
    void print(const Identifier *node, const int col, const AST::Node *caret);
    void print(const UnaryExpression *node, const int col, const AST::Node *caret);

    int _indent;
    std::ostringstream _out;
  };

} // namespace AST

#endif // ASTPRINTER_H
