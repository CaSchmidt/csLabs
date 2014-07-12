/****************************************************************************
** Copyright (c) 2013-2014, Carsten Schmidt. All rights reserved.
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

#include <csPDF/internal/fz_render.h>
#include <csPDF/internal/fz_util.h>

FzRenderData::FzRenderData()
  : context(NULL)
  , displayList(NULL)
  , pixmap(NULL)
{
  renderRect.x0 = 0;
  renderRect.y0 = 0;
  renderRect.x1 = 0;
  renderRect.y1 = 0;
}

FzRenderData::FzRenderData(fz_context *ctx, fz_display_list *list,
                           fz_pixmap *pix, fz_rect *rect)
  : context(ctx)
  , displayList(list)
  , pixmap(pix)
{
  renderRect.x0 = rect->x0;
  renderRect.y0 = rect->y0;
  renderRect.x1 = rect->x1;
  renderRect.y1 = rect->y1;

  fz_keep_display_list(context, displayList);
}

FzRenderData::FzRenderData(const FzRenderData& other)
  : context(other.context)
  , displayList(other.displayList)
  , pixmap(other.pixmap)
  , renderRect(other.renderRect)
{
  ref();
}

FzRenderData::~FzRenderData()
{
  unref();
}

FzRenderData& FzRenderData::operator=(const FzRenderData& other)
{
  if( this != &other ) {
    unref();

    context     = other.context;
    displayList = other.displayList;
    pixmap      = other.pixmap;
    renderRect  = other.renderRect;

    ref();
  }
  return *this;
}

void FzRenderData::ref()
{
  if( context != NULL  &&  displayList != NULL ) {
    fz_keep_display_list(context, displayList);
  }
  if( context != NULL  &&  pixmap != NULL ) {
    fz_keep_pixmap(context, pixmap);
  }
}

void FzRenderData::unref()
{
  if( context != NULL  &&  displayList != NULL ) {
    fz_drop_display_list(context, displayList);
  }
  if( context != NULL  &&  pixmap != NULL ) {
    fz_drop_pixmap(context, pixmap);
  }
}

void fzRender(FzRenderData& data)
{
  fz_matrix I;

  fz_context *context = fz_clone_context(data.context);
  if( context == NULL ) {
    return;
  }

  fz_try(context) {
    fz_device *device = fz_new_draw_device(context, data.pixmap);
    fz_run_display_list(data.displayList, device,
                        setIdentity(&I),
                        &(data.renderRect), NULL);
    fz_free_device(device);
  } fz_catch(context) {
  }

  fz_free_context(context);
}
