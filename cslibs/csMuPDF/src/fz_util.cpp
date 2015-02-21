/****************************************************************************
** Copyright (c) 2013-2015, Carsten Schmidt. All rights reserved.
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

extern "C" {
#include <mupdf/pdf.h>
};

#include "internal/fz_util.h"

////// Public ////////////////////////////////////////////////////////////////

void toContentsNode(fz_outline *outline, csPdfContentsNode *parent)
{
  csPdfContentsNode *child =
      new csPdfContentsNode(outline->title,
                            outline->dest.kind == FZ_LINK_GOTO
                            ? outline->dest.ld.gotor.page
                            : -1,
                            parent);
  if( child == 0 ) {
    return;
  }

  parent->appendChild(child);

  if( outline->down != NULL ) {
    toContentsNode(outline->down, child);
  }

  if( outline->next != NULL ) {
    toContentsNode(outline->next, parent);
  }
}

inline bool testFlags(fz_link_dest *dest, const int flags)
{
  return (dest->ld.gotor.flags & flags) != 0;
}

csPdfLink toLink(fz_link *link, const QTransform& ctm)
{
  return toLink(toRect(&(link->rect)), &(link->dest), ctm);
}

csPdfLink toLink(const QRectF& srcRect, void *data, const QTransform& ctm)
{
  fz_link_dest *dest = static_cast<fz_link_dest*>(data);

  csPdfLink link;

  if( dest->kind == FZ_LINK_GOTO ) {
    link._type = csPdfLink::Goto;

    link._page = dest->ld.gotor.page;

    const double l = testFlags(dest, fz_link_flag_l_valid)
        ? dest->ld.gotor.lt.x
        : 0.0;
    const double t = testFlags(dest, fz_link_flag_t_valid)
        ? dest->ld.gotor.lt.y
        : 0.0;
    const double r = testFlags(dest, fz_link_flag_r_valid)
        ? dest->ld.gotor.rb.x
        : 0.0;
    const double b = testFlags(dest, fz_link_flag_b_valid)
        ? dest->ld.gotor.rb.y
        : 0.0;

    link._destTL = ctm.map(QPointF(l, t));
    link._destBR = ctm.map(QPointF(r, b));

    link._is_fit_h      = testFlags(dest, fz_link_flag_fit_h);
    link._is_fit_v      = testFlags(dest, fz_link_flag_fit_v);
    link._is_zoom       = testFlags(dest, fz_link_flag_r_is_zoom);
    link._is_new_window = dest->ld.gotor.new_window != 0;
  }

  if( !link.isInvalid() ) {
    link._srcRect = srcRect;
  }

  return link;
}

inline QPointF distance(const QRectF& a, const QRectF& b)
{
  double dx(0.0), dy(0.0);

  if(        a.right() < b.left() ) {
    dx = b.left() - a.right(); // 'a' left of 'b'
  } else if( b.right() < a.left() ) {
    dx = a.left() - b.right(); // 'b' left of 'a'
  }

  if(        a.bottom() < b.top() ) {
    dy = b.top() - a.bottom(); // 'a' above 'b'
  } else if( b.bottom() < a.top() ) {
    dy = a.top() - b.bottom(); // 'b' above 'a'
  }

  return QPointF(dx, dy);
}

#define COMMIT_TEXT()                           \
  if( !hitText.isEmpty() ) {                    \
    const csPdfText pdfText(hitRect, hitText);  \
    texts.push_back(pdfText);                   \
  }

csPdfTexts extractText(void *data, const double wordSpacing)
{
  fz_text_page *page = static_cast<fz_text_page*>(data);

  csPdfTexts texts;

  for(int blockIdx = 0; blockIdx < page->len; blockIdx++) {
    if( page->blocks[blockIdx].type != FZ_PAGE_BLOCK_TEXT ) {
      continue;
    }

    fz_text_block *text = page->blocks[blockIdx].u.text;

    for(int lineIdx = 0; lineIdx < text->len; lineIdx++) {
      fz_text_line *line = &(text->lines[lineIdx]);

      for(fz_text_span *span = line->first_span; span != NULL; span = span->next) {

        QString hitText = QString();
        QRectF  hitRect = QRectF();
        for(int charIdx = 0; charIdx < span->len; charIdx++) {
          const QChar  c(span->text[charIdx].c);
          fz_rect bbox;
          const QRectF r(toRect(fz_text_char_bbox(&bbox, span, charIdx)));
          if( c.isSpace() ) {
            // Process Whitespace
            COMMIT_TEXT();
            hitText = QString();
            hitRect = QRectF();
          } else if( hitText.isEmpty() ) {
            // No Hit Yet -> Initialize
            hitText = c;
            hitRect = r;
          } else if( distance(hitRect, r).x() > wordSpacing ) {
            // Distance Too Big -> Split Text
            COMMIT_TEXT();
            hitText = c;
            hitRect = r;
          } else {
            // Normal Case: Append
            hitText += c;
            hitRect |= r;
          }
        } // character
        COMMIT_TEXT(); // Append Any Remaining Text
      } // span
    } // line
  } // block

  return texts;
}

extern "C" void fzLock(void *user, int lock)
{
  QMutex **locks = static_cast<QMutex**>(user);
  locks[lock]->lock();
}

extern "C" void fzUnlock(void *user, int lock)
{
  QMutex **locks = static_cast<QMutex**>(user);
  locks[lock]->unlock();
}

fz_locks_context *newLocksContext()
{
  QMutex **locks = new QMutex*[FZ_LOCK_MAX];
  if( locks == 0 ) {
    return NULL;
  }

  for(int i = 0; i < FZ_LOCK_MAX; i++) {
    locks[i] = new QMutex();
    if( locks[i] == 0 ) {
      for(int j = 0; j < i; j++) {
        delete locks[j];
      }
      delete[] locks;

      return NULL;
    }
  }

  fz_locks_context *ctx = static_cast<fz_locks_context*>(malloc(sizeof(fz_locks_context)));
  if( ctx == NULL ) {
    for(int i = 0; i < FZ_LOCK_MAX; i++) {
      delete locks[i];
    }
    delete[] locks;

    return NULL;
  }

  ctx->user   = static_cast<void*>(locks);
  ctx->lock   = fzLock;
  ctx->unlock = fzUnlock;

  return ctx;
}

void deleteLocksContext(fz_locks_context* &ctx)
{
  if( ctx == NULL ) {
    return;
  }

  QMutex **locks = static_cast<QMutex**>(ctx->user);
  for(int i = 0; i < FZ_LOCK_MAX; i++) {
    delete locks[i];
  }
  delete[] locks;

  free(ctx);
  ctx = NULL;
}

QTransform ctmForPage(fz_document *doc, fz_page *page)
{
  pdf_document *pdfdoc = pdf_specifics(doc);
  if( pdfdoc == NULL ) {
    return QTransform();
  }

  pdf_page *pdfpage = reinterpret_cast<pdf_page*>(page);

  return toTransform(&(pdfpage->ctm));
}
