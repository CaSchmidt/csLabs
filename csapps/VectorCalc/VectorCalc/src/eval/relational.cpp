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
#include "eval/environment.h"

#include "eval/relational.h"

namespace eval {

  bool isRelational(const AST::Node *node)
  {
    const AST::NodeType id = node->nodeType();
    return
        id == AST::NodeType::Equal         ||
        id == AST::NodeType::Greater       ||
        id == AST::NodeType::GreaterEqual  ||
        id == AST::NodeType::Less          ||
        id == AST::NodeType::LessEqual     ||
        id == AST::NodeType::NotEqual;
  }

  ResultPtr relational(const Result *lhs, const AST::NodeType op, const Result *rhs, const Engine *eng)
  {
    ResultPtr result;

    const ScalarRealResult *lhsS = dynamic_cast<const ScalarRealResult*>(lhs);
    const VectorRealResult *lhsV = dynamic_cast<const VectorRealResult*>(lhs);

    const ScalarRealResult *rhsS = dynamic_cast<const ScalarRealResult*>(rhs);
    const VectorRealResult *rhsV = dynamic_cast<const VectorRealResult*>(rhs);

    if(        lhs->context() == ResultContext::Scalar  &&  rhs->context() == ResultContext::Scalar ) {
      AST::BoolType value;
      if(        op == AST::NodeType::Equal ) {
        value = lhsS->value() == rhsS->value();
      } else if( op == AST::NodeType::Greater ) {
        value = lhsS->value() >  rhsS->value();
      } else if( op == AST::NodeType::GreaterEqual ) {
        value = lhsS->value() >= rhsS->value();
      } else if( op == AST::NodeType::Less ) {
        value = lhsS->value() <  rhsS->value();
      } else if( op == AST::NodeType::LessEqual ) {
        value = lhsS->value() <= rhsS->value();
      } else if( op == AST::NodeType::NotEqual ) {
        value = lhsS->value() != rhsS->value();
      }
      result.reset(new ScalarBoolResult(value));

    } else if( lhs->context() == ResultContext::Scalar  &&  rhs->context() == ResultContext::Vector ) {
      BoolVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::Equal ) {
        value = lhsS->value() == rhsV->value();
      } else if( op == AST::NodeType::Greater ) {
        value = lhsS->value() >  rhsV->value();
      } else if( op == AST::NodeType::GreaterEqual ) {
        value = lhsS->value() >= rhsV->value();
      } else if( op == AST::NodeType::Less ) {
        value = lhsS->value() <  rhsV->value();
      } else if( op == AST::NodeType::LessEqual ) {
        value = lhsS->value() <= rhsV->value();
      } else if( op == AST::NodeType::NotEqual ) {
        value = lhsS->value() != rhsV->value();
      }
      result.reset(new VectorBoolResult(value));

    } else if( lhs->context() == ResultContext::Vector  &&  rhs->context() == ResultContext::Scalar ) {
      BoolVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::Equal ) {
        value = lhsV->value() == rhsS->value();
      } else if( op == AST::NodeType::Greater ) {
        value = lhsV->value() >  rhsS->value();
      } else if( op == AST::NodeType::GreaterEqual ) {
        value = lhsV->value() >= rhsS->value();
      } else if( op == AST::NodeType::Less ) {
        value = lhsV->value() <  rhsS->value();
      } else if( op == AST::NodeType::LessEqual ) {
        value = lhsV->value() <= rhsS->value();
      } else if( op == AST::NodeType::NotEqual ) {
        value = lhsV->value() != rhsS->value();
      }
      result.reset(new VectorBoolResult(value));

    } else if( lhs->context() == ResultContext::Vector  &&  rhs->context() == ResultContext::Vector ) {
      BoolVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::Equal ) {
        value = lhsV->value() == rhsV->value();
      } else if( op == AST::NodeType::Greater ) {
        value = lhsV->value() >  rhsV->value();
      } else if( op == AST::NodeType::GreaterEqual ) {
        value = lhsV->value() >= rhsV->value();
      } else if( op == AST::NodeType::Less ) {
        value = lhsV->value() <  rhsV->value();
      } else if( op == AST::NodeType::LessEqual ) {
        value = lhsV->value() <= rhsV->value();
      } else if( op == AST::NodeType::NotEqual ) {
        value = lhsV->value() != rhsV->value();
      }
      result.reset(new VectorBoolResult(value));

    }

    return result;
  }

} // namespace eval
