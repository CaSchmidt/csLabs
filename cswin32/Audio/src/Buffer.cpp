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

#include "Audio/Buffer.h"

#include "Audio/Device.h"
#include "internal/BufferImpl.h"
#include "internal/DeviceImpl.h"

namespace Audio {

  ////// public //////////////////////////////////////////////////////////////

  Buffer::~Buffer()
  {
  }

  bool Buffer::isNull() const
  {
    return !d;
  }

  void Buffer::clear()
  {
    d.reset(nullptr);
  }

  BufferImpl *Buffer::impl() const
  {
    return d.get();
  }

  void Buffer::play(const bool looping) const
  {
    DWORD flags = 0;
    if( looping ) {
      flags |= DSBPLAY_LOOPING;
    }
    d->buffer->Play(0, 0, flags);
  }

  void Buffer::stop() const
  {
    d->buffer->Stop();
  }

  float Buffer::volume() const
  {
    LONG dxdb;
    HRESULT hr = d->buffer->GetVolume(&dxdb);
    if( FAILED(hr) ) {
      return -1.0f;
    }

    if(        dxdb >= DSBVOLUME_MAX ) {
      return 1.0f;
    } else if( dxdb <= DSBVOLUME_MIN ) {
      return 0.0f;
    }

    return std::powf(10.0f, (float)dxdb/2000.0f);
  }

  void Buffer::setVolume(float linear) const
  {
    if(        linear < 0.0f ) {
      linear = 0.0f;
    } else if( linear > 1.0f ) {
      linear = 1.0f;
    }

    if( linear <= 0.0f ) {
      d->buffer->SetVolume(DSBVOLUME_MIN);
      return;
    }

    const LONG dxdb = (LONG)std::roundf(2000.0f*std::log10f(linear));
    d->buffer->SetVolume(dxdb);
  }

  bool Buffer::setData(const void *data, const dword_t size)
  {
    HRESULT hr;

    void *data1;
    DWORD bytes1;
    void *data2;
    DWORD bytes2;
    hr = d->buffer->Lock(0, 0, &data1, &bytes1, &data2, &bytes2, DSBLOCK_ENTIREBUFFER);
    if( FAILED(hr) ) {
      return false;
    }

    const unsigned char *src = reinterpret_cast<const unsigned char*>(data);

    const DWORD block1 = size < bytes1  ?  size : bytes1;
    std::memcpy(data1, src, block1);

    if( data2 != NULL  &&  size > bytes1 ) {
      const DWORD remain = size - bytes1;
      const DWORD block2 = remain < bytes2  ?  remain : bytes2;
      std::memcpy(data2, src + bytes1, block2);
    }

    hr = d->buffer->Unlock(data1, bytes1, data2, bytes2);
    if( FAILED(hr) ) {
      return false;
    }

    return true;
  }

  Buffer Buffer::create(const Device& device,
                        const word_t bits, const word_t channels,
                        const dword_t frequency, const dword_t size)
  {
    Buffer result;

    try {
      result.d = std::make_unique<BufferImpl>(device.impl(),
                                              bits, channels, frequency, size);
    } catch(...) {
      result.clear();
      return result;
    }

    return result;
  }

  ////// private /////////////////////////////////////////////////////////////

  Buffer::Buffer(BufferImpl *ptr)
    : d(ptr)
  {
  }

} // namespace Audio
