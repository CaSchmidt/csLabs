/****************************************************************************
** Copyright (c) 2018, Carsten Schmidt. All rights reserved.
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

#ifndef __BUFFERIMPL_H__
#define __BUFFERIMPL_H__

#include <dsound.h>

#include "internal/DeviceImpl.h"

namespace Audio {

  class BufferImpl {
  public:
    BufferImpl(DeviceImpl *device,
               const word_t bits, const word_t channels,
               const dword_t frequency, const dword_t size)
      : buffer(NULL)
    {
      HRESULT hr;

      WAVEFORMATEX fmt;
      {
        memset(&fmt, 0, sizeof(WAVEFORMATEX));
        fmt.wFormatTag      = WAVE_FORMAT_PCM;
        fmt.nChannels       = channels;
        fmt.wBitsPerSample  = bits;
        fmt.nSamplesPerSec  = frequency;
        fmt.nBlockAlign     = fmt.nChannels*fmt.wBitsPerSample/8;
        fmt.nAvgBytesPerSec = fmt.nSamplesPerSec*fmt.nBlockAlign;
        fmt.cbSize          = 0;
      }

      DSBUFFERDESC desc;
      {
        memset(&desc, 0, sizeof(DSBUFFERDESC));
        desc.dwSize          = sizeof(DSBUFFERDESC);
        desc.dwFlags         = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
        desc.dwBufferBytes   = size;
        desc.lpwfxFormat     = &fmt;
        desc.guid3DAlgorithm = DS3DALG_DEFAULT;
      }

      IDirectSoundBuffer *buffer_temp = NULL;
      hr = device->device->CreateSoundBuffer(&desc, &buffer_temp, NULL);
      if( FAILED(hr) ) {
        buffer = NULL;
        throw Exception("CreateSoundBuffer", hr);
      }

      hr = buffer_temp->QueryInterface(IID_IDirectSoundBuffer8, (void**)&buffer);
      buffer_temp->Release();
      buffer_temp = NULL;
      if( FAILED(hr) ) {
        buffer = NULL;
        throw Exception("QueryInterface", hr);
      }
    }

    ~BufferImpl()
    {
      if( buffer != NULL ) {
        buffer->Release();
        buffer = NULL;
      }
    }

    IDirectSoundBuffer8 *buffer;
  };

} // namespace Audio

#endif // __BUFFERIMPL_H__
