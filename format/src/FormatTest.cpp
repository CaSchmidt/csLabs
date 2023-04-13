#include <cstdio>
#include <cstdlib>

enum MyEnum {
  A,
  B,
  C
};

struct MyStruct {};

class MyParentA {};
class MyParentB {};

class MyClass
  : public MyParentA
  , public MyParentB {
public:
  MyClass()
    : MyParentA()
    , MyParentB()
  {
  }

private:
  int x, y;
};

void test_empty(const int *intPtr, const int& intRef)
{
}

void test_flow(const int *intPtr, const int& intRef)
{
  int y = 0;
  if (intPtr != nullptr) {
    y = *intPtr;
  } else if (intRef != 0) {
    y = intRef;
  } else {
  }
}

int test_ops(const int a, const int b)
{
  int x = a
        + b
        + 7;
  return x;
}

int test_return(const int i)
{
  return i >= 0
         ? 1
         : 0;
}
