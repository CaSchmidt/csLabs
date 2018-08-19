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
#include <sstream>

#include <csUtil/csILogger.h>

#include "eval/environment.h"

#include "ast/printer.h"
#include "eval/exception.h"

namespace eval {

  ////// public //////////////////////////////////////////////////////////////

  Environment::Environment(const index_type dim, csILogger *logger)
    : _dimension()
    , _engine(this)
    , _fpFlags(0)
    , _fpPrecision(3)
    , _logger(logger)
    , _parser(logger)
    , _variables()
  {
    setDimension(dim);
  }

  Environment::~Environment() = default;

  index_type Environment::dimension() const
  {
    return _dimension;
  }

  void Environment::setDimension(const index_type dim)
  {
    const index_type old = _dimension;
    _dimension = std::max(dim, static_cast<index_type>(2));
    if( _dimension != old ) {
      _variables.clear();
      on_EnvironmentChange();
    }
  }

  void Environment::evaluate(const std::string& input)
  {
    AST::NodePtr ast;
    AST::Printer printer;
    printer.setFloatFlags(_fpFlags);
    printer.setFloatPrecision(_fpPrecision);

    try {
      // (1) Parse ///////////////////////////////////////////////////////////

      ast = _parser.parse(input);
      if( !ast ) {
        return;
      }

      // (2) Show input //////////////////////////////////////////////////////

      _logger->logText("<<< " + input);

      // (3) Evaluate input //////////////////////////////////////////////////

      ResultPtr result = _engine.dispatch(ast.get());

      // (4) Show result /////////////////////////////////////////////////////

      const AST::Assignment *assignment = dynamic_cast<const AST::Assignment*>(ast.get());

      std::ostringstream out;
      out << ">>>";
      if( assignment != nullptr ) {
        out << ' ' << assignment->var()->value() << " =";
      }
      out << std::endl << print(result.get()) << std::endl;
      _logger->logText(out.str());

      // (5) Store result ////////////////////////////////////////////////////

      if( assignment != nullptr ) {
        _variables.assign(assignment->var()->value(), result);
        on_VariableChange(assignment->var()->value());
      }

    } catch( const Exception& e ) {
      std::ostringstream out;
      out << e.what() << std::endl;
      out << printer.print(ast.get(), 0, e.node()) << std::endl;
      _logger->logError(out.str());

    } catch( const std::exception& e ) {
      _logger->logError(e.what());

    } catch( ... ) {
      _logger->logError("unknown exception");

    }
  }

  std::string Environment::print(const AST::IdentifierType& name) const
  {
    const Result *value = _variables.value(name);
    return print(value);
  }

  void Environment::setFloatFlags(const std::ios_base::fmtflags flags)
  {
    _fpFlags = flags & std::ios_base::floatfield;
    on_EnvironmentChange();
  }

  void Environment::setFloatPrecision(const std::streamsize precision)
  {
    _fpPrecision = std::max(precision, static_cast<std::streamsize>(0));
    on_EnvironmentChange();
  }

  const Variables& Environment::variables() const
  {
    return _variables;
  }

  ////// protected ///////////////////////////////////////////////////////////

  void Environment::on_EnvironmentChange()
  {
  }

  void Environment::on_VariableChange(const AST::IdentifierType& /*name*/)
  {
  }

  ////// private /////////////////////////////////////////////////////////////

  std::string Environment::print(const Result *result) const
  {
    std::ostringstream out;
    out.setf(std::ios_base::boolalpha, std::ios_base::boolalpha);
    out.setf(_fpFlags, std::ios_base::floatfield);
    out.precision(_fpPrecision);

    if(        dynamic_cast<const ScalarBoolResult*>(result) != nullptr ) {
      out << dynamic_cast<const ScalarBoolResult*>(result)->value();
    } else if( dynamic_cast<const VectorBoolResult*>(result) != nullptr ) {
      out << dynamic_cast<const VectorBoolResult*>(result)->value();
    } else if( dynamic_cast<const ScalarRealResult*>(result) != nullptr ) {
      out << dynamic_cast<const ScalarRealResult*>(result)->value();
    } else if( dynamic_cast<const VectorRealResult*>(result) != nullptr ) {
      out << dynamic_cast<const VectorRealResult*>(result)->value();
    }

    return out.str();
  }

} // namespace eval
