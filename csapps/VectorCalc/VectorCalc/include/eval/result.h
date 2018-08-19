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

#ifndef RESULT_H
#define RESULT_H

#include <list>
#include <memory>
#include <type_traits>

#include <Eigen/Dense>

#include "ast/ast_basictypes.h"

namespace eval {

  using index_type = Eigen::Index;

  using BoolVector = Eigen::Array<AST::BoolType,Eigen::Dynamic,1>;
  using RealVector = Eigen::Array<AST::RealType,Eigen::Dynamic,1>;

  enum class ResultContext {
    Null = 0,
    List,
    Scalar,
    Vector
  };

  enum class ResultType {
    Void = 0,
    Any,
    Bool,
    Real
  };

  ////// Base ////////////////////////////////////////////////////////////////

  class Result {
  public:
    virtual ~Result();

    virtual ResultContext context() const = 0;
    virtual ResultType type() const = 0;

    virtual Result *copy() const = 0;
  };

  using ResultPtr = std::unique_ptr<Result>;

  ////// Implementation //////////////////////////////////////////////////////

  template<typename T, ResultContext CID, ResultType TID>
  class ResultImpl : public Result {
  public:
    explicit ResultImpl(const T& value)
      : _value(value)
    {
    }

    ~ResultImpl()
    {
    }

    ResultContext context() const final
    {
      return CID;
    }

    ResultType type() const final
    {
      return TID;
    }

    Result *copy() const final
    {
      return new ResultImpl<T,CID,TID>(_value);
    }

    const T& value() const
    {
      return _value;
    }

    ResultImpl<T,CID,TID>& operator=(const T& value)
    {
      _value = value;
      return *this;
    }

  private:
    T _value;
  };

  using ScalarBoolResult = ResultImpl<AST::BoolType,ResultContext::Scalar,ResultType::Bool>;
  using VectorBoolResult = ResultImpl<BoolVector,ResultContext::Vector,ResultType::Bool>;
  using ScalarRealResult = ResultImpl<AST::RealType,ResultContext::Scalar,ResultType::Real>;
  using VectorRealResult = ResultImpl<RealVector,ResultContext::Vector,ResultType::Real>;

  // List of Result //////////////////////////////////////////////////////////

  class ResultList : public Result {
  public:
    ResultList(std::list<ResultPtr>& list);
    ~ResultList();

    ResultContext context() const final;

    ResultType type() const final;

    Result *copy() const final;

    const std::list<ResultPtr>& list() const;

  private:
    std::list<ResultPtr> _list;
  };

} // namespace eval

#endif // RESULT_H
