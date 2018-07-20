#include <cstdio>
#include <cstdlib>

#include <csUtil/csFlags.h>

enum class MyFlags : unsigned int {
  NoFlags  =  0,
  Flag0    =  1,
  Flag1    =  2,
  Flag2    =  4,
  Flag3    =  8,
  AllFlags = 15
};

CS_ENABLE_FLAGS(MyFlags)

void print(const MyFlags& f, const char *name)
{
  using data_t = typename std::underlying_type<MyFlags>::type;
  printf(" %s = %08X\n", name, static_cast<data_t>(f));
  printf("!%s = %s\n", name, !f ? "true" : "false");
  printf("~%s = %08X\n", name, static_cast<data_t>(~f));
}

void print(const MyFlags& f)
{
  using data_t = typename std::underlying_type<MyFlags>::type;
  printf("%08X\n", static_cast<data_t>(f));
}

int main(int /*argc*/, char ** /*argv*/)
{
  const MyFlags flags0(MyFlags::NoFlags);
  const MyFlags flag0(MyFlags::Flag0);
  const MyFlags flag1(MyFlags::Flag1);
  const MyFlags flag2(MyFlags::Flag2);
  const MyFlags flag3(MyFlags::Flag3);
  const MyFlags flagsAll(MyFlags::AllFlags);

  print(flags0, "flags0");
  printf("---\n");
  print(flagsAll, "flagsAll");
  printf("---\n");

  MyFlags f(MyFlags::NoFlags);
  f |= MyFlags::Flag0;
  f |= flag1;
  printf(" OR(1|2): "); print(f);
  f &= MyFlags::Flag2;
  f &= flag3;
  printf("AND(4&8): "); print(f);
  f ^= MyFlags::Flag1;
  f ^= flag2;
  printf("XOR(2^4): "); print(f);
  printf("---\n");

  f &= MyFlags::NoFlags;
  cs::setFlags(f, MyFlags::Flag1);
  printf("  SET: "); print(f);
  printf(" TEST: %s\n", cs::testFlags(f, MyFlags::Flag1) ? "true" : "false");
  cs::setFlags(f, MyFlags::Flag1, false);
  printf("RESET: "); print(f);
  printf(" TEST: %s\n", cs::testFlags(f, MyFlags::Flag1) ? "true" : "false");
  printf("---\n");

  f = MyFlags::Flag0 | MyFlags::Flag1;
  printf("COMBINE(1|2): "); print(f);
  f = flag0 | MyFlags::Flag1;
  printf("COMBINE(1|2): "); print(f);
  f = MyFlags::Flag0 | flag1;
  printf("COMBINE(1|2): "); print(f);
  printf("---\n");

  f = MyFlags::Flag0 | MyFlags::Flag1 | MyFlags::Flag2 | MyFlags::Flag3;
  printf("COMBINE(1|2|4|8): "); print(f);
  f = flag0 | MyFlags::Flag1 | MyFlags::Flag2 | MyFlags::Flag3;
  printf("COMBINE(1|2|4|8): "); print(f);
  f = MyFlags::Flag0 | MyFlags::Flag1 | MyFlags::Flag2 | flag3;
  printf("COMBINE(1|2|4|8): "); print(f);

  printf("TEST(1|2|4|8): %s\n",
         cs::testMask(f, MyFlags::Flag0 | MyFlags::Flag1 | MyFlags::Flag2 | MyFlags::Flag3)
         ? "true"
         : "false");

  return EXIT_SUCCESS;
}
