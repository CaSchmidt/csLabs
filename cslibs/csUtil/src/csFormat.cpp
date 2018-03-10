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

#include <array>
#include <list>

#include "csUtil/csFormat.h"

#include "csUtil/csCharConv.h"

namespace cs {

  ////// Implementation //////////////////////////////////////////////////////

  namespace format {

    template<typename CharT>
    struct Options {
      Options()
        : base(10)
        , fill(CharT(' '))
        , format(CharT('g'))
        , precision(6)
        , width(0)
      {
      }

      int   base;
      CharT fill;
      CharT format;
      int   precision;
      int   width;
    };

    template<typename CharT>
    class Formatter {
    public:
      typedef unsigned int                       index_type;
      typedef std::basic_string<CharT>          string_type;
      typedef typename string_type::size_type     size_type;
      typedef std::pair<index_type,string_type>  token_type;
      typedef std::list<token_type>             tokens_type;

      Formatter(const CharT *begin, const CharT *end)
        : _options()
        , _temp()
        , _tokens()
      {
        tokenize(begin, end);
      }

      ~Formatter()
      {
      }

      void arg(const std::basic_string<CharT>& s,
               const int width, const CharT fill)
      {
        replace_index(s.data(), s.data() + s.size(), width, fill);
      }

      void arg(const CharT *s,
               const int width, const CharT fill)
      {
        const size_type len = length(s);
        replace_index(s, s + len, width, fill);
      }

      template<typename ValueT>
      void arg(const ValueT value,
               const int width, const int base, const CharT fill,
               typename std::enable_if<
               std::is_integral<ValueT>::value
               >::type * = nullptr)
      {
        _temp.fill(0);
        const cs::to_chars_result res =
            cs::to_chars(_temp.data(), _temp.data() + _temp.size(), value, base);
        if( res.ec != std::errc() ) {
          return;
        }
        const size_type len = length(_temp.data(), res.ptr);
        replace_index(_temp.data(), _temp.data() + len, width, fill);
      }

      template<typename ValueT>
      void arg(const ValueT value,
               const int width, const CharT format, const int precision, const CharT fill,
               typename std::enable_if<
               std::is_floating_point<ValueT>::value
               >::type * = nullptr)
      {
        _temp.fill(0);
        cs::chars_format fmt;
        if(        format == CharT('e')  ||  format == CharT('E') ) {
          fmt = cs::chars_format::scientific;
        } else if( format == CharT('f')  ||  format == CharT('F') ) {
          fmt = cs::chars_format::fixed;
        } else { // Default
          fmt = cs::chars_format::general;
        }
        const cs::to_chars_result res =
            cs::to_chars(_temp.data(), _temp.data() + _temp.size(), value, fmt, precision);
        if( res.ec != std::errc() ) {
          return;
        }
        const size_type len = length(_temp.data(), res.ptr);
        replace_index(_temp.data(), _temp.data() + len, width, fill);
      }

      static inline size_type length(const CharT *s)
      {
        size_type result = 0;
        if( s == nullptr ) {
          return result;
        }
        while( *s++ != '\0' ) {
          result++;
        }
        return result;
      }

      inline const Options<CharT>& options() const
      {
        return _options;
      }

      string_type yield() const
      {
        size_type len = 0;
        for(const token_type& token : _tokens) {
          len += token.second.size();
        }

        string_type result;
        result.reserve(len);
        for(const token_type& token : _tokens) {
          result += token.second;
        }

        return result;
      }

    private:
      Formatter() = delete;
      Formatter(const Formatter<CharT>& other) = delete;
      Formatter& operator=(const Formatter<CharT>& other) = delete;
      Formatter(Formatter<CharT>&& other) = delete;
      Formatter& operator=(Formatter<CharT>&& other) = delete;

      Options<CharT> _options;
      std::array<CharT,128> _temp;
      tokens_type _tokens;

      // NOTE: Processes [begin,end)
      void tokenize(const CharT *begin, const CharT *end)
      {
        size_type count = 0;
        const CharT *copy = begin;
        const CharT  *pos = begin;
        while( pos < end ) {

          // (1) Handle '%' character ////////////////////////////////////////////

          if( *pos == CharT('%')  &&  pos + 1 < end ) {
            if( CharT('1') <= pos[1]  &&  pos[1] <= CharT('9') ) {
              append_token(copy, pos, count);

              index_type index = 0;
              const cs::from_chars_result res =
                  cs::from_chars(pos+1, end, index, 10);

              if( res.ec == std::errc() ) {
                _tokens.emplace_back(index, std::string());
                pos = res.ptr;
              } else {
                pos++; // Skip leading '%'
              }

              copy = pos;
              count = 0;

            } else if( pos[1] == CharT('%') ) { // Escape literal '%'
              count++; // Count 2nd character; literal '%'
              pos += 2;

            } else { // Take following character as a literal
              count += 2; // Count 1st & 2nd character; e.g. "%x"
              pos += 2;

            }

            // (2) Handle all other characters /////////////////////////////////////

          } else {
            count++;
            pos++;

          }

          // (3) Copy stale token ////////////////////////////////////////////////

          if( pos >= end ) {
            append_token(copy, end, count);
          }

        } // For Each Character
      }

      // NOTE: Processes [begin,end)
      const CharT *append_token(const CharT *begin, const CharT *end,
                                const size_type length)
      {
        if( length < 1  ||  begin >= end ) {
          return begin;
        }

        string_type tokstr(length, 0);

        size_type count = 0;
        CharT *dest = const_cast<CharT*>(tokstr.data());
        while( begin < end  &&  count < length ) {
          if( *begin == CharT('%')  &&  begin + 1 < end ) {
            if( begin[1] == CharT('%') ) {
              begin++;  // skip 1st character
            } else {
              copy(dest, begin, count); // copy 1st character
            }
            copy(dest, begin, count); // copy 2nd character

          } else {
            copy(dest, begin, count);

          }
        } // For Each Character

        _tokens.emplace_back(0, std::move(tokstr));

        return begin;
      }

      static inline void copy(CharT* &dest, const CharT* &src,
                              size_type& count)
      {
        count++;
        *dest++ = *src++;
      }

      // NOTE: Processes [begin,end)
      static inline size_type length(const CharT *begin,
                                     const CharT *end)
      {
        if( end <= begin ) {
          return 0;
        }
        const CharT *last = end - 1;
        return *last == CharT('\0') // Interval includes '\0'?
            ? static_cast<size_type>(last - begin)
            : static_cast<size_type>(end - begin);
      }

      index_type next_index() const
      {
        index_type result = 0;
        for(const token_type& token : _tokens) {
          if( token.first == 0 ) {
            continue;
          }

          if( result == 0  ||  token.first < result ) {
            result = token.first;
          }
        } // For Each Token
        return result;
      }

      // NOTE: Processes [begin,end)
      void replace_index(const CharT *begin, const CharT *end,
                         const int width, const CharT fill)
      {
        if( end <= begin ) {
          return;
        }

        const index_type index = next_index();
        if( index == 0 ) {
          return;
        }

        const size_type   length = static_cast<size_type>(end - begin);
        const size_type abswidth = static_cast<size_type>(width < 0
                                                          ? -width
                                                          :  width);

        std::basic_string<CharT> repstr(std::max(abswidth, length), fill);
        if( length >= abswidth  ||  width <= 0 ) {
          repstr.replace(0, length, begin, length);
        } else {
          repstr.replace(abswidth - length, length, begin, length);
        }

        for(token_type& token : _tokens) {
          if( token.first != index ) {
            continue;
          }

          token.first  = 0;
          token.second = std::move(repstr);
        }
      }
    };

  } // namespace format

} // namespace cs

////// User Interface //////////////////////////////////////////////////////

csFormat::csFormat(const std::string& s)
  : impl()
{
  try {
    impl = std::make_unique<Formatter>(s.data(), s.data() + s.size());
  } catch(...) {
    impl.reset(nullptr);
  }
}

csFormat::csFormat(const char *s)
  : impl()
{
  try {
    impl = std::make_unique<Formatter>(s, s + Formatter::length(s));
  } catch(...) {
    impl.reset(nullptr);
  }
}

csFormat::~csFormat()
{
}

csFormat::operator std::string() const
{
  return impl->yield();
}

csFormat& csFormat::arg(const std::string& s,
                        const int width, const char fill)
{
  impl->arg(s, width, fill);
  return *this;
}

csFormat& csFormat::arg(const char *s,
                        const int width, const char fill)
{
  impl->arg(s, width, fill);
  return *this;
}

csFormat& csFormat::arg(const int8_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const uint8_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const int16_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const uint16_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const int32_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const uint32_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const int64_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const uint64_t value,
                        const int width, const int base, const char fill)
{
  impl->arg(value, width, base, fill);
  return *this;
}

csFormat& csFormat::arg(const float value,
                        const int width, const char format, const int precision, const char fill)
{
  impl->arg(value, width, format, precision, fill);
  return *this;
}

csFormat& csFormat::arg(const double value,
                        const int width, const char format, const int precision, const char fill)
{
  impl->arg(value, width, format, precision, fill);
  return *this;
}

csFormat& csFormat::operator%(const std::string& s)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(s, opt.width, opt.fill);
}

csFormat& csFormat::operator%(const char *s)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(s, opt.width, opt.fill);
}

csFormat& csFormat::operator%(const int8_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const uint8_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const int16_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const uint16_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const int32_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const uint32_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const int64_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const uint64_t value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.base, opt.fill);
}

csFormat& csFormat::operator%(const float value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.format, opt.precision, opt.fill);
}

csFormat& csFormat::operator%(const double value)
{
  const cs::format::Options<char>& opt = impl->options();
  return arg(value, opt.width, opt.format, opt.precision, opt.fill);
}
