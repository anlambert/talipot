// The MIT License (MIT)
//
// Copyright (c) 2020 hadesragon@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef ZST_STREAM_H_
#define ZST_STREAM_H_

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

#include <iostream>
#include <memory>
#include <vector>

#define DEFAULT_COMPRESS_LEVEL 10

namespace zstd {

class ZstdOStream;

namespace detail {

using CStreamPtr = std::unique_ptr<ZSTD_CStream, decltype(&ZSTD_freeCStream)>;
using DStreamPtr = std::unique_ptr<ZSTD_DStream, decltype(&ZSTD_freeDStream)>;

class ZstdOStreamBuf : public std::streambuf
{
  friend class zstd::ZstdOStream;
public:
    explicit ZstdOStreamBuf(std::streambuf* buffer, int compression_level=DEFAULT_COMPRESS_LEVEL)
        : buffer_(buffer)
        , zstd_stream_(ZSTD_createCStream(), ZSTD_freeCStream)
        , in_chunk_(ZSTD_CStreamInSize())
        , out_chunk_size_(ZSTD_CStreamOutSize())
        , out_chunk_(out_chunk_size_)

    {
#if ZSTD_VERSION_NUMBER >= 10400
        ZSTD_CCtx_setParameter(zstd_stream_.get(), ZSTD_c_compressionLevel, compression_level);
        ZSTD_CCtx_setParameter(zstd_stream_.get(), ZSTD_c_checksumFlag, 1);
        ZSTD_CCtx_setParameter(zstd_stream_.get(), ZSTD_c_nbWorkers, 4);
#else
        ZSTD_initCStream(zstd_stream_.get(), compression_level);
#endif

        setp(in_chunk_.data(), in_chunk_.data() + in_chunk_.size() - 1);
    }
    ~ZstdOStreamBuf() override = default;

protected:

    void flush_stream(ZSTD_EndDirective mode = ZSTD_e_continue) {
        std::size_t num_bytes = std::distance(pbase(), pptr());
        ZSTD_inBuffer input {pbase(), num_bytes, 0};
        bool finished;
        do {
            ZSTD_outBuffer output = {out_chunk_.data(), out_chunk_size_, 0};
#if ZSTD_VERSION_NUMBER >= 10308
            auto remain = ZSTD_compressStream2(zstd_stream_.get(), &output, &input, mode);
#else
            auto remain = ZSTD_compress_generic(zstd_stream_.get(), &output, &input, mode);
#endif
            buffer_->sputn(out_chunk_.data(), output.pos);
            finished = (mode == ZSTD_e_end ? remain == 0 : input.pos == input.size);
        } while (!finished);
        setp(in_chunk_.data(), in_chunk_.data() + in_chunk_.size() - 1);
    }

    int overflow(int ch) override {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
        flush_stream();
        return ch;
    }

    virtual int sync() override {
        flush_stream();
        return buffer_->pubsync();
    }

private:
    std::streambuf*         buffer_;
    CStreamPtr              zstd_stream_;
    std::vector<char>       in_chunk_;
    std::size_t             out_chunk_size_;
    std::vector<char>       out_chunk_;
};

class ZstdIStreamBuf : public std::streambuf
{
public:
    explicit ZstdIStreamBuf(std::streambuf* buffer)
        : buffer_(buffer)
        , zstd_stream_(ZSTD_createDStream(), ZSTD_freeDStream)
        , in_chunk_(ZSTD_DStreamInSize())
        , out_chunk_(ZSTD_DStreamOutSize())
        , out_offset_(0)
        , out_size_(0)
    {
        ZSTD_initDStream(zstd_stream_.get());
    }
    ~ZstdIStreamBuf() override = default;

protected:
    virtual int underflow() override {
        while (true) {
            if(out_offset_ == out_size_) {
                out_size_ = buffer_->sgetn(in_chunk_.data(), in_chunk_.size());
                out_offset_ = 0;
            }

            if(out_size_ == 0) {
                return traits_type::eof();
            }

            ZSTD_inBuffer input = {in_chunk_.data(), out_size_, out_offset_};
            ZSTD_outBuffer output = {out_chunk_.data(), static_cast<size_t>(out_chunk_.size()), 0};
            size_t ret = ZSTD_decompressStream(zstd_stream_.get(), &output, &input);
            out_offset_ = input.pos;

            if (ZSTD_isError(ret) != 0) {
                throw std::runtime_error(ZSTD_getErrorName(ret));
            }

            if(output.pos > 0) {
                setg(out_chunk_.data(), out_chunk_.data(), out_chunk_.data() + output.pos);
                return traits_type::to_int_type(*gptr());
            }
        }
        return traits_type::eof();
    }

private:
    std::streambuf*     buffer_;
    DStreamPtr          zstd_stream_;
    std::vector<char>   in_chunk_;
    std::vector<char>   out_chunk_;
    std::size_t         out_offset_;
    std::size_t         out_size_;
};

using StreamBufPtr = std::unique_ptr<std::streambuf>;
class ZstdStreamBase
{
public:
    explicit ZstdStreamBase(std::ostream& stream, int level)
        : buf_(new ZstdOStreamBuf(stream.rdbuf(), level))
    {}
    explicit ZstdStreamBase(std::istream& stream)
        : buf_(new ZstdIStreamBuf(stream.rdbuf()))
    {}
    ~ZstdStreamBase() = default;

protected:
    StreamBufPtr        buf_;
};

} // namespace detail

class ZstdOStream: public detail::ZstdStreamBase, public std::ostream
{
  std::ostream *stream_;

  void flush_stream() {
    static_cast<detail::ZstdOStreamBuf*>(buf_.get())->flush_stream(ZSTD_e_end);
  }

public:
    explicit ZstdOStream(std::ostream& stream, int level = DEFAULT_COMPRESS_LEVEL)
        : detail::ZstdStreamBase(stream, level)
        , std::ostream(buf_.get())
        , stream_(nullptr)
    {
      clear(stream.rdstate());
    }

    explicit ZstdOStream(std::ostream* stream, int level = DEFAULT_COMPRESS_LEVEL)
        : detail::ZstdStreamBase(*stream, level)
        , std::ostream(buf_.get())
        , stream_(stream)
    {
      clear(stream->rdstate());
    }

    ~ZstdOStream() override {
      flush_stream();
      delete stream_;
    }


};

class ZstdIStream: public detail::ZstdStreamBase, public std::istream
{
  std::istream *stream_;
public:
    explicit ZstdIStream(std::istream& stream)
        : detail::ZstdStreamBase(stream)
        , std::istream(buf_.get())
        , stream_(nullptr)
    {
      clear(stream.rdstate());
    }

    explicit ZstdIStream(std::istream* stream)
        : detail::ZstdStreamBase(*stream)
        , std::istream(buf_.get())
        , stream_(stream)
    {
      clear(stream->rdstate());
    }

    ~ZstdIStream() override {
      delete stream_;
    }
};

} // namespace zstd

#endif // ZST_STREAM_H_
