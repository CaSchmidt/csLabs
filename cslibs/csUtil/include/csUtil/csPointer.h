/****************************************************************************
** Copyright (c) 2016, Carsten Schmidt. All rights reserved.
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

#ifndef __CSPOINTER_H__
#define __CSPOINTER_H__

#include <cstdint>

namespace cs {

  namespace pointer {

    template<int Size>
    struct csPointerTypeImpl {
      // SFINAE
    };

    template<>
    struct csPointerTypeImpl<4> {
      typedef uint32_t type;
    };

    template<>
    struct csPointerTypeImpl<8> {
      typedef uint64_t type;
    };

  } // namespace pointer

} // namespace cs

struct csPointer {
  typedef cs::pointer::csPointerTypeImpl<sizeof(void*)>::type type;

  template<typename DataT>
  inline static bool isAlignedTo(const void *p)
  {
    return ( (type)p & (type)(sizeof(DataT)-1) ) == 0;
  }

  template<typename DataT>
  inline static void *alignTo(const void *p)
  {
    return (void*)( (type)p & ~(type)(sizeof(DataT)-1) );
  }
};

#endif // __CSPOINTER_H__
