/****************************************************************************
** Copyright (c) 2014-2015, Carsten Schmidt. All rights reserved.
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

#include "internal/fz_pathext.h"

#include "internal/fz_util.h"

#if 0
extern "C" void pathext_free_user(fz_device *)
{
}

extern "C" void pathext_fill_path(fz_device *, fz_path *, int, const fz_matrix *, fz_colorspace *, float *, float)
{
}
#endif

extern "C" void pathext_stroke_path(fz_device *dev, fz_path *path,
                                    fz_stroke_state *, const fz_matrix *ctm,
                                    fz_colorspace *, float *, float)
{
  PathExt *ext = static_cast<PathExt*>(dev->user);
  if( ext == 0 ) {
    return;
  }

  QPainterPath qPath;
  float *c = path->coords;
  for(int i = 0; i < path->cmd_len; i++) {
    if(        path->cmds[i] == FZ_MOVETO ) {
      qPath.moveTo(c[0], c[1]);
      c += 2;
    } else if( path->cmds[i] == FZ_LINETO ) {
      qPath.lineTo(c[0], c[1]);
      c += 2;
    } else if( path->cmds[i] == FZ_CURVETO ) {
      qPath.cubicTo(c[0], c[1], c[2], c[3], c[4], c[5]);
      c += 6;
    } else if( path->cmds[i] == FZ_CLOSE_PATH ) {
      if( !ext->closed ) {
        return;
      }
      qPath.closeSubpath();
    } else {
      return;
    }
  }

  const QTransform qCtm = toTransform(ctm);
  qPath = qCtm.map(qPath);

  ext->paths.push_back(qPath);
}

#if 0
extern "C" void pathext_clip_path(fz_device *, fz_path *, const fz_rect *, int, const fz_matrix *)
{
}

extern "C" void pathext_clip_stroke_path(fz_device *, fz_path *, const fz_rect *, fz_stroke_state *, const fz_matrix *)
{
}

extern "C" void pathext_fill_text(fz_device *, fz_text *, const fz_matrix *, fz_colorspace *, float *, float)
{
}

extern "C" void pathext_stroke_text(fz_device *, fz_text *, fz_stroke_state *, const fz_matrix *, fz_colorspace *, float *, float)
{
}

extern "C" void pathext_clip_text(fz_device *, fz_text *, const fz_matrix *, int)
{
}

extern "C" void pathext_clip_stroke_text(fz_device *, fz_text *, fz_stroke_state *, const fz_matrix *)
{
}

extern "C" void pathext_ignore_text(fz_device *, fz_text *, const fz_matrix *)
{
}

extern "C" void pathext_fill_image_mask(fz_device *, fz_image *, const fz_matrix *, fz_colorspace *, float *, float)
{
}

extern "C" void pathext_clip_image_mask(fz_device *, fz_image *, const fz_rect *, const fz_matrix *)
{
}

extern "C" void pathext_fill_image(fz_device *, fz_image *, const fz_matrix *, float)
{
}

extern "C" void pathext_fill_shade(fz_device *, fz_shade *, const fz_matrix *, float)
{
}

extern "C" void pathext_pop_clip(fz_device *)
{
}

extern "C" void pathext_begin_mask(fz_device *, const fz_rect *, int, fz_colorspace *, float *)
{
}

extern "C" void pathext_end_mask(fz_device *)
{
}

extern "C" void pathext_begin_group(fz_device *, const fz_rect *, int, int, int, float)
{
}

extern "C" void pathext_end_group(fz_device *)
{
}

extern "C" int pathext_begin_tile(fz_device *, const fz_rect *, const fz_rect *, float, float, const fz_matrix *, int)
{
  return 0;
}

extern "C" void pathext_end_tile(fz_device *)
{
}
#endif

fz_device *fzNewPathExtDevice(fz_context *ctx, PathExt *user)
{
  fz_device *dev = NULL;
  fz_var(dev);

  fz_try(ctx) {
    dev = fz_new_device(ctx, user);
  } fz_catch(ctx) {
    return NULL;
  }

#if 0
  dev->free_user = pathext_free_user;

  dev->fill_path        = pathext_fill_path;
#endif
  dev->stroke_path      = pathext_stroke_path;
#if 0
  dev->clip_path        = pathext_clip_path;
  dev->clip_stroke_path = pathext_clip_stroke_path;

  dev->fill_text        = pathext_fill_text;
  dev->stroke_text      = pathext_stroke_text;
  dev->clip_text        = pathext_clip_text;
  dev->clip_stroke_text = pathext_clip_stroke_text;
  dev->ignore_text      = pathext_ignore_text;

  dev->fill_image_mask = pathext_fill_image_mask;
  dev->clip_image_mask = pathext_clip_image_mask;
  dev->fill_image      = pathext_fill_image;
  dev->fill_shade      = pathext_fill_shade;

  dev->pop_clip = pathext_pop_clip;

  dev->begin_mask  = pathext_begin_mask;
  dev->end_mask    = pathext_end_mask;
  dev->begin_group = pathext_begin_group;
  dev->end_group   = pathext_end_group;

  dev->begin_tile = pathext_begin_tile;
  dev->end_tile   = pathext_end_tile;
#endif

  return dev;
}
