#include <cstdio>
#include <cstdlib>

#undef HAVE_BENCHMARK

#ifdef HAVE_BENCHMARK
# include <Windows.h>
#endif

#include <csUtil/csConvert.h>

////// Names /////////////////////////////////////////////////////////////////

template<typename DataT>
struct Name {
  inline static const char *get()
  {
    // SFINAE
  }
};

template<>
struct Name<int8_t> {
  inline static const char *get()
  {
    return "int8_t";
  }
};

template<>
struct Name<int16_t> {
  inline static const char *get()
  {
    return "int16_t";
  }
};

template<>
struct Name<int32_t> {
  inline static const char *get()
  {
    return "int32_t";
  }
};

template<>
struct Name<uint8_t> {
  inline static const char *get()
  {
    return "uint8_t";
  }
};

template<>
struct Name<uint16_t> {
  inline static const char *get()
  {
    return "uint16_t";
  }
};

template<>
struct Name<uint32_t> {
  inline static const char *get()
  {
    return "uint32_t";
  }
};

template<>
struct Name<float> {
  inline static const char *get()
  {
    return "float";
  }
};

////// Cast //////////////////////////////////////////////////////////////////

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

  printf("%s\n", Name<DataT>::get());
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

  printf("%s\n", Name<DataT>::get());
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

  printf("%s\n", Name<float>::get());
  for(int i = 0; i < N; i++) {
    printf("%6.1f = %6.1f%s\n", raw[i], dest[i],
           double(raw[i]) != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

////// Scale /////////////////////////////////////////////////////////////////

template<typename DataT>
void test_signed(const double c1)
{
  const int N = 255;
  DataT   *raw = new DataT[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = -128+i;
    dest[i] = 0;
  }

  csConvert<DataT>(dest, raw, N, c1);

  printf("%s, c1=%.1f\n", Name<DataT>::get(), c1);
  for(int i = 0; i < N; i++) {
    printf("%4d = %6.1f%s\n", raw[i], dest[i],
           c1*double(raw[i]) != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

template<typename DataT>
void test_unsigned(const double c1)
{
  const int N = 255;
  DataT   *raw = new DataT[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = i;
    dest[i] = 0;
  }

  csConvert<DataT>(dest, raw, N, c1);

  printf("%s, c1=%.1f\n", Name<DataT>::get(), c1);
  for(int i = 0; i < N; i++) {
    printf("%3d = %5.1f%s\n", raw[i], dest[i],
           c1*double(raw[i]) != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

void test_float(const double c1)
{
  const int N = 255;
  float   *raw = new float[N];
  double *dest = new double[N];

  for(int i = 0; i < N; i++) {
    raw[i]  = -128.0f+i;
    dest[i] = 0;
  }

  csConvert<float>(dest, raw, N, c1);

  printf("%s, c1=%.1f\n", Name<float>::get(), c1);
  for(int i = 0; i < N; i++) {
    printf("%6.1f = %6.1f%s\n", raw[i], dest[i],
           c1*double(raw[i]) != dest[i] ? " ERROR" : "");
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

  csConvert<DataT>(dest, raw, N, c1, c0);

  printf("%s, c1=%.1f, c0=%.1f\n", Name<DataT>::get(), c1, c0);
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

  printf("%s, c1=%.1f, c0=%.1f\n", Name<DataT>::get(), c1, c0);
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

  printf("%s, c1=%.1f, c0=%.1f\n", Name<float>::get(), c1, c0);
  for(int i = 0; i < N; i++) {
    printf("%6.1f = %6.1f%s\n", raw[i], dest[i],
           c1*double(raw[i])+c0 != dest[i] ? " ERROR" : "");
  }

  delete[] raw;
  delete[] dest;
}

////// Benchmark /////////////////////////////////////////////////////////////

#ifdef HAVE_BENCHMARK
template<typename DataT>
void convert(double *dest, const DataT *src, const int count)
{
  for(int i = 0; i < count; i++) {
    *dest++ = static_cast<double>(*src++);
  }
}

template<typename DataT>
void convert(double *dest, const DataT *src, const int count,
             const double c1, const double c0)
{
  for(int i = 0; i < count; i++) {
    *dest++ = c1*static_cast<double>(*src++) + c0;
  }
}

template<typename DataT>
void benchmark()
{
  const int N = 500000000;

  DataT  *raw = new DataT[N];
  double *phy = new double[N];

  DataT value = 0;
  for(int i = 0; i < N; i++, value++) {
    raw[i] = value;
  }

  const double c1 = 2.0;
  const double c0 = 1.0;

#define HAVE_LINEAR
#define HAVE_OPT

  const DWORD beg = GetTickCount();
#ifdef HAVE_LINEAR
# ifdef HAVE_OPT
  csConvert(phy, raw, N, c1, c0);
# else
  convert(phy, raw, N, c1, c0);
# endif
#else
# ifdef HAVE_OPT
  csConvert(phy, raw, N);
# else
  convert(phy, raw, N);
# endif
#endif
  const DWORD end = GetTickCount();

  printf("duration = %dms\n", end-beg);

  delete[] raw;
  delete[] phy;
}
#endif

////// Main //////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
#ifndef HAVE_BENCHMARK  
  // Cast ////////////////////////////////////////////////////////////////////

  // test_signed<int8_t>();
  // test_signed<int16_t>();
  // test_signed<int32_t>();

  // test_unsigned<uint8_t>();
  // test_unsigned<uint16_t>();
  // test_unsigned<uint32_t>();

  // test_float();

  // Scale ///////////////////////////////////////////////////////////////////

  // test_signed<int8_t>(2.0);
  // test_signed<int16_t>(2.0);
  // test_signed<int32_t>(2.0);

  // test_unsigned<uint8_t>(2.0);
  // test_unsigned<uint16_t>(2.0);
  // test_unsigned<uint32_t>(2.0);

  // test_float(2.0);

  // Linear //////////////////////////////////////////////////////////////////

  // test_signed<int8_t>(2.0, 1.0);
  // test_signed<int16_t>(2.0, 1.0);
  // test_signed<int32_t>(2.0, 1.0);

  // test_unsigned<uint8_t>(2.0, 1.0);
  // test_unsigned<uint16_t>(2.0, 1.0);
  // test_unsigned<uint32_t>(2.0, 1.0);

  // test_float(2.0, 1.0);
#else
  benchmark<int8_t>();
#endif

  return EXIT_SUCCESS;
}
