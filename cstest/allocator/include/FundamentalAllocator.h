/****************************************************************************
** Copyright (c) 2021, Carsten Schmidt. All rights reserved.
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

#ifndef FUNDAMENTALALLOCATOR_H
#define FUNDAMENTALALLOCATOR_H

#include <cstddef>
#include <cstdint>

#include <new>
#include <type_traits>

#define HAVE_BOILERPLATE
#define HAVE_PRINT

// Cf. https://www.grimm-jaud.de/index.php/35-blog/c/170-memory-allocation-mit-std-allocate

template<typename T, std::size_t COUNT, bool INIT_TO_ZERO = false>
class FundementalAllocator {
private:
  static_assert( std::is_fundamental_v<T> );
  static_assert( 0 < COUNT  &&  COUNT <= 1024 );

public:
  using difference_type = std::ptrdiff_t;
  using       size_type = std::size_t;
  using      value_type = T;

#ifdef HAVE_BOILERPLATE
  template<typename U>
  struct rebind {
    using other = FundementalAllocator<U,COUNT,INIT_TO_ZERO>;
  };
#endif

  FundementalAllocator() noexcept
  {
    for(size_type i = 0; i < NUM_SLOT; i++) {
      _slot[i] = 0;
    }

#ifdef HAVE_PRINT
    printIndex("ctor");
#endif
  }

  FundementalAllocator(const FundementalAllocator& other) noexcept
  {
    for(size_type i = 0; i < NUM_DATA; i++) {
      _data[i] = other._data[i];
    }
    for(size_type i = 0; i < NUM_SLOT; i++) {
      _slot[i] = other._slot[i];
    }
  }

  ~FundementalAllocator() noexcept = default;

  static constexpr size_type capacity()
  {
    return NUM_DATA;
  }

  value_type *allocate(const size_type n, const void * = nullptr)
  {
    if( n < 1  ||  n > NUM_DATA ) {
      throw std::bad_alloc();
    }

    size_type avail = 0;
    size_type begin = 0;
    for(size_type i = 0; i <= NUM_DATA - n; i++) {
      avail = 0;

      if( !isFree(i) ) {
        continue;
      }
      avail++;
      begin = i;

      for(; avail < n; avail++) {
        if( !isFree(i + avail) ) {
          break;
        }
      }

      if( avail == n ) {
        break;
      }
    }

    if( avail != n ) {
      throw std::bad_alloc();
    }

    for(size_type i = 0; i < n; i++) {
      markSlot(begin + i);
      if constexpr( INIT_TO_ZERO ) {
        _data[begin + i] = 0;
      }
    }

#ifdef HAVE_PRINT
    printIndex("allocate");
#endif

    return &_data[begin];
  }

  void deallocate(value_type *p, const size_type n)
  {
    if( p == nullptr  ||  n < 1  ||  n > NUM_DATA ) {
      return;
    }

    const difference_type diff = p - &_data[0]; // CAUTION!
    if( diff < 0 ) {
      return;
    }

    const size_type begin = size_type(diff);
    if( begin >= NUM_DATA  ||  n > NUM_DATA - begin ) {
      return;
    }

    for(size_type i = 0; i < n; i++) {
      markSlot(begin + i, false);
    }

#ifdef HAVE_PRINT
    printIndex("deallocate");
#endif
  }

private:
  FundementalAllocator& operator=(const FundementalAllocator&) noexcept = delete;

  FundementalAllocator(FundementalAllocator&&) noexcept = delete;
  FundementalAllocator& operator=(FundementalAllocator&&) noexcept = delete;

  static constexpr size_type nextX8(const size_type x)
  {
    return (x + 7)/8;
  }

  inline bool isFree(const size_type i) const
  {
    return (_slot[i >> 3] & makeMask(i)) == 0;
  }

  inline static uint8_t makeMask(const size_type i)
  {
    return (uint8_t(1) << (i & 7));
  }

  inline void markSlot(const size_type i, const bool on = true)
  {
    if( on ) {
      _slot[i >> 3] |= makeMask(i);
    } else {
      _slot[i >> 3] &= ~makeMask(i);
    }
  }

#ifdef HAVE_PRINT
  inline void printIndex(const char *reason) const
  {
    printf("index(%s) =", reason);
    for(size_type i = 0; i < NUM_SLOT; i++) {
      printf(" %02X", _slot[i]);
    }
    printf("\n"); fflush(stdout);
  }
#endif

  static constexpr size_type NUM_DATA = COUNT;
  static constexpr size_type NUM_SLOT = nextX8(COUNT);

  value_type _data[NUM_DATA];
  uint8_t    _slot[NUM_SLOT];
};

#endif // FUNDAMENTALALLOCATOR_H
