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

#include <cmath>

#include "eval/arithmetic.h"

#include "eval/engine.h"
#include "eval/environment.h"

namespace eval {

  bool isArithmetic(const AST::Node *node)
  {
    const AST::NodeType id = node->nodeType();
    return
        id == AST::NodeType::Addition        ||
        id == AST::NodeType::Division        ||
        id == AST::NodeType::Modulo          ||
        id == AST::NodeType::Multiplication  ||
        id == AST::NodeType::Subtraction;
  }

  ResultPtr arithmetic(const Result *lhs, const AST::NodeType op, const Result *rhs, const Engine *eng)
  {
    ResultPtr result;

    const ScalarRealResult *lhsS = dynamic_cast<const ScalarRealResult*>(lhs);
    const VectorRealResult *lhsV = dynamic_cast<const VectorRealResult*>(lhs);

    const ScalarRealResult *rhsS = dynamic_cast<const ScalarRealResult*>(rhs);
    const VectorRealResult *rhsV = dynamic_cast<const VectorRealResult*>(rhs);

    if(        lhs->context() == ResultContext::Scalar  &&  rhs->context() == ResultContext::Scalar ) {
      AST::RealType value;
      if(        op == AST::NodeType::Addition ) {
        value = lhsS->value() + rhsS->value();
      } else if( op == AST::NodeType::Division ) {
        value = lhsS->value() / rhsS->value();
      } else if( op == AST::NodeType::Modulo ) {
        value = std::fmod(lhsS->value(), rhsS->value());
      } else if( op == AST::NodeType::Multiplication ) {
        value = lhsS->value() * rhsS->value();
      } else if( op == AST::NodeType::Subtraction ) {
        value = lhsS->value() - rhsS->value();
      }
      result.reset(new ScalarRealResult(value));

    } else if( lhs->context() == ResultContext::Scalar  &&  rhs->context() == ResultContext::Vector ) {
      RealVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::Addition ) {
        value = lhsS->value() + rhsV->value();
      } else if( op == AST::NodeType::Division ) {
        value = lhsS->value() / rhsV->value();
      } else if( op == AST::NodeType::Modulo ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = std::fmod(lhsS->value(), rhsV->value()(i));
        }
      } else if( op == AST::NodeType::Multiplication ) {
        value = lhsS->value() * rhsV->value();
      } else if( op == AST::NodeType::Subtraction ) {
        value = lhsS->value() - rhsV->value();
      }
      result.reset(new VectorRealResult(value));

    } else if( lhs->context() == ResultContext::Vector  &&  rhs->context() == ResultContext::Scalar ) {
      RealVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::Addition ) {
        value = lhsV->value() + rhsS->value();
      } else if( op == AST::NodeType::Division ) {
        value = lhsV->value() / rhsS->value();
      } else if( op == AST::NodeType::Modulo ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = std::fmod(lhsV->value()(i), rhsS->value());
        }
      } else if( op == AST::NodeType::Multiplication ) {
        value = lhsV->value() * rhsS->value();
      } else if( op == AST::NodeType::Subtraction ) {
        value = lhsV->value() - rhsS->value();
      }
      result.reset(new VectorRealResult(value));

    } else if( lhs->context() == ResultContext::Vector  &&  rhs->context() == ResultContext::Vector ) {
      RealVector value(eng->environment()->dimension());
      if(        op == AST::NodeType::Addition ) {
        value = lhsV->value() + rhsV->value();
      } else if( op == AST::NodeType::Division ) {
        value = lhsV->value() / rhsV->value();
      } else if( op == AST::NodeType::Modulo ) {
        for(index_type i = 0; i < eng->environment()->dimension(); i++) {
          value(i) = std::fmod(lhsV->value()(i), rhsV->value()(i));
        }
      } else if( op == AST::NodeType::Multiplication ) {
        value = lhsV->value() * rhsV->value();
      } else if( op == AST::NodeType::Subtraction ) {
        value = lhsV->value() - rhsV->value();
      }
      result.reset(new VectorRealResult(value));

    }

    return result;
  }

} // namespace eval
