#include "binject.h"

void binject::In::bytes(char* begin, char* end)
{
    std::size_t count = end - begin;
    m_istream.read(begin, count);
}

void binject::uint16_le(binject::In& in, std::uint16_t& i)
{
    char a;
    char b;
    in.byte(a);
    in.byte(b);
    i = std::uint16_t(a) + std::uint16_t(b) << 8;
}

void binject::uint16_be(binject::In& in, std::uint16_t& i)
{
    char a;
    char b;
    in.byte(b);
    in.byte(a);
    i = std::uint16_t(a) + std::uint16_t(b) << 8;
}

void binject::uint32_le(binject::In& in, std::uint32_t& i)
{
    char a;
    char b;
    char c;
    char d;
    in.byte(a);
    in.byte(b);
    in.byte(c);
    in.byte(d);
    std::uint32_t n = 0;
    n += d;
    n <<= 8;
    n += c;
    n <<= 8;
    n += b;
    n <<= 8;
    n += a;
    i = n;
}

void binject::uint32_be(binject::In& in, std::uint32_t& i)
{
    char a;
    char b;
    char c;
    char d;
    in.byte(d);
    in.byte(c);
    in.byte(b);
    in.byte(a);
    std::uint32_t n = 0;
    n += d;
    n <<= 8;
    n += c;
    n <<= 8;
    n += b;
    n <<= 8;
    n += a;
    i = n;
}

void binject::uint64_le(binject::In& in, std::uint64_t& out)
{
    char bytes[8];
    for (int i = 0; i < 8; ++i) {
        in.byte(bytes[i]);
    }
    std::uint64_t n = bytes[7];
    for (int i = 6; i >= 0; --i) {
        n <<= 8;
        n += bytes[i];
    }
    out = n;
}

void binject::uint64_be(binject::In& in, std::uint64_t& out)
{
    char first_byte;
    in.byte(first_byte);
    std::uint64_t n = first_byte;
    for (int i = 0; i < 7; ++i) {
        n <<= 8;
        char byte;
        in.byte(byte);
        n += byte;
    }
    out = n;
}

void binject::Out::bytes(const char* begin, const char* end)
{
    std::size_t count = end - begin;
    m_ostream.write(begin, count);
}

void binject::uint16_le(binject::Out& out, std::uint16_t in)
{
    char a = in & 0xff;
    char b = (in >> 8) & 0xff;
    out.byte(a);
    out.byte(b);
}

void binject::uint16_be(binject::Out& out, std::uint16_t in)
{
    char a = in & 0xff;
    char b = (in >> 8) & 0xff;
    out.byte(b);
    out.byte(a);
}

void binject::uint32_le(binject::Out& out, std::uint32_t in)
{
    char a = in & 0xff;
    in >>= 8;
    char b = in & 0xff;
    in >>= 8;
    char c = in & 0xff;
    in >>= 8;
    char d = in;
    out.byte(a);
    out.byte(b);
    out.byte(c);
    out.byte(d);
}

void binject::uint32_be(binject::Out& out, std::uint32_t in)
{
    char a = in & 0xff;
    in >>= 8;
    char b = in & 0xff;
    in >>= 8;
    char c = in & 0xff;
    in >>= 8;
    char d = in;
    out.byte(d);
    out.byte(c);
    out.byte(b);
    out.byte(a);
}

void binject::uint64_le(binject::Out& out, std::uint64_t in)
{
    for (int i = 0; i < 8; ++i) {
        char b = in & 0xff;
        i >>= 8;
        out.byte(b);
    }
}

void binject::uint64_be(binject::Out& out, std::uint64_t in)
{
    char bytes[8];
    for (int i = 0; i < 8; ++i) {
        bytes[i] = in & 0xff;
        i >>= 8;
    }
    for (int i = 7; i >= 0; --i) {
        out.byte(bytes[i]);
    }
}
