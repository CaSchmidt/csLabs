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

#ifndef ENGINE_H
#define ENGINE_H

#include <map>

#include "ast/ast.h"
#include "eval/function.h"

namespace eval {

  class Environment;

  class Engine {
  public:
    Engine(Environment *env);
    ~Engine();

    ResultPtr dispatch(const AST::Node *node) const;

    const Environment *environment() const;

    bool isFunction(const AST::IdentifierType& name) const;

  private:
    using FunctionStore = std::map<AST::IdentifierType,FunctionPtr>;

    ResultPtr evaluate(const AST::Assignment *node) const;
    ResultPtr evaluate(const AST::BinaryExpression *node) const;
    ResultPtr evaluate(const AST::RealConstant *node) const;
    ResultPtr evaluate(const AST::UIntConstant *node) const;
    ResultPtr evaluate(const AST::Expression *node) const;
    ResultPtr evaluate(const AST::ExpressionList *node) const;
    ResultPtr evaluate(const AST::Function *node) const;
    ResultPtr evaluate(const AST::Identifier *node) const;
    ResultPtr evaluate(const AST::UnaryExpression *node) const;

    Environment *_env;
    FunctionStore _functions;
  };

} // namespace eval

#endif // ENGINE_H
