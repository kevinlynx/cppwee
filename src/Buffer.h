//
// file: Buffer.h
// author: Kevin Lynx
// 
#ifndef KLBUFFER_H
#define KLBUFFER_H

#include <assert.h>
#include <string>
#include <vector>

class ReadBuffer {
public:
    virtual ~ReadBuffer() {
    }

    virtual bool read(void *out, unsigned size) = 0;

    template <typename T>
    T read() {
        T ret;
        if (!read(&ret, sizeof(T)))
            return T();
        return ret;
    }

    std::string readString() {
        unsigned len = readSize();
        std::string ret;
        if (len == 0) return ret;
        ret.resize(len);
        read((char*) ret.c_str(), len);
        return ret;
    }

    unsigned readSize() {
        unsigned char f = read<unsigned char>();
        return f == 0xff ? read<int>() : f;
    }

    template <typename T>
    T readIfHas() {
        unsigned char f = read<unsigned char>();
        return f > 0 ? read<T>() : T();
    }
};

class WriteBuffer {
public:
    virtual ~WriteBuffer() {
    }

    virtual void write(const void *buf, unsigned size) = 0;

    template <typename T>
    void write(T v) {
        write(&v, sizeof(v));
    }

    void writeString(const std::string &s) {
        writeSize(s.size());
        write(s.c_str(), s.size());
    }

    void writeSize(unsigned size) {
        if (size > 255) {
            write<unsigned char>(0xff);
            write(size);
        } else {
            write<unsigned char>((unsigned char) size);
        }
    }

    template <typename T>
    void writeIfHas(const T &v) {
        if (v) {
            write<unsigned char>(1);
            write(&v, sizeof(T));
        } else {
            write<unsigned char>(0);
        }
    }
};

typedef std::vector<unsigned char> ByteBuffer;

class VectorReadBuffer : public ReadBuffer {
public:
    VectorReadBuffer(const void *buf, unsigned size) : m_buf(buf), m_size(size) {
        m_offset = 0;
    }

    VectorReadBuffer(const ByteBuffer &buf) {
        assert(buf.size() > 0);
        m_buf = &buf[0];
        m_size = buf.size();
        m_offset = 0;
    }

    virtual bool read(void *out, unsigned size) {
        if (m_offset + size > m_size) return false;
        memcpy(out, (const char *)m_buf + m_offset, size);
        m_offset += size;
        return true;
    }

private:
    const void *m_buf;
    unsigned m_size;
    unsigned m_offset;
};

class VectorWriteBuffer : public WriteBuffer {
public:
    VectorWriteBuffer(ByteBuffer &buf, unsigned reserved = 256) : m_bytes(buf) {
        m_bytes.reserve(reserved);
    }

    virtual void write(const void *buf, unsigned size) {
        size_t start = m_bytes.size();
        m_bytes.resize(start + size);
        memcpy(&m_bytes[0] + start, buf, size);
    }

private:
    ByteBuffer &m_bytes;
};

#endif // KLBUFFER_H
