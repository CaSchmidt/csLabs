#include <cstdio>
#include <cstdlib>

#include <array>
#include <iostream>
#include <string>

#include <csUtil/csCharConv.h>

template<typename ValueT>
void print_from(const char *first, const char *last, const int base = 10)
{
  ValueT value = 0;
  const cs::from_chars_result res =
      cs::from_chars(first, last, value, base);

  printf("STR: %4s -> VAL: %4d, RES: %d\n", first, value, res.ec);
}

/*
void test_to_unicode()
{
  std::array<char16_t,10> u{u"42"};

  unsigned char value = 0;
  const cs::from_chars_result<char16_t> res =
      cs::from_chars<unsigned char,char16_t>(u.data(), u.data()+u.size(), value);

  printf("Unicode: value = %d, res = %d\n", value, res.ec);
}
*/

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
  std::array<char,5> s = { 0, 0, 0, 0, 0 };
  for(int i = 0; i <= 255; i++) {
    print_to(s.data(), s.data()+s.size()-1, (uint8_t)i);
  }
}

void test_from_signed()
{
  std::array<char,5> s = { 0, 0, 0, 0, 0 };
  for(int i = -128; i <= 127; i++) {
    print_to(s.data(), s.data()+s.size()-1, (int8_t)i);
  }
}

void test_double_input()
{
  std::array<char,128> s;
  s.fill(0);

  printf("double: ");
  fgets(s.data(), s.size()-1, stdin);

  double d = 0;
  const cs::from_chars_result res =
      cs::from_chars(s.data(), s.data()+s.size(), d);

  printf("VAL: %g, RES: %d\n", d, res.ec);
}

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

  /*
  test_to_unicode();
  printf("---------------------\n");
  */

  test_to_unsigned();
  printf("---------------------\n");

  test_to_signed();
  printf("---------------------\n");

  test_from_unsigned();
  printf("---------------------\n");

  test_from_signed();
  printf("---------------------\n");

  test_double_input();
  printf("---------------------\n");

  return EXIT_SUCCESS;
}
