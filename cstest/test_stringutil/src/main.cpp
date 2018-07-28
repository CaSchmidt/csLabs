#include <cstdio>
#include <cstdlib>

#include <csUtil/csStringUtil.h>

void simplify(const std::string& s)
{
  const std::string x = cs::simplified(s);
  printf("<%s> -> <%s>\n", s.data(), x.data());
}

int main(int /*argc*/, char ** /*argv*/)
{
  const std::string null;
  const std::string empty("");
  const std::string single("single");
  const std::string example("  lots\t of\nwhitespace\r\n ");

  simplify(null);
  simplify(empty);
  simplify(single);
  simplify(example);

  return EXIT_SUCCESS;
}
