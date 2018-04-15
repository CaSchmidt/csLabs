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

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <memory>

#include <Audio/Types.h>

namespace Audio {

  class BufferImpl;
  class Device;

  class Buffer {
  public:
    ~Buffer();

    Buffer(Buffer&&) = default;
    Buffer& operator=(Buffer&&) = default;

    bool isNull() const;
    void clear();
    BufferImpl *impl() const;

    void play(const bool looping = false) const;
    void stop() const;

    float volume() const;
    void setVolume(float linear) const;

    bool setData(const void *data, const dword_t size);

    static Buffer create(const Device& device,
                         const word_t bits, const word_t channels,
                         const dword_t frequency, const dword_t size);

  private:
    Buffer(BufferImpl *ptr = nullptr);

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    std::unique_ptr<BufferImpl> d;
  };

} // namespace Audio

#endif // __BUFFER_H__
