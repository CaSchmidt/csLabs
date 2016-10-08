#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include <csUtil/csEndian.h>

#include "data.h"

template<typename T>
struct Test {
  inline static void run()
  {
    T dest0;
    // __debugbreak();
    csCopy<false>(&dest0, &src);
    printf("FALSE: 0x%08X -> 0x%08X\n", (T)src, dest0);

    T dest1;
    // __debugbreak();
    csCopy<true>(&dest1, &src);
    printf("TRUE : 0x%08X -> 0x%08X\n", (T)src, dest1);

    printf("\n");
  }
};

template<>
struct Test<uint64_t> {
  inline static void run()
  {
    uint64_t dest0;
    // __debugbreak();
    csCopy<false>(&dest0, &src);
    printf("FALSE: 0x%016I64X -> 0x%016I64X\n", src, dest0);

    uint64_t dest1;
    // __debugbreak();
    csCopy<true>(&dest1, &src);
    printf("TRUE : 0x%016I64X -> 0x%016I64X\n", src, dest1);

    printf("\n");
  }
};

int main(int argc, char **argv)
{
  Test<uint8_t>::run();
  Test<uint16_t>::run();
  Test<uint32_t>::run();
  Test<uint64_t>::run();

  return EXIT_SUCCESS;
}
