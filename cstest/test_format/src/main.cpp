#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>

#include <csUtil/csFormat.h>

template<typename CharT>
void print_format(const CharT *str)
{
  std::cout << (std::string)csFormat(str) << std::endl;
}

template<typename ValueT>
void format_floating(const ValueT value,
                     const int width, const char format = 'g', const int precision = 6, const char fill = '.',
                     typename std::enable_if<
                     std::is_floating_point<ValueT>::value
                     >::type * = nullptr)
{
  const int abswidth = width < 0
      ? -width
      :  width;

  const std::string output =
      csFormat("   le = %1|   ri = %2|   no = %3")
      .arg(value, -abswidth, format, precision, fill)
      .arg(value,  abswidth, format, precision, fill)
      .arg(value,         0, format, precision, fill);

  printf("%s", output.data());
}

template<typename ValueT>
void format_integral(const ValueT value,
                     const int width, const int base = 10, const char fill = '.',
                     typename std::enable_if<
                     std::is_integral<ValueT>::value
                     >::type * = nullptr)
{
  const int abswidth = width < 0
      ? -width
      :  width;

  const std::string output =
      csFormat("   le = %1|   ri = %2|   no = %3")
      .arg(value, -abswidth, base, fill)
      .arg(value,  abswidth, base, fill)
      .arg(value,         0, base, fill);

  printf("%s", output.data());
}

void format_string(const std::string& value,
                   const int width, const char fill = '.')
{
  const int abswidth = width < 0
      ? -width
      :  width;

  const std::string output =
      csFormat("   le = %1|   ri = %2|   no = %3")
      .arg(value, -abswidth, fill)
      .arg(value,  abswidth, fill)
      .arg(value,         0, fill);

  printf("%s", output.data());
}

void format_test()
{
  printf("\n");
  printf("0123456701234567|0123456701234567|0123456701234567\n");

  format_string(std::string("abc"), 8, '.'); printf("<<\n");
  format_integral(255, 8, 10); printf("<<\n");
  format_floating(-7.5f, 8, 'f', 3); printf("<<\n");

  printf("\n");
}

int main(int /*argc*/, char ** /*argv*/)
{
  print_format("%abc%abc%0abc%%1abc%");
  print_format("abc%abc%abc%0abc%%1abc%abc");
  print_format("%1abc%2abc%3");
  print_format("abc%1abc%2abc%3abc");
  print_format("%1%4abc%2%5abc%3%6");
  print_format("abc%1%4abc%2%5abc%3%6abc");
  print_format("%10000000000abc%20000000000abc%30000000000");
  print_format("abc%10000000000abc%20000000000abc%30000000000abc");

  format_test();

  const std::string output = csFormat("%1 = %2") % "samurai" % 7;
  printf("%s\n", output.data());

  return EXIT_SUCCESS;
}
