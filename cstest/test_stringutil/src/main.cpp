#include <cstdio>
#include <cstdlib>

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

  return EXIT_SUCCESS;
}
