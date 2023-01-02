#include "binject.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(Basic, Input)
{
    std::istringstream src("cat");
    binject::In in(src);
    char ch = 'a';
    in.byte(ch);
    EXPECT_EQ(ch, 'c');
}

TEST(Basic, Output)
{
    std::ostringstream dest;
    binject::Out out(dest);
    out.byte('c');
    EXPECT_EQ(dest.str(), "c");
}

TEST(Basic, InputBytes)
{
    std::istringstream src("cat");
    binject::In in(src);
    std::string s{"kitnip"};
    in.bytes(s.data(), s.data() + 3);
    EXPECT_EQ(s, "catnip");
}

TEST(Basic, OutputBytes)
{
    std::ostringstream dest;
    binject::Out out(dest);
    char bytes[] = {'c', 'a', 't'};
    out.bytes(bytes, bytes + 3);
    EXPECT_EQ(dest.str(), "cat");
}

TEST(Endian, Input16LE)
{
    std::istringstream src("cat");
    binject::In in(src);
    std::uint16_t i = 0;
    binject::uint16_le(in, i);
    EXPECT_EQ(i, 'c' + 'a' << 8);
}

TEST(Endian, Input16BE)
{
    std::istringstream src("cat");
    binject::In in(src);
    std::uint16_t i = 0;
    binject::uint16_be(in, i);
    EXPECT_EQ(i, 'a' + 'c' << 8);
}

TEST(Endian, Output16LE)
{
    std::ostringstream dest;
    binject::Out out(dest);
    binject::uint16_le(out, 0xabcd);
    EXPECT_EQ(dest.str(), u8"\xcd\xab");
}

TEST(Endian, Output16BE)
{
    std::ostringstream dest;
    binject::Out out(dest);
    binject::uint16_be(out, 0xabcd);
    EXPECT_EQ(dest.str(), u8"\xab\xcd");
}

TEST(Endian, Input32LE)
{
    std::istringstream src(u8"\x02\x04\x06\x08");
    binject::In in(src);
    std::uint32_t i = 0;
    binject::uint32_le(in, i);
    EXPECT_EQ(i, 0x08060402);
}

TEST(Endian, Input32BE)
{
    std::istringstream src(u8"\x02\x04\x06\x08");
    binject::In in(src);
    std::uint32_t i = 0;
    binject::uint32_be(in, i);
    EXPECT_EQ(i, 0x02040608);
}

TEST(Endian, Output32LE)
{
    std::ostringstream dest;
    binject::Out out(dest);
    binject::uint32_le(out, 0xabcd1234);
    EXPECT_EQ(dest.str(), u8"\x34\x12\xcd\xab");
}

TEST(Endian, Output32BE)
{
    std::ostringstream dest;
    binject::Out out(dest);
    binject::uint32_be(out, 0xabcd1234);
    EXPECT_EQ(dest.str(), u8"\xab\xcd\x12\x34");
}

TEST(Endian, Input64LE)
{
    std::istringstream src(u8"\x02\x04\x06\x08\x01\x03\x05\x07");
    binject::In in(src);
    std::uint64_t i = 0;
    binject::uint64_le(in, i);
    EXPECT_EQ(i, 0x0705030108060402);
}

TEST(Endian, Input64BE)
{
    std::istringstream src(u8"\x02\x04\x06\x08\x01\x03\x05\x07");
    binject::In in(src);
    std::uint64_t i = 0;
    binject::uint64_be(in, i);
    EXPECT_EQ(i, 0x0204060801030507);
}

struct Pair
{
    std::uint32_t x;
    std::uint32_t y;
};

template <class T>
bool stream_pair(T& stream, typename T::large_type<Pair> pair)
{
    if (!constant(stream, std::array<char, 4>({'p', 'a', 'i', 'r'}))) {
        return false;
    }
    binject::uint32_le(stream, pair.x);
    binject::uint32_le(stream, pair.y);
    return true;
}

TEST(ReadWrite, PairInOut)
{
    const Pair src = {1, 2};
    std::ostringstream sink;
    binject::Out out(sink);
    EXPECT_TRUE(stream_pair(out, src));
    Pair dest = {7, 8};
    std::istringstream source(sink.str());
    binject::In in(source);
    EXPECT_TRUE(stream_pair(in, dest));
    EXPECT_EQ(dest.x, src.x);
    EXPECT_EQ(dest.y, src.y);
}

struct Char4
{
    char chrs[4];
};

template <class T>
bool stream_char4(T& stream, typename T::large_type<Char4> char4)
{
    if (!constant(stream, std::array<char, 4>({'c', 'h', 'a', 'r'}))) {
        return false;
    }
    stream.bytes(char4.chrs, char4.chrs + 4);
    return true;
}

TEST(ReadWrite, BytesInOut)
{
    const Char4 src{{'a', 'b', 'c', 'd'}};
    std::ostringstream sink;
    binject::Out out(sink);
    EXPECT_TRUE(stream_char4(out, src));
    Char4 dest{{'w', 'x', 'y', 'z'}};
    std::istringstream source(sink.str());
    binject::In in(source);
    EXPECT_TRUE(stream_char4(in, dest));
    EXPECT_EQ(dest.chrs[0], src.chrs[0]);
    EXPECT_EQ(dest.chrs[1], src.chrs[1]);
    EXPECT_EQ(dest.chrs[2], src.chrs[2]);
    EXPECT_EQ(dest.chrs[3], src.chrs[3]);
}

struct Variadic
{
    char len;
    std::unique_ptr<char[]> bytes;
};

template <class T>
void stream_variadic(T& stream, typename T::large_type<Variadic> v)
{
    stream.byte(v.len);
    binject::if_in(stream, v, [&stream](auto& v) {
        v.bytes = std::unique_ptr<char[]>(new char[v.len]);
    });
    stream.bytes(v.bytes.get(), v.bytes.get() + v.len);
}

TEST(ReadWrite, Variadic)
{
    const std::string original{"\x04\x01\x02\x03\x04"};
    std::istringstream source(original);
    binject::In in(source);
    Variadic v;
    stream_variadic(in, v);
    EXPECT_EQ(v.len, 4);
    EXPECT_EQ(v.bytes[0], 1);
    EXPECT_EQ(v.bytes[1], 2);
    EXPECT_EQ(v.bytes[2], 3);
    EXPECT_EQ(v.bytes[3], 4);
    std::ostringstream dest;
    binject::Out out(dest);
    stream_variadic(out, v);
    EXPECT_EQ(dest.str(), original);
}
