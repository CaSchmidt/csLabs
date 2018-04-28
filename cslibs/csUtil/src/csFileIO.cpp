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

#include <fstream>
#include <type_traits>

#include "csUtil/csFileIO.h"

#include "internal/csutil_internal.h"
#include "csUtil/csTextConverter.h"

////// Private ///////////////////////////////////////////////////////////////

namespace impl {

  template<typename T>
  struct is_compatible {
    enum : bool {
      value = sizeof(std::fstream::char_type) == sizeof(typename T::value_type)
    };
  };

  template<typename T>
  inline T read(const std::string& filename_utf8, bool *ok,
                typename std::enable_if<is_compatible<T>::value>::type * = nullptr)
  {
    using DataT = typename T::value_type;
    T result;

    if( ok != nullptr ) {
      *ok = false;
    }

    try {
      const std::ios_base::openmode mode = std::ios_base::in | std::ios_base::binary;
      std::fstream file;
#if defined(_MSC_VER) && defined(_WIN32)
      const std::u16string filename_utf16 = csUtf8ToUnicode(filename_utf8.data());
      file.open(reinterpret_cast<const wchar_t*>(filename_utf16.data()), mode);
#else
      file.open(filename_utf8, mode);
#endif
      if( !file.is_open() ) {
        return result;
      }

      file.seekg(0, std::ios_base::end);
      const auto size = file.tellg();
      file.seekg(0);

      result.resize(size);

      DataT *dest = const_cast<DataT*>(result.data()); // C++17
      file.read(reinterpret_cast<char*>(dest), size);
    } catch(...) {
      result.clear();
      return result;
    }

    if( ok != nullptr ) {
      *ok = true;
    }

    return result;
  }

} // namespace impl

////// Public ////////////////////////////////////////////////////////////////

CS_UTIL_EXPORT std::string csReadTextFile(const std::string& filename_utf8, bool *ok)
{
  return impl::read<std::string>(filename_utf8, ok);
}

CS_UTIL_EXPORT std::vector<uint8_t> csReadBinaryFile(const std::string& filename_utf8, bool *ok)
{
  return impl::read<std::vector<uint8_t>>(filename_utf8, ok);
}
