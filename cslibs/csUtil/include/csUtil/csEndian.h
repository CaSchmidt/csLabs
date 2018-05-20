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

#ifndef CSENDIAN_H
#define CSENDIAN_H

#include <type_traits>

namespace cs {

  namespace endian {

    ////// Swap Meta Program /////////////////////////////////////////////////

    template<int i, int COUNT>
    struct swap {
      inline static void run(unsigned char *dest, const unsigned char *src)
      {
        dest[COUNT-1-i] = src[i];
        swap<i-1,COUNT>::run(dest, src);
      }
    };

    template<int COUNT>
    struct swap<0,COUNT> {
      inline static void run(unsigned char *dest, const unsigned char *src)
      {
        dest[COUNT-1] = src[0];
      }
    };

    ////// Dispatch //////////////////////////////////////////////////////////

    template<bool SWAP, typename T>
    struct dispatch {
      inline static void run(T *dest, const void *src)
      {
        swap<sizeof(T)-1,sizeof(T)>::run(reinterpret_cast<unsigned char*>(dest),
                                         reinterpret_cast<const unsigned char *>(src));
      }
    };

    template<typename T>
    struct dispatch<false,T> {
      inline static void run(T *dest, const void *src)
      {
        *dest = *reinterpret_cast<const T*>(src);
      }
    };

  } // namespace endian

} // namespace cs

////// User Interface ////////////////////////////////////////////////////////

template<bool SWAP, typename T>
inline void csCopy(T *dest, const void *src,
                   typename std::enable_if<std::is_arithmetic<T>::value>::type * = 0)
{
  using namespace cs::endian;
  dispatch<SWAP,T>::run(dest, src);
}

#endif // CSENDIAN_H
