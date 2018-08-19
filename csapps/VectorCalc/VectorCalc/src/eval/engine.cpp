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

#include "eval/engine.h"

#include "eval/arithmetic.h"
#include "eval/environment.h"
#include "eval/exception.h"
#include "eval/logical.h"
#include "eval/relational.h"
#include "eval/unary.h"

namespace eval {

  ////// public //////////////////////////////////////////////////////////////

  Engine::Engine(Environment *env)
    : _env(env)
  {
    _functions["new"] = FunctionPtr(new Function_new());
  }

  Engine::~Engine() = default;

  ResultPtr Engine::dispatch(const AST::Node *node) const
  {
    ResultPtr result;

    if(        node->nodeType() == AST::NodeType::Assignment ) {
      result = evaluate(dynamic_cast<const AST::Assignment*>(node));

    } else if( dynamic_cast<const AST::BinaryExpression*>(node) != nullptr ) {
      result = evaluate(dynamic_cast<const AST::BinaryExpression*>(node));

    } else if( node->nodeType() == AST::NodeType::RealConstant ) {
      result = evaluate(dynamic_cast<const AST::RealConstant*>(node));

    } else if( node->nodeType() == AST::NodeType::UIntConstant ) {
      result = evaluate(dynamic_cast<const AST::UIntConstant*>(node));

    } else if( node->nodeType() == AST::NodeType::Expression ) {
      result = evaluate(dynamic_cast<const AST::Expression*>(node));

    } else if( node->nodeType() == AST::NodeType::ExpressionList ) {
      result = evaluate(dynamic_cast<const AST::ExpressionList*>(node));

    } else if( node->nodeType() == AST::NodeType::Function ) {
      result = evaluate(dynamic_cast<const AST::Function*>(node));

    } else if( node->nodeType() == AST::NodeType::Identifier ) {
      result = evaluate(dynamic_cast<const AST::Identifier*>(node));

    } else if( dynamic_cast<const AST::UnaryExpression*>(node) != nullptr ) {
      result = evaluate(dynamic_cast<const AST::UnaryExpression*>(node));

    }

    return result;
  }

  const Environment *Engine::environment() const
  {
    return _env;
  }

  bool Engine::isFunction(const AST::IdentifierType& name) const
  {
    return _functions.find(name) != _functions.end();
  }

  ////// private /////////////////////////////////////////////////////////////

  ResultPtr Engine::evaluate(const AST::Assignment *node) const
  {
    return dispatch(node->expr());
  }

  ResultPtr Engine::evaluate(const AST::BinaryExpression *node) const
  {
    const ResultPtr lhs = dispatch(node->lhs());
    const ResultPtr rhs = dispatch(node->rhs());

    ResultPtr result;
    if(        isArithmetic(node) ) {
      if( node->rhs()->nodeType() == AST::NodeType::Minus  ||
          node->rhs()->nodeType() == AST::NodeType::Plus ) {
        throw Operand(node->rhs());
      }
      if( lhs->type() != ResultType::Real ) {
        throw OperandType(node->lhs());
      }
      if( rhs->type() != ResultType::Real ) {
        throw OperandType(node->rhs());
      }
      result = arithmetic(lhs.get(), node->nodeType(), rhs.get(), this);

    } else if( isLogical(node) ) {
      if( lhs->type() != ResultType::Bool ) {
        throw OperandType(node->lhs());
      }
      if( rhs->type() != ResultType::Bool ) {
        throw OperandType(node->rhs());
      }
      result = logical(lhs.get(), node->nodeType(), rhs.get(), this);

    } else if( isRelational(node) ) {
      if( lhs->type() != ResultType::Real ) {
        throw OperandType(node->lhs());
      }
      if( rhs->type() != ResultType::Real ) {
        throw OperandType(node->rhs());
      }
      result = relational(lhs.get(), node->nodeType(), rhs.get(), this);

    }

    return result;
  }

  ResultPtr Engine::evaluate(const AST::RealConstant *node) const
  {
    ResultPtr result(new ScalarRealResult(node->value()));
    return result;
  }

  ResultPtr Engine::evaluate(const AST::UIntConstant *node) const
  {
    const AST::UIntType uint_max(0x20000000000000); // 2^53

    if( node->value() > uint_max ) {
      throw ConstantRange(node);
    }
    ResultPtr result(new ScalarRealResult(static_cast<AST::RealType>(node->value())));
    return result;
  }

  ResultPtr Engine::evaluate(const AST::Expression *node) const
  {
    return dispatch(node->expr());
  }

  ResultPtr Engine::evaluate(const AST::ExpressionList *node) const
  {
    std::list<ResultPtr> list;
    for(const AST::NodePtr& ptr : node->list()) {
      ResultPtr result = dispatch(ptr.get());
      list.emplace_back(result.release());
    }
    ResultPtr result(new ResultList(list));
    return result;
  }

  ResultPtr Engine::evaluate(const AST::Function *node) const
  {
    if( !isFunction(node->name()) ) {
      throw UnknownFunction(node);
    }
    Function *function = _functions.at(node->name()).get();
    return function->evaluate(node, this);
  }

  ResultPtr Engine::evaluate(const AST::Identifier *node) const
  {
    const Result *value = _env->variables().value(node->value());
    if( value == nullptr ) {
      throw UnknownVariable(node);
    }
    ResultPtr result(value->copy());
    return result;
  }

  ResultPtr Engine::evaluate(const AST::UnaryExpression *node) const
  {
    const ResultPtr rhs = dispatch(node->rhs());

    ResultPtr result;
    if(        isUnaryBool(node) ) {
      if( node->rhs()->nodeType() != AST::NodeType::Expression  &&
          node->rhs()->nodeType() != AST::NodeType::Function ) {
        throw Operand(node->rhs());
      }
      if( rhs->type() != ResultType::Bool ) {
        throw OperandType(node->rhs());
      }
      result = unaryBool(node->nodeType(), rhs.get(), this);

    } else if( isUnaryReal(node) ) {
      if( rhs->type() != ResultType::Real ) {
        throw OperandType(node->rhs());
      }
      result = unaryReal(node->nodeType(), rhs.get(), this);

    }

    return result;
  }

} // namespace eval
