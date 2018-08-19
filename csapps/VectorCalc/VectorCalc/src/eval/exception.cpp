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

#include "eval/exception.h"

namespace eval {

  ////// Exception ///////////////////////////////////////////////////////////

  Exception::Exception(const AST::Node *node)
    : _node(const_cast<AST::Node*>(node))
  {
  }

  Exception::~Exception() = default;

  const AST::Node *Exception::node() const
  {
    return _node;
  }

  ////// ArgumentType ////////////////////////////////////////////////////////

  ArgumentType::ArgumentType(const AST::Node *node)
    : Exception(node)
  {
  }

  ArgumentType::~ArgumentType() = default;

  const char *ArgumentType::what() const noexcept
  {
    return "argument type";
  }

  ////// ConstantRange ///////////////////////////////////////////////////////

  ConstantRange::ConstantRange(const AST::Node *node)
    : Exception(node)
  {
  }

  ConstantRange::~ConstantRange() = default;

  const char *ConstantRange::what() const noexcept
  {
    return "constant range";
  }

  ////// NumberOfArguments ///////////////////////////////////////////////////

  NumberOfArguments::NumberOfArguments(const AST::Node *node)
    : Exception(node)
  {
  }

  NumberOfArguments::~NumberOfArguments() = default;

  const char *NumberOfArguments::what() const noexcept
  {
    return "number of arguments";
  }

  ////// Operand /////////////////////////////////////////////////////////////

  Operand::Operand(const AST::Node *node)
    : Exception(node)
  {
  }

  Operand::~Operand() = default;

  const char *Operand::what() const noexcept
  {
    return "operand";
  }

  ////// OperandType /////////////////////////////////////////////////////////

  OperandType::OperandType(const AST::Node *node)
    : Exception(node)
  {
  }

  OperandType::~OperandType() = default;

  const char *OperandType::what() const noexcept
  {
    return "operand type";
  }

  ////// UnknownFunction /////////////////////////////////////////////////////

  UnknownFunction::UnknownFunction(const AST::Node *node)
    : Exception(node)
  {
  }

  UnknownFunction::~UnknownFunction() = default;

  const char *UnknownFunction::what() const noexcept
  {
    return "unknown function";
  }

  ////// UnknownVariable /////////////////////////////////////////////////////

  UnknownVariable::UnknownVariable(const AST::Node *node)
    : Exception(node)
  {
  }

  UnknownVariable::~UnknownVariable() = default;

  const char *UnknownVariable::what() const noexcept
  {
    return "unknown variable";
  }

} // namespace eval
