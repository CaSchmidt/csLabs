#include <cstdio>
#include <cstdlib>

#include <csUtil/csConvert.h>

// Conversion ////////////////////////////////////////////////////////////////

template<typename DataT>
void test_signed()
{
  const int N = 255;
  DataT   *raw = new DataT[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = -128+i;
    dest[i] = 0;
  }

  csConvert<DataT>(dest, raw, N);

  for(int i = 0; i < N; i++) {
    printf("%4d = %6.1f%s\n", raw[i], dest[i],
           double(raw[i]) != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

template<typename DataT>
void test_unsigned()
{
  const int N = 255;
  DataT   *raw = new DataT[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = i;
    dest[i] = 0;
  }

  csConvert<DataT>(dest, raw, N);

  for(int i = 0; i < N; i++) {
    printf("%3d = %5.1f%s\n", raw[i], dest[i],
           double(raw[i]) != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

void test_float()
{
  const int N = 255;
  float   *raw = new float[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = -128.0f+i;
    dest[i] = 0;
  }

  csConvert<float>(dest, raw, N);

  for(int i = 0; i < N; i++) {
    printf("%6.1f = %6.1f%s\n", raw[i], dest[i],
           double(raw[i]) != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

////// Linear ////////////////////////////////////////////////////////////////

template<typename DataT>
void test_signed(const double c1, const double c0)
{
  const int N = 255;
  DataT   *raw = new DataT[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = -128+i;
    dest[i] = 0;
  }

  printf("c1=%.1f, c0=%.1f\n", c1, c0);
  csConvert<DataT>(dest, raw, N, c1, c0);

  for(int i = 0; i < N; i++) {
    printf("%4d = %6.1f%s\n", raw[i], dest[i],
           c1*double(raw[i])+c0 != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

template<typename DataT>
void test_unsigned(const double c1, const double c0)
{
  const int N = 255;
  DataT   *raw = new DataT[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = i;
    dest[i] = 0;
  }

  csConvert<DataT>(dest, raw, N, c1, c0);

  printf("c1=%.1f, c0=%.1f\n", c1, c0);
  for(int i = 0; i < N; i++) {
    printf("%3d = %5.1f%s\n", raw[i], dest[i],
           c1*double(raw[i])+c0 != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

void test_float(const double c1, const double c0)
{
  const int N = 255;
  float   *raw = new float[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = -128.0f+i;
    dest[i] = 0;
  }

  csConvert<float>(dest, raw, N, c1, c0);

  printf("c1=%.1f, c0=%.1f\n", c1, c0);
  for(int i = 0; i < N; i++) {
    printf("%6.1f = %6.1f%s\n", raw[i], dest[i],
           c1*double(raw[i])+c0 != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

////// Main //////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
  // test_float();
  // test_signed<int32_t>();
  // test_unsigned<uint32_t>();

  // test_float(2.0, 1.0);
  // test_signed<int32_t>(2.0, 1.0);
  // test_unsigned<uint32_t>(2.0, 1.0);

  return EXIT_SUCCESS;
}
