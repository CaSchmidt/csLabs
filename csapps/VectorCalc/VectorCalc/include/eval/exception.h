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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

namespace AST {
  class Node;
}

namespace eval {

  class Exception : public std::exception {
  public:
    Exception(const AST::Node *node);
    ~Exception();

    const AST::Node *node() const;

  private:
    AST::Node *_node;
  };

  ////// The Exceptions... ///////////////////////////////////////////////////

  class ArgumentType : public Exception {
  public:
    ArgumentType(const AST::Node *node);
    ~ArgumentType();

    const char *what() const noexcept final;
  };

  class ConstantRange : public Exception {
  public:
    ConstantRange(const AST::Node *node);
    ~ConstantRange();

    const char *what() const noexcept final;
  };

  class NumberOfArguments : public Exception {
  public:
    NumberOfArguments(const AST::Node *node);
    ~NumberOfArguments();

    const char *what() const noexcept final;
  };

  class Operand : public Exception {
  public:
    Operand(const AST::Node *node);
    ~Operand();

    const char *what() const noexcept final;
  };

  class OperandType : public Exception {
  public:
    OperandType(const AST::Node *node);
    ~OperandType();

    const char *what() const noexcept final;
  };

  class UnknownFunction : public Exception {
  public:
    UnknownFunction(const AST::Node *node);
    ~UnknownFunction();

    const char *what() const noexcept final;
  };

  class UnknownVariable : public Exception {
  public:
    UnknownVariable(const AST::Node *node);
    ~UnknownVariable();

    const char *what() const noexcept final;
  };

} // namespace eval

#endif // EXCEPTION_H
