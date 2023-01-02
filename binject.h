#ifndef BINFMT_H
#define BINFMT_H

#include <array>
#include <cstdint>
#include <istream>
#include <ostream>

namespace binject {
struct In
{
    template <class T> using small_type = T&;
    template <class T> using pointer_type = T*;
    template <class T> using large_type = T&;

    In(std::istream& istream) : m_istream(istream) {}

    void byte(char& ch) { m_istream.get(ch); }

    void bytes(char* begin, char* end);

private:
    std::istream& m_istream;
};

template <std::size_t N> bool constant(In& in, const std::array<char, N>& arr)
{
    std::array<char, N> buf;
    in.bytes(buf.data(), buf.data() + N);
    return buf == arr;
}

template <class T, class F> void if_in(In&, T& x, F f) { f(x); }

void uint16_le(In&, std::uint16_t&);

void uint16_be(In&, std::uint16_t&);

void uint32_le(In&, std::uint32_t&);

void uint32_be(In&, std::uint32_t&);

void uint64_le(In&, std::uint64_t&);

void uint64_be(In&, std::uint64_t&);

struct Out
{
    template <class T> using small_type = T;
    template <class T> using pointer_type = const T*;
    template <class T> using large_type = const T&;

    Out(std::ostream& ostream) : m_ostream(ostream) {}

    void byte(char ch) { m_ostream.put(ch); }

    void bytes(const char* begin, const char* end);

private:
    std::ostream& m_ostream;
};

template <std::size_t N> bool constant(Out& out, const std::array<char, N>& arr)
{
    out.bytes(arr.data(), arr.data() + N);
    return true;
}

template <class T, class F> void if_in(Out&, const T&, F) {}

void uint16_le(Out&, std::uint16_t);

void uint16_be(Out&, std::uint16_t);

void uint32_le(Out&, std::uint32_t);

void uint32_be(Out&, std::uint32_t);

void uint64_le(Out&, std::uint64_t);

void uint64_be(Out&, std::uint64_t);

} // namespace binject

#endif

// Local Variables:
// mode: c++
// End:
