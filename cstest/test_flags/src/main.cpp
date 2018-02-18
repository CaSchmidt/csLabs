#include <cstdio>
#include <cstdlib>

#include <csUtil/csFlags.h>

enum MyFlag {
  NoFlags  =  0,
  Flag1    =  1,
  Flag2    =  2,
  Flag3    =  4,
  Flag4    =  8,
  AllFlags = 15
};
typedef csFlags<MyFlag> MyFlags;

CS_DECLARE_OPERATORS_FOR_FLAGS(MyFlags)

void print(const MyFlags& f, const char *name)
{
  printf(" %s = %08X\n", name, f);
  printf("!%s = %s\n",   name, !f ? "true" : "false");
  printf("~%s = %08X\n", name, ~f);
}

void print(const MyFlags& f)
{
  printf("%08X\n", f);
}

int main(int /*argc*/, char ** /*argv*/)
{
  const MyFlags flags0(NoFlags);
  const MyFlags flag1(Flag1);
  const MyFlags flag2(Flag2);
  const MyFlags flag3(Flag3);
  const MyFlags flag4(Flag4);
  const MyFlags flagsAll(AllFlags);

  print(flags0, "flags0");
  printf("---\n");
  print(flagsAll, "flagsAll");
  printf("---\n");

  MyFlags f;
  f |= Flag1;
  f |= flag2;
  printf(" OR(1|2): "); print(f);
  f &= Flag3;
  f &= flag4;
  printf("AND(3&4): "); print(f);
  f ^= Flag2;
  f ^= flag3;
  printf("XOR(2^3): "); print(f);
  printf("---\n");

  f &= NoFlags;
  f.setFlag(Flag2);
  printf("  SET: "); print(f);
  printf(" TEST: %s\n", f.testFlag(Flag2) ? "true" : "false");
  f.setFlag(Flag2, false);
  printf("RESET: "); print(f);
  printf(" TEST: %s\n", f.testFlag(Flag2) ? "true" : "false");
  printf("---\n");

#ifdef CS_FLAGS_HAVE_VALUE_OPERATORS
  f = flag1 & flag1;
  printf("AND(1&1): "); print(f);
  f = flag1 & 1;
  printf("AND(1&1): "); print(f);
  f = flag1 | flag2;
  printf(" OR(1|2): "); print(f);
  f = flag1 | 2;
  printf(" OR(1|2): "); print(f);
  f = flag1 ^ flag2;
  printf("XOR(1^2): "); print(f);
  f = flag1 ^ 2;
  printf("XOR(1^2): "); print(f);
  printf("---\n");
#endif

  f = Flag1 | Flag2;
  printf("COMBINE(1|2): "); print(f);
  f = flag1 | Flag2;
  printf("COMBINE(1|2): "); print(f);
  f = Flag1 | flag2;
  printf("COMBINE(1|2): "); print(f);
  printf("---\n");

  f = Flag1 | Flag2 | Flag3 | Flag4;
  printf("COMBINE(1|2|3|4): "); print(f);
  f = flag1 | Flag2 | Flag3 | Flag4;
  printf("COMBINE(1|2|3|4): "); print(f);
  f = Flag1 | Flag2 | Flag3 | flag4;
  printf("COMBINE(1|2|3|4): "); print(f);

  printf("TEST(1|2|3|4): %s\n", f.testFlags(Flag1 | Flag2 | Flag3 | Flag4)
         ? "true"
         : "false");

  return EXIT_SUCCESS;
}
