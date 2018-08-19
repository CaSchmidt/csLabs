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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "ast/parser.h"
#include "eval/engine.h"
#include "eval/variables.h"

class csILogger;

namespace eval {

  class Environment {
  public:
    Environment(const index_type dim, csILogger *logger);
    virtual ~Environment();

    index_type dimension() const;
    void setDimension(const index_type dim);

    void evaluate(const std::string& input);

    std::string print(const AST::IdentifierType& name) const;

    void setFloatFlags(const std::ios_base::fmtflags flags);
    void setFloatPrecision(const std::streamsize precision);

    const Variables& variables() const;

  protected:
    virtual void on_EnvironmentChange();
    virtual void on_VariableChange(const AST::IdentifierType& name);

  private:
    std::string print(const Result *result) const;

    index_type _dimension;
    Engine _engine;
    std::ios_base::fmtflags _fpFlags;
    std::streamsize _fpPrecision;
    csILogger *_logger;
    AST::Parser _parser;
    Variables _variables;
  };

} // namespace eval

#endif // ENVIRONMENT_H
