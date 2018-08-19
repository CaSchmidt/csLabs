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

#ifndef AST_H
#define AST_H

#include <list>
#include <memory>

#include "ast/ast_basictypes.h"

namespace AST {

  enum class NodeType {
    Invalid = 0,
    // Primary
    RealConstant,
    UIntConstant,
    Expression,
    ExpressionList,
    Function,
    Identifier,
    // Unary
    Minus,
    Negation,
    Plus,
    // Arithmetic (binary)
    Addition,
    Division,
    Modulo,
    Multiplication,
    Subtraction,
    // Relational (binary)
    Equal,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    NotEqual,
    // Logical (binary)
    And,
    Or,
    // Auxiliary
    Assignment
  };

  class Node {
  public:
    virtual ~Node();

    virtual NodeType nodeType() const = 0;
  };

  using NodePtr = std::unique_ptr<Node>;

  ////// Complex types ///////////////////////////////////////////////////////

  using ExpressionListType = std::list<NodePtr>;

  ////// Primary expressions /////////////////////////////////////////////////

  class RealConstant : public Node {
  public:
    RealConstant(const RealType value);
    ~RealConstant();

    NodeType nodeType() const final;

    RealType value() const;

  private:
    RealType _value;
  };

  class UIntConstant : public Node {
  public:
    UIntConstant(const UIntType value);
    ~UIntConstant();

    NodeType nodeType() const final;

    UIntType value() const;

  private:
    UIntType _value;
  };

  class Expression : public Node {
  public:
    Expression(Node *expr);
    ~Expression();

    NodeType nodeType() const final;

    const Node *expr() const;

  private:
    NodePtr _expr;
  };

  class ExpressionList : public Node {
  public:
    ExpressionList();
    ~ExpressionList();

    NodeType nodeType() const final;

    void append(Node *node);
    const ExpressionListType& list() const;

  private:
    ExpressionListType _list;
  };

  class Function : public Node {
  public:
    Function(class Identifier *name, ExpressionList *args);
    ~Function();

    NodeType nodeType() const final;

    const IdentifierType& name() const;
    ExpressionList *args() const;

  private:
    NodePtr _name;
    NodePtr _args;
  };

  class Identifier : public Node {
  public:
    Identifier(const IdentifierType& value);
    ~Identifier();

    NodeType nodeType() const final;

    const IdentifierType& value() const;

  private:
    IdentifierType _value;
  };

  ////// Unary expressions ///////////////////////////////////////////////////

  class UnaryExpression : public Node {
  public:
    UnaryExpression(Node *rhs);
    ~UnaryExpression();

    const Node *rhs() const;

  private:
    NodePtr _rhs;
  };

  template<NodeType ID>
  class UnaryNode : public UnaryExpression {
  public:
    UnaryNode(Node *rhs)
      : UnaryExpression(rhs)
    {
    }

    ~UnaryNode()
    {
    }

    NodeType nodeType() const final
    {
      return ID;
    }
  };

  using    Minus = UnaryNode<NodeType::Minus>;
  using Negation = UnaryNode<NodeType::Negation>;
  using     Plus = UnaryNode<NodeType::Plus>;

  ////// Binary expressions //////////////////////////////////////////////////

  class BinaryExpression : public Node {
  public:
    BinaryExpression(Node *lhs, Node *rhs);
    ~BinaryExpression();

    const Node *lhs() const;
    const Node *rhs() const;

  private:
    NodePtr _lhs;
    NodePtr _rhs;
  };

  ////// Arithmetic expressions //////////////////////////////////////////////

  template<NodeType ID>
  class ArithmeticNode : public BinaryExpression {
  public:
    ArithmeticNode(Node *lhs, Node *rhs)
      : BinaryExpression(lhs, rhs)
    {
    }

    ~ArithmeticNode()
    {
    }

    NodeType nodeType() const final
    {
      return ID;
    }
  };

  using       Addition = ArithmeticNode<NodeType::Addition>;
  using       Division = ArithmeticNode<NodeType::Division>;
  using         Modulo = ArithmeticNode<NodeType::Modulo>;
  using Multiplication = ArithmeticNode<NodeType::Multiplication>;
  using    Subtraction = ArithmeticNode<NodeType::Subtraction>;

  ////// Relational expressions //////////////////////////////////////////////

  template<NodeType ID>
  class RelationalNode : public BinaryExpression {
  public:
    RelationalNode(Node *lhs, Node *rhs)
      : BinaryExpression(lhs, rhs)
    {
    }

    ~RelationalNode()
    {
    }

    NodeType nodeType() const final
    {
      return ID;
    }
  };

  using        Equal = RelationalNode<NodeType::Equal>;
  using      Greater = RelationalNode<NodeType::Greater>;
  using GreaterEqual = RelationalNode<NodeType::GreaterEqual>;
  using         Less = RelationalNode<NodeType::Less>;
  using    LessEqual = RelationalNode<NodeType::LessEqual>;
  using     NotEqual = RelationalNode<NodeType::NotEqual>;

  ////// Logical expressions /////////////////////////////////////////////////

  template<NodeType ID>
  class LogicalNode : public BinaryExpression {
  public:
    LogicalNode(Node *lhs, Node *rhs)
      : BinaryExpression(lhs, rhs)
    {
    }

    ~LogicalNode()
    {
    }

    NodeType nodeType() const final
    {
      return ID;
    }
  };

  using And = LogicalNode<NodeType::And>;
  using  Or = LogicalNode<NodeType::Or>;

  ////// Auxiliary ///////////////////////////////////////////////////////////

  class Assignment : public Node {
  public:
    Assignment(Identifier *var, Node *expr);
    ~Assignment();

    NodeType nodeType() const final;

    const Identifier *var() const;
    const Node *expr() const;

  private:
    NodePtr _var;
    NodePtr _expr;
  };

} // namespace AST

#endif // AST_H
