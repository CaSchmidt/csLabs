#include <cstdio>
#include <cstdlib>

#include <array>

#include <csUtil/csStringUtil.h>

void simplify(const std::string& s)
{
  const std::string x = cs::simplified(s);
  printf("<%s> -> <%s>\n", s.data(), x.data());
}

void trim(const std::string& s)
{
  const std::string x = cs::trimmed(s);
  printf("<%s> -> <%s>\n", s.data(), x.data());
}

void test_length(const std::array<char,4>& s, const char *name)
{
  printf("length %s = %llu\n", name, cs::lengthN(s.data(), s.size()));
}

void test_length()
{
  std::array<char,4> s1{ {'a', 'b', 'c', 'd'} };
  test_length(s1, "s1");
  std::array<char,4> s2{ {'a', 'b', '\0', 'd'} };
  test_length(s2, "s2");
}

int main(int /*argc*/, char ** /*argv*/)
{
  const std::string null;
  const std::string empty("");
  const std::string letter1("l");
  const std::string letter2(" l ");
  const std::string single("single");
  const std::string simple("   simple   ");
  const std::string example("  lots\t of\nwhitespace\r\n ");

  simplify(null);
  simplify(empty);
  simplify(letter1);
  simplify(letter2);
  simplify(single);
  simplify(simple);
  simplify(example);

  printf("\n---------\n\n");

  trim(null);
  trim(empty);
  trim(letter1);
  trim(letter2);
  trim(single);
  trim(simple);
  trim(example);

  printf("\n---------\n\n");

  test_length();

  return EXIT_SUCCESS;
}
