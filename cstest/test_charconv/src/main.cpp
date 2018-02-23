#include <cstdio>
#include <cstdlib>

#include <array>
#include <iostream>
#include <string>

#include <csUtil/csCharConv.h>

// from_chars() //////////////////////////////////////////////////////////////

template<typename ValueT>
void print_from(const char *first, const char *last, const int base = 10)
{
  ValueT value = 0;
  const cs::from_chars_result res =
      cs::from_chars(first, last, value, base);

  printf("STR: %4s -> VAL: %4d, RES: %d\n", first, value, res.ec);
}

void test_to_unsigned()
{
  std::array<char,10> s;
  for(int i = 0; i <= 266; i++) {
    snprintf(s.data(), s.size(), "%d", i);
    print_from<uint8_t>(s.data(), s.data()+s.size());
  }
}

void test_to_signed()
{
  std::array<char,10> s;
  for(int i = -139; i <= 138; i++) {
    snprintf(s.data(), s.size(), "%d", i);
    print_from<int8_t>(s.data(), s.data()+s.size());
  }
}

template<typename ValueT>
void print_from(const char *fmt)
{
  std::array<char,32> s;
  s.fill(0);

  snprintf(s.data(), s.size(), fmt);

  ValueT value = 0;
  const cs::from_chars_result res =
      cs::from_chars(s.data(), s.data()+s.size(), value);

  printf("STR: %s -> VAL: %g, RES: %d\n", s.data(), value, res.ec);
}

template<typename ValueT>
void test_to_fp()
{
  printf("*** IEEE-754 binary%d **********\n", static_cast<int>(sizeof(ValueT)*8));
  print_from<ValueT>("inf");
  print_from<ValueT>("+inf");
  print_from<ValueT>("-inf");
  print_from<ValueT>("nan");
  print_from<ValueT>("+nan");
  print_from<ValueT>("-nan");
  print_from<ValueT>("z42");
  print_from<ValueT>("42");
  print_from<ValueT>("+42");
  print_from<ValueT>("-42");
  print_from<ValueT>("1.0000000001");
  print_from<ValueT>("2.9999999999");
  print_from<ValueT>("0.0001");
  print_from<ValueT>("0.00001");
  print_from<ValueT>("0.0001234");
  print_from<ValueT>("0.00001234");
  print_from<ValueT>("0.0005678");
  print_from<ValueT>("0.00005678");
  print_from<ValueT>("100000.1234");
  print_from<ValueT>("1000000.1234");
  print_from<ValueT>("100000.5678");
  print_from<ValueT>("1000000.5678");
}

// to_chars() ////////////////////////////////////////////////////////////////

template<typename ValueT>
void print_to(char *first, char *last, const ValueT value, const int base = 10)
{
  for(char *ptr = first; ptr < last; ptr++) {
    *ptr = '?';
  }

  cs::to_chars_result res =
      cs::to_chars(first, last, value, base);
  if( res.ptr < last ) {
    *(res.ptr) = '\0';
  }

  printf("VAL: %4d -> STR: %4s, RES: %d\n", value, first, res.ec);
}

void test_from_unsigned()
{
  std::array<char,5> s;
  s.fill(0);
  for(int i = 0; i <= 255; i++) {
    print_to(s.data(), s.data()+s.size()-1, (uint8_t)i);
  }
}

void test_from_signed()
{
  std::array<char,5> s;
  s.fill(0);
  for(int i = -128; i <= 127; i++) {
    print_to(s.data(), s.data()+s.size()-1, (int8_t)i);
  }
}

template<typename ValueT>
void print_to(const ValueT value, const cs::chars_format fmt, const int precision)
{
  std::array<char,128> s;
  s.fill(0);

  const cs::to_chars_result res =
      cs::to_chars(s.data(), s.data()+s.size(), value, fmt, precision);

  char ch_fmt = 'f';
  if(        fmt == cs::chars_format::general ) {
    ch_fmt = 'g';
  } else if( fmt == cs::chars_format::scientific ) {
    ch_fmt = 'e';
  }

  std::array<char,80> fmtstr;
  if( precision > 0 ) {
    snprintf(fmtstr.data(), fmtstr.size(),
             "VAL: %%.%d%c -> STR: %%s, RES: %%d\n", precision, ch_fmt);
  } else {
    snprintf(fmtstr.data(), fmtstr.size(),
             "VAL: %%%c -> STR: %%s, RES: %%d\n", ch_fmt);
  }

  // printf("%s\n", fmtstr.data());
  printf(fmtstr.data(), value, s.data(), res.ec);
}

template<typename ValueT>
void test_from_fp(const cs::chars_format fmt = cs::chars_format::general,
                  const int precision = -1)
{
  print_to(std::numeric_limits<ValueT>::infinity(), fmt, precision);
  print_to(-std::numeric_limits<ValueT>::infinity(), fmt, precision);
  print_to(std::numeric_limits<ValueT>::quiet_NaN(), fmt, precision);
  print_to(static_cast<ValueT>(1), fmt, precision);
  print_to(static_cast<ValueT>(0.1), fmt, precision);
  print_to(static_cast<ValueT>(0.0001), fmt, precision);
  print_to(static_cast<ValueT>(0.00001), fmt, precision);
  print_to(static_cast<ValueT>(0.0001234), fmt, precision);
  print_to(static_cast<ValueT>(0.00001234), fmt, precision);
  print_to(static_cast<ValueT>(0.0005678), fmt, precision);
  print_to(static_cast<ValueT>(0.00005678), fmt, precision);
  print_to(static_cast<ValueT>(0.00012345678), fmt, precision);
  print_to(static_cast<ValueT>(0.000012345678), fmt, precision);
  print_to(static_cast<ValueT>(100000.1234), fmt, precision);
  print_to(static_cast<ValueT>(1000000.1234), fmt, precision);
  print_to(static_cast<ValueT>(100000.5678), fmt, precision);
  print_to(static_cast<ValueT>(1000000.5678), fmt, precision);
}

// Main //////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
  std::array<char,10> s{"z42"};
  print_from<uint8_t>(s.data(), s.data()+s.size());

  s = {"ff"};
  print_from<uint8_t>(s.data(), s.data()+s.size(), 16);

  s = {"FF"};
  print_from<uint8_t>(s.data(), s.data()+s.size(), 16);

  s = {"377"};
  print_from<uint8_t>(s.data(), s.data()+s.size(), 8);

  printf("---------------------\n");

  test_to_unsigned();
  printf("---------------------\n");

  test_to_signed();
  printf("---------------------\n");

  test_from_unsigned();
  printf("---------------------\n");

  test_from_signed();
  printf("---------------------\n");

  test_to_fp<double>();
  printf("---------------------\n");

  test_to_fp<float>();
  printf("---------------------\n");

  test_from_fp<double>();
  printf("---------------------\n");

  test_from_fp<float>();
  printf("---------------------\n");

  return EXIT_SUCCESS;
}
