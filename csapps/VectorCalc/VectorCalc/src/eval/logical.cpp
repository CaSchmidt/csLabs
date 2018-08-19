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

namespace eval {

  bool isLogical(const AST::Node *node)
  {
    const AST::NodeType id = node->nodeType();
    return
        id == AST::NodeType::And  ||
        id == AST::NodeType::Or;
  }

  ResultPtr logical(const Result *lhs, const AST::NodeType op, const Result *rhs, const Engine *eng)
  {
    ResultPtr result;

    const ScalarBoolResult *lhsS = dynamic_cast<const ScalarBoolResult*>(lhs);
    const VectorBoolResult *lhsV = dynamic_cast<const VectorBoolResult*>(lhs);

    const ScalarBoolResult *rhsS = dynamic_cast<const ScalarBoolResult*>(rhs);
    const VectorBoolResult *rhsV = dynamic_cast<const VectorBoolResult*>(rhs);

    if(        lhs->context() == ResultContext::Scalar  &&  rhs->context() == ResultContext::Scalar ) {
      AST::BoolType value;
      if(        op == AST::NodeType::And ) {
        value = lhsS->value()  &&  rhsS->value();
      } else if( op == AST::NodeType::Or ) {
        value = lhsS->value()  ||  rhsS->value();
      }
      result.reset(new ScalarBoolResult(value));

    } else if( lhs->context() == ResultContext::Scalar  &&  rhs->context() == ResultContext::Vector ) {
      BoolVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::And ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = lhsS->value()  &&  rhsV->value()(i);
        }
      } else if( op == AST::NodeType::Or ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = lhsS->value()  ||  rhsV->value()(i);
        }
      }
      result.reset(new VectorBoolResult(value));

    } else if( lhs->context() == ResultContext::Vector  &&  rhs->context() == ResultContext::Scalar ) {
      BoolVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::And ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = lhsV->value()(i)  &&  rhsS->value();
        }
      } else if( op == AST::NodeType::Or ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = lhsV->value()(i)  ||  rhsS->value();
        }
      }
      result.reset(new VectorBoolResult(value));

    } else if( lhs->context() == ResultContext::Vector  &&  rhs->context() == ResultContext::Vector ) {
      BoolVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::And ) {
        value = lhsV->value()  &&  rhsV->value();
      } else if( op == AST::NodeType::Or ) {
        value = lhsV->value()  ||  rhsV->value();
      }
      result.reset(new VectorBoolResult(value));

    }

    return result;
  }

} // namespace eval
