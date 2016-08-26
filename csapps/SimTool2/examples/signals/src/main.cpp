#include <cmath>

#include <SimCore/sim_mod.h>

////// Macros ////////////////////////////////////////////////////////////////

#define IMPL(no) \
  sig##no = signal(t, amp##no, frq##no, phs##no);

#define USE(no)                                     \
  sim_use_double("AMP" #no, &amp##no, Input, ctx);  \
  sim_use_double("FRQ" #no, &frq##no, Input, ctx);  \
  sim_use_double("PHS" #no, &phs##no, Input, ctx);  \
  sim_use_double("SIG" #no, &sig##no, Output, ctx);

////// Global ////////////////////////////////////////////////////////////////

static const double PI = 3.14;

static double t;

static double sig1, amp1, frq1, phs1;
static double sig2, amp2, frq2, phs2;
static double sig3, amp3, frq3, phs3;
static double sig4, amp4, frq4, phs4;

////// Private ///////////////////////////////////////////////////////////////

double signal(const double t,
              const double amp, const double frq, const double phs)
{
  const double phi = phs*PI/180.0;
  return amp*std::sin(2.0*PI*frq*t + phi);
}

////// Public ////////////////////////////////////////////////////////////////

extern "C" SIM_MOD_EXPORT void sim_init(int /*argc*/, char ** /*argv*/, void *ctx)
{
  USE(1);
  USE(2);
  USE(3);
  USE(4);
}

extern "C" SIM_MOD_EXPORT void sim_start(void * /*ctx*/)
{
  t = 0;

  IMPL(1);
  IMPL(2);
  IMPL(3);
  IMPL(4);
}

extern "C" SIM_MOD_EXPORT void sim_step(double dt, void * /*ctx*/)
{
  t += dt;

  IMPL(1);
  IMPL(2);
  IMPL(3);
  IMPL(4);
}

extern "C" SIM_MOD_EXPORT void sim_stop(void * /*ctx*/)
{
}
