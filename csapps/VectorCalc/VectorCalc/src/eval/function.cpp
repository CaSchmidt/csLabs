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

#include "eval/function.h"

#include "ast/ast.h"
#include "eval/engine.h"
#include "eval/environment.h"
#include "eval/exception.h"

namespace eval {

  ////// IFunction ///////////////////////////////////////////////////////////

  Function::~Function()
  {
  }

  ////// Function_new ////////////////////////////////////////////////////////

  Function_new::Function_new() = default;

  Function_new::~Function_new() = default;

  AST::IdentifierType Function_new::name() const
  {
    return AST::IdentifierType("new");
  }

  ResultPtr Function_new::evaluate(const AST::Function *node, const Engine *eng) const
  {
    if( static_cast<index_type>(node->args()->list().size()) != eng->environment()->dimension() ) {
      throw NumberOfArguments(node);
    }

    ResultPtr argsList = eng->dispatch(node->args());
    const std::list<ResultPtr>& args = dynamic_cast<const ResultList*>(argsList.get())->list();

    AST::ExpressionListType::const_iterator nodeIt = node->args()->list().begin();
    for(const ResultPtr& arg : args) {
      if( arg->context() != ResultContext::Scalar  ||
          arg->type() != ResultType::Real ) {
        throw ArgumentType(nodeIt->get());
      }
      ++nodeIt;
    }

    RealVector value(eng->environment()->dimension());

    index_type i = 0;
    for(const ResultPtr& arg : args) {
      const ScalarRealResult *srr = dynamic_cast<const ScalarRealResult*>(arg.get());
      value(i++) = srr->value();
    }

    return ResultPtr(new VectorRealResult(value));
  }

} // namespace eval
