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

#include <algorithm>

#include "ast/printer.h"

namespace AST {

  ////// public //////////////////////////////////////////////////////////////

  Printer::Printer(const int indent)
    : _indent(indent)
    , _out()
  {
    _indent = std::max(_indent, 1);
  }

  Printer::~Printer()
  {
  }

  std::string Printer::print(const Node *node, int col, const AST::Node *caret)
  {
    col = std::max(col, 0);
    _out.str(std::string());
    dispatch(node, col, caret);

    return _out.str();
  }

  void Printer::setFloatFlags(const std::ios_base::fmtflags flags)
  {
    _out.setf(flags, std::ios_base::floatfield);
  }

  void Printer::setFloatPrecision(const std::streamsize precision)
  {
    _out.precision(std::max(precision, static_cast<std::streamsize>(0)));
  }

  ////// private /////////////////////////////////////////////////////////////

  std::string Printer::caretString(const AST::Node *node, const AST::Node *caret)
  {
    std::string result("");
    if( node == caret ) {
      result = "  <-- ERROR";
    }
    return result;
  }

  void Printer::dispatch(const Node *node, const int col, const AST::Node *caret)
  {
    if(        node->nodeType() == NodeType::Assignment ) {
      print(dynamic_cast<const Assignment*>(node), col, caret);

    } else if( dynamic_cast<const BinaryExpression*>(node) != nullptr ) {
      print(dynamic_cast<const BinaryExpression*>(node), col, caret);

    } else if( node->nodeType() == NodeType::RealConstant ) {
      print(dynamic_cast<const RealConstant*>(node), col, caret);

    } else if( node->nodeType() == NodeType::UIntConstant ) {
      print(dynamic_cast<const UIntConstant*>(node), col, caret);

    } else if( node->nodeType() == NodeType::Expression ) {
      print(dynamic_cast<const Expression*>(node), col, caret);

    } else if( node->nodeType() == NodeType::ExpressionList ) {
      print(dynamic_cast<const ExpressionList*>(node), col, caret);

    } else if( node->nodeType() == NodeType::Function ) {
      print(dynamic_cast<const Function*>(node), col, caret);

    } else if( node->nodeType() == NodeType::Identifier ) {
      print(dynamic_cast<const Identifier*>(node), col, caret);

    } else if( dynamic_cast<const UnaryExpression*>(node) != nullptr ) {
      print(dynamic_cast<const UnaryExpression*>(node), col, caret);

    } else {
      _out << "<INVALID>" << std::endl;

    }
  }

  void Printer::indentation(const int col)
  {
    for(int i = 0; i < col; i++) {
      _out << ' ';
    }
  }

  void Printer::print(const Assignment *node, const int col, const AST::Node *caret)
  {
    indentation(col); _out << node->var()->value() << " (ASSIGNMENT)" << std::endl;
    indentation(col); _out << '=' << std::endl;
    dispatch(node->expr(), col, caret);
  }

  void Printer::print(const BinaryExpression *node, const int col, const AST::Node *caret)
  {
    dispatch(node->lhs(), col + _indent, caret);

    std::string op("<INVALID>");
    const NodeType id = node->nodeType();
    if(        id == NodeType::Addition ) { // Arithmetic ////////////////////
      op = "+";
    } else if( id == NodeType::Division ) {
      op = "/";
    } else if( id == NodeType::Modulo ) {
      op = "%";
    } else if( id == NodeType::Multiplication ) {
      op = "*";
    } else if( id == NodeType::Subtraction ) {
      op = "-";
    } else if( id == NodeType::And ) { // Logical ////////////////////////////
      op = "&&";
    } else if( id == NodeType::Or ) {
      op = "||";
    } else if( id == NodeType::Equal ) { // Relational ///////////////////////
      op = "==";
    } else if( id == NodeType::Greater ) {
      op = ">";
    } else if( id == NodeType::GreaterEqual ) {
      op = ">=";
    } else if( id == NodeType::Less ) {
      op = "<";
    } else if( id == NodeType::LessEqual ) {
      op = "<=";
    } else if( id == NodeType::NotEqual ) {
      op = "!=";
    }

    indentation(col); _out << op << caretString(node, caret) << std::endl;

    dispatch(node->rhs(), col + _indent, caret);
  }

  void Printer::print(const RealConstant *node, const int col, const AST::Node *caret)
  {
    indentation(col); _out << node->value() << " (REAL)" << caretString(node, caret) << std::endl;
  }

  void Printer::print(const UIntConstant *node, const int col, const AST::Node *caret)
  {
    indentation(col); _out << node->value() << " (UINT)" << caretString(node, caret) << std::endl;
  }

  void Printer::print(const Expression *node, const int col, const AST::Node *caret)
  {
    indentation(col); _out << '(' << caretString(node, caret) << std::endl;
    dispatch(node->expr(), col + _indent, caret);
    indentation(col); _out << ')' << std::endl;
  }

  void Printer::print(const ExpressionList *node, const int col, const AST::Node *caret)
  {
    if( node == nullptr  ||  node->list().empty() ) {
      return;
    }

    ExpressionListType::size_type i = 0;
    for(const NodePtr& ptr : node->list()) {
      dispatch(ptr.get(), col + _indent, caret);

      ++i;
      if( i < node->list().size() ) {
        indentation(col); _out << ',' << std::endl;
      }
    }
  }

  void Printer::print(const Function *node, const int col, const AST::Node *caret)
  {
    indentation(col); _out << node->name() << " (FUNCTION)" << caretString(node, caret) << std::endl;
    indentation(col); _out << '(' << std::endl;
    dispatch(node->args(), col, caret);
    indentation(col); _out << ')' << std::endl;
  }

  void Printer::print(const Identifier *node, const int col, const AST::Node *caret)
  {
    indentation(col); _out << node->value() << " (VARIABLE)" << caretString(node, caret) << std::endl;
  }

  void Printer::print(const UnaryExpression *node, const int col, const AST::Node *caret)
  {
    std::string op("<INVALID>");
    const NodeType id = node->nodeType();
    if(        id == NodeType::Minus ) {
      op = "-";
    } else if( id == NodeType::Negation ) {
      op = "!";
    } else if( id == NodeType::Plus ) {
      op = "+";
    }

    indentation(col); _out << op << caretString(node, caret) << std::endl;

    dispatch(node->rhs(), col + _indent, caret);
  }

} // namespace AST
