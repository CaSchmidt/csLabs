#include <SimCore/sim_mod.h>

static double t;

static double   Test_Double;
static float    Test_Single;
static int32_t  Test_Int;
static uint32_t Test_UInt;

static int32_t  Test_State;

static double Test_Sum;
static double Test_Summand;

extern "C" SIM_MOD_EXPORT void sim_init(int argc, char **argv, void *ctx)
{
  sim_printf(ctx, "sim_init");
  sim_printf2(ctx, Text, "Text");
  sim_printf2(ctx, Warning, "Warning");
  sim_printf2(ctx, Error, "Error");

  for(int i = 0; i <= argc; i++) {
    sim_printf(ctx, "%d: %s", i, argv[i]);
  }

  t = 0;

  sim_use_double("Test_Double", &Test_Double, Input, ctx);
  sim_use_single("Test_Single", &Test_Single, Input, ctx);
  sim_use_int("Test_Int", &Test_Int, Input, ctx);
  sim_use_uint("Test_UInt", &Test_UInt, Input, ctx);

  sim_use_double("Test_Alias", &Test_Double, InputOutput, ctx);

  sim_use_int("Test_State", &Test_State, Output, ctx);

  sim_use_double("Test_Sum", &Test_Sum, Output, ctx);
  sim_use_double("Test_Summand", &Test_Summand, Input, ctx);
}

extern "C" SIM_MOD_EXPORT void sim_start(void *ctx)
{
  sim_printf(ctx, "sim_start");
  Test_State = 1000;
}

extern "C" SIM_MOD_EXPORT void sim_step(double dt, void *ctx)
{
  t += dt;
  sim_printf(ctx, "sim_step(%.3fs,%.3fs)", dt, t);

  Test_State++;

  if( Test_State == 1005 ) {
    sim_off(&Test_State, ctx);
  }
  if( Test_State == 1010 ) {
    sim_on(&Test_State, ctx);
  }

  Test_Sum += Test_Summand;
}

extern "C" SIM_MOD_EXPORT void sim_stop(void *ctx)
{
  sim_printf(ctx, "sim_stop");
  Test_State = 2000;
}
