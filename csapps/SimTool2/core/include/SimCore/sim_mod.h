/****************************************************************************
** Copyright (c) 2012,2016, Carsten Schmidt. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#ifndef __SIM_MOD_H__
#define __SIM_MOD_H__

#include <stdint.h>

#ifdef _MSC_VER
# define SIM_EXPORT  __declspec(dllexport)
# define SIM_IMPORT  __declspec(dllimport)
#else
# error Compiler not recognized!
#endif

#define SIM_MOD_EXPORT  SIM_EXPORT

#ifdef SIM_BUILD_CORE_LIB
# define SIM_MOD_IMPORT  SIM_EXPORT
#else
# define SIM_MOD_IMPORT  SIM_IMPORT
#endif

/*
 * Direction
 * =========
 *
 * +--------+    Input    +------------+
 * |        |  -------->  |            |
 * |   DB   |             |   Module   |
 * |        |  <--------  |            |
 * +--------+    Output   +------------+
 *
 */
typedef enum _Direction {
  Input       = 1,
  Output      = 2,
  InputOutput = 3
} Direction;

typedef enum _LogLevel {
  Text    = 0,
  Warning = 1,
  Error   = 2
} LogLevel;

#ifdef __cplusplus
extern "C" {
#endif

SIM_MOD_IMPORT void sim_use_double(const char *nam, double *ptr,
                                   const uint32_t dir, void *ctx);
SIM_MOD_IMPORT void sim_use_single(const char *nam, float *ptr,
                                   const uint32_t dir, void *ctx);
SIM_MOD_IMPORT void sim_use_int(const char *nam, int32_t *ptr,
                                const uint32_t dir, void *ctx);
SIM_MOD_IMPORT void sim_use_uint(const char *nam, uint32_t *ptr,
                                 const uint32_t dir, void *ctx);

SIM_MOD_IMPORT void sim_on(void *ptr, void *ctx);
SIM_MOD_IMPORT void sim_off(void *ptr, void *ctx);

SIM_MOD_IMPORT void sim_printf(void *ctx, const char *fmt, ...);
SIM_MOD_IMPORT void sim_printf2(void *ctx, const uint32_t lvl,
                                const char *fmt, ...);

typedef void(*sim_init_func)(int argc, char **argv, void *ctx);
typedef void(*sim_start_func)(void *ctx);
typedef void(*sim_step_func)(double dt, void *ctx);
typedef void(*sim_stop_func)(void *ctx);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SIM_MOD_H__
