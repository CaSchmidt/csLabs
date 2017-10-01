/****************************************************************************
** Copyright (c) 2012,2016, Carsten Schmidt. All rights reserved.
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

#include <cstdint>

#include "SimCore/SimUtil.h"

namespace sim {

  SIM_CORE_EXPORT double bound(const double value, const SimDataType type)
  {
    if(        type == DoubleType ) {
      return qBound(std::numeric_limits<double>::lowest(),
                    value,
                    std::numeric_limits<double>::max());
    } else if( type == SingleType ) {
      return qBound(std::numeric_limits<float>::lowest(),
                    (float)value,
                    std::numeric_limits<float>::max());
    } else if( type == IntType ) {
      return qBound(std::numeric_limits<int32_t>::lowest(),
                    (int32_t)value,
                    std::numeric_limits<int32_t>::max());
    } else if( type == UIntType  ) {
      return qBound(std::numeric_limits<uint32_t>::lowest(),
                    (uint32_t)value,
                    std::numeric_limits<uint32_t>::max());
    }
    return 0;
  }

  SIM_CORE_EXPORT QLocale locale()
  {
    QLocale locale(QLocale::c());
    locale.setNumberOptions(QLocale::OmitGroupSeparator | QLocale::RejectGroupSeparator);
    return locale;
  }

  SIM_CORE_EXPORT SimDataType toDataType(const QString& string)
  {
    if(        string == QStringLiteral("Double") ) {
      return DoubleType;
    } else if( string == QStringLiteral("Single") ) {
      return SingleType;
    } else if( string == QStringLiteral("Int") ) {
      return IntType;
    } else if( string == QStringLiteral("UInt") ) {
      return UIntType;
    }
    return InvalidDataType;
  }

  SIM_CORE_EXPORT QString toString(const SimDataType type)
  {
    if(        type == DoubleType ) {
      return QStringLiteral("Double");
    } else if( type == SingleType ) {
      return QStringLiteral("Single");
    } else if( type == IntType ) {
      return QStringLiteral("Int");
    } else if( type == UIntType ) {
      return QStringLiteral("UInt");
    }
    return QStringLiteral("Invalid");
  }

  SIM_CORE_EXPORT SimOperationMode toOperationMode(const QString& string)
  {
    if(        string == QStringLiteral("Offline") ) {
      return OfflineMode;
    } else if( string == QStringLiteral("RealTime") ) {
      return RealTimeMode;
    }
    return InvalidOperationMode;
  }

  SIM_CORE_EXPORT QString toString(const SimOperationMode mode)
  {
    if(        mode == OfflineMode ) {
      return QStringLiteral("Offline");
    } else if( mode == RealTimeMode ) {
      return QStringLiteral("RealTime");
    }
    return QStringLiteral("Invalid");
  }

} // namespace sim
