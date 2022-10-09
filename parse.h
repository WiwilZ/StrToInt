/*
 * Created by WiwilZ on 2022/7/17.
 */

#pragma

#include <concepts>
#include <cstdint>


namespace {
	template <std::unsigned_integral ReturnType>
	consteval ReturnType power(auto base, auto exponent) noexcept {
		ReturnType ans = 1;
		while (exponent) {
			if (exponent & 1) {
				ans *= base;
				base *= base;
			}
			exponent >>= 1;
		}
		return ans;
	}

	template <std::size_t n_bytes>
	struct uint;

	template <>
	struct uint<4> {
		using type = uint32_t;
		static constexpr type mask0 = 0x40404040;
		static constexpr type mask1 = 0x0f0f0f0f;
	};

	template <>
	struct uint<8> {
		using type = uint64_t;
		static constexpr type mask0 = 0x4040404040404040;
		static constexpr type mask1 = 0x0f0f0f0f0f0f0f0f;
	};

	template <>
	struct uint<16> {
		using type = __uint128_t;
		static constexpr type mask0 = static_cast<__uint128_t>(0x4040404040404040) << 64 | 0x4040404040404040;
		static constexpr type mask1 = static_cast<__uint128_t>(0x0f0f0f0f0f0f0f0f) << 64 | 0x0f0f0f0f0f0f0f0f;
	};

	template <std::size_t base, std::size_t n_chars>
	requires (2 <= base && base <= 36)
	inline decltype(auto) Impl(const char* str) {
		auto t = *reinterpret_cast<const typename uint<n_chars>::type*>(str);
		if constexpr (base >= 11) {
			auto mask = t & uint<n_chars>::mask0;
			if constexpr (base >= 17) {
				t &= mask >> 2 | uint<n_chars>::mask1;
			}
			t += mask >> 3 | mask >> 6;
		}
		return t;
	}
}

/*----------------------------------------2----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires (base == 2 && n_chars == 4)
inline uint8_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = t << 3 | t >> 6;
	t |= t >> 18;
	return static_cast<uint8_t>(t & 0xf);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 2 && n_chars == 8)
inline uint8_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t << 7 | t >> 2) & 0x00c000c000c000c0;
	t |= t >> 18;
	t |= t >> 36;
	return static_cast<uint8_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 2 && n_chars == 16)
inline uint16_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t << 15 | t << 6) & (static_cast<__uint128_t>(0xc000c000c000c000) << 64 | 0xc000c000c000c000);
	t |= t >> 18;
	t |= t >> 36;
	t |= t >> 72;
	return static_cast<uint16_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 2 && n_chars == 32)
inline uint32_t parse(const char* str) noexcept {
	return static_cast<uint32_t>(parse<2, 16>(str)) << 16 | parse<2, 16>(str + 16);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 2 && n_chars == 64)
inline uint64_t parse(const char* str) noexcept {
	return static_cast<uint64_t>(parse<2, 32>(str)) << 32 | parse<2, 32>(str + 32);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 2 && n_chars == 128)
inline __uint128_t parse(const char* str) noexcept {
	return static_cast<__uint128_t>(parse<2, 64>(str)) << 64 | parse<2, 64>(str + 64);
}
/*----------------------------------------2----------------------------------------*/


/*----------------------------------------4----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires (base == 4 && n_chars == 4)
inline uint8_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t << 6 | t >> 4) & 0x00f000f0;
	t |= t >> 20;
	return static_cast<uint8_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 4 && n_chars == 8)
inline uint16_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t << 14 | t << 4) & 0xf000f000f000f000;
	t |= t >> 20;
	t |= t >> 40;
	return static_cast<uint16_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 4 && n_chars == 16)
inline uint32_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t << 6 | t >> 4) & (static_cast<__uint128_t>(0x00f000f000f000f0) << 64 | 0x00f000f000f000f0);
	t = (t | t >> 20) << 24;
	t = (t | t >> 40) & (static_cast<__uint128_t>(0x00000000ffff0000) << 64 | 0x00000000ffff000);
	t = t >> 80;
	return static_cast<uint32_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 4 && n_chars == 32)
inline uint64_t parse(const char* str) noexcept {
	return static_cast<uint64_t>(parse<4, 16>(str)) << 32 | parse<4, 16>(str + 16);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 4 && n_chars == 64)
inline __uint128_t parse(const char* str) noexcept {
	return static_cast<__uint128_t>(parse<4, 32>(str)) << 32 | parse<4, 32>(str + 32);
}
/*----------------------------------------4----------------------------------------*/


/*----------------------------------------8----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires (base == 8 && n_chars == 4)
inline uint16_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t &= 0x07070707;
	t = (t << 9 | t >> 2) & 0x0fc00fc0;
	t |= t >> 2;
	return static_cast<uint16_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 8 && n_chars == 8)
inline uint32_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t &= 0x0707070707070707;
	t = (t | t << 11) & 0x3f003f003f003f00;
	t = (t | t >> 22) & 0x00003ffc00003ffc;
	t = t << 10 | t >> 34;
	return static_cast<uint32_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 8 && n_chars == 16)
inline uint64_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t &= static_cast<__uint128_t>(0x0707070707070707) << 64 | 0x0707070707070707;
	t = (t | t << 11) & (static_cast<__uint128_t>(0x3f003f003f003f00) << 64 | 0x3f003f003f003f00);
	t = (t | t >> 22) & (static_cast<__uint128_t>(0x00003ffc00003ffc) << 64 | 0x00003ffc00003ffc);
	t = (t << 34 | t >> 10) & (static_cast<__uint128_t>(0x0000ffffff000000) << 64 | 0x0000ffffff000000);
	t |= t >> 88;
	return static_cast<uint64_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 8 && n_chars == 32)
inline __uint128_t parse(const char* str) noexcept {
	return static_cast<__uint128_t>(parse<8, 16>(str)) << 48 | parse<8, 16>(str + 16);
}
/*----------------------------------------8----------------------------------------*/


/*----------------------------------------16----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires (base == 16 && n_chars == 4)
inline uint16_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t &= 0x0f0f0f0f;
	t = (t | t << 12) & 0xff00ff00;
	t |= t >> 24;
	return static_cast<uint16_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 16 && n_chars == 8)
inline uint32_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t &= 0x0f0f0f0f0f0f0f0f;
	t = (t | t << 12) & 0xff00ff00ff00ff00;
	t = ((t | t >> 24) & 0x0000ffff0000ffff) << 16;
	t |= t >> 48;
	return static_cast<uint32_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 16 && n_chars == 16)
inline uint64_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t &= static_cast<__uint128_t>(0x0f0f0f0f0f0f0f0f) << 64 | 0x0f0f0f0f0f0f0f0f;
	t = (t | t << 12) & (static_cast<__uint128_t>(0xff00ff00ff00ff00) << 64 | 0xff00ff00ff00ff00);
	t = ((t | t >> 24) & (static_cast<__uint128_t>(0x0000ffff0000ffff) << 64 | 0x0000ffff0000ffff)) << 16;
	t = ((t | t >> 48) & (static_cast<__uint128_t>(0x00000000ffffffff) << 64 | 0x00000000ffffffff)) << 32;
	t |= t >> 96;
	return static_cast<uint64_t>(t);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 16 && n_chars == 32)
inline __uint128_t parse(const char* str) noexcept {
	return static_cast<__uint128_t>(parse<16, 16>(str)) << 64 | parse<16, 16>(str + 16);
}
/*----------------------------------------16----------------------------------------*/


/*----------------------------------------32----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires (base == 32 && n_chars == 4)
inline uint32_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = t << 5 & 0x03e003e0 | t >> 8 & 0x001f001f;
	t = (t << 10 | t >> 16) & 0x000fffff;
	return t;
}

template <std::size_t base, std::size_t n_chars>
requires (base == 32 && n_chars == 8)
inline uint64_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = t << 5 & 0x03e003e003e003e0 | t >> 8 & 0x001f001f001f001f;
	t = t << 10 & 0x000ffcc0000ffcc0 | t >> 16 & 0x000003ff000003ff;
	t = (t << 20 | t >> 32) & 0xffffffffff;
	return t;
}

template <std::size_t base, std::size_t n_chars>
requires (base == 32 && n_chars == 16)
inline __uint128_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = t << 5 & (static_cast<__uint128_t>(0x03e003e003e003e0) << 64 | 0x03e003e003e003e0) | t >> 8 & (static_cast<__uint128_t>(0x001f001f001f001f) << 64 | 0x001f001f001f001f);
	t = t << 10 & (static_cast<__uint128_t>(0x000ffcc0000ffcc0) << 64 | 0x000ffcc0000ffcc0) | t >> 16 & (static_cast<__uint128_t>(0x000003ff000003ff) << 64 | 0x000003ff000003ff);
	t = t << 20 & (static_cast<__uint128_t>(0x000000fffff00000) << 64 | 0x000000fffff00000) | t >> 32 & (static_cast<__uint128_t>(0x00000000000fffff) << 64 | 0x00000000000fffff);
	t = (t << 40 | t >> 64) & (static_cast<__uint128_t>(0xffffffffffffffff) << 64 | 0xffff);
	return t;
}
/*----------------------------------------32----------------------------------------*/


/*----------------------------------------3----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires (base == 3 && n_chars == 4)
inline uint8_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t & 0x000f000f) * 3 + (t >> 8 & 0x000f000f);
	return static_cast<uint8_t>(t) * 9 + static_cast<uint8_t>(t >> 16);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 3 && n_chars == 8)
inline uint16_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	t = (t & 0x000f000f000f000f) * 3 + (t >> 8 & 0x000f000f000f000f);
	t = (t & 0x0000ffff0000ffff) * 9 + (t >> 16 & 0x0000ffff0000ffff);
	return static_cast<uint16_t>(t) * 81 + static_cast<uint16_t>(t >> 32);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 3 && n_chars == 16)
inline uint32_t parse(const char* str) noexcept {
	auto t = Impl<base, n_chars>(str);
	constexpr auto m0 = static_cast<__uint128_t>(0x000f000f000f000f) << 64 | 0x000f000f000f000f;
	t = (t & m0) * 3 + (t >> 8 & m0);
	constexpr auto m1 = static_cast<__uint128_t>(0x0000ffff0000ffff) << 64 | 0x0000ffff0000ffff;
	t = (t & m1) * 9 + (t >> 16 & m1);
	constexpr auto m2 = static_cast<__uint128_t>(0x00000000ffffffff) << 64 | 0x00000000ffffffff;
	t = (t & m2) * 81 + (t >> 32 & m2);
	return static_cast<uint32_t>(t) * 6561 + static_cast<uint32_t>(t >> 64);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 3 && n_chars == 32)
inline uint64_t parse(const char* str) noexcept {
	constexpr auto r16 = power<uint64_t>(base, 16);
	return static_cast<uint64_t>(parse<3, 16>(str)) * r16 + parse<3, 16>(str + 16);
}

template <std::size_t base, std::size_t n_chars>
requires (base == 3 && n_chars == 64)
inline __uint128_t parse(const char* str) noexcept {
	constexpr auto r32 = power<__uint128_t>(base, 32);
	return static_cast<__uint128_t>(parse<3, 32>(str)) * r32 + parse<3, 32>(str + 32);
}
/*----------------------------------------3----------------------------------------*/


/*----------------------------------------[5, 7]U[9, 15]----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires ((5 <= base && base <= 7 || 9 <= base && base <= 15) && n_chars == 4)
inline uint16_t parse(const char* str) noexcept {
	constexpr uint16_t r1 = base;
	constexpr uint16_t r2 = r1 * r1;

	auto t = Impl<base, n_chars>(str);
	t = (t & 0x000f000f) * r1 + (t >> 8 & 0x000f000f);
	return static_cast<uint16_t>(t) * r2 + static_cast<uint16_t>(t >> 16);
}

template <std::size_t base, std::size_t n_chars>
requires ((5 <= base && base <= 7 || 9 <= base && base <= 15) && n_chars == 8)
inline uint32_t parse(const char* str) noexcept {
	constexpr uint32_t r1 = base;
	constexpr uint32_t r2 = r1 * r1;
	constexpr uint32_t r4 = r2 * r2;

	auto t = Impl<base, n_chars>(str);
	t = (t & 0x000f000f000f000f) * r1 + (t >> 8 & 0x000f000f000f000f);
	t = (t & 0x0000ffff0000ffff) * r2 + (t >> 16 & 0x0000ffff0000ffff);
	return static_cast<uint32_t>(t) * r4 + static_cast<uint32_t>(t >> 32);
}

template <std::size_t base, std::size_t n_chars>
requires ((5 <= base && base <= 7 || 9 <= base && base <= 15) && n_chars == 16)
inline uint64_t parse(const char* str) noexcept {
	constexpr uint64_t r1 = base;
	constexpr uint64_t r2 = r1 * r1;
	constexpr uint64_t r4 = r2 * r2;
	constexpr uint64_t r8 = r4 * r4;

	auto t = Impl<base, n_chars>(str);
	constexpr auto m0 = static_cast<__uint128_t>(0x000f000f000f000f) << 64 | 0x000f000f000f000f;
	t = (t & m0) * r1 + (t >> 8 & m0);
	constexpr auto m1 = static_cast<__uint128_t>(0x0000ffff0000ffff) << 64 | 0x0000ffff0000ffff;
	t = (t & m1) * r2 + (t >> 16 & m1);
	constexpr auto m2 = static_cast<__uint128_t>(0x00000000ffffffff) << 64 | 0x00000000ffffffff;
	t = (t & m2) * r4 + (t >> 32 & m2);
	return static_cast<uint64_t>(t) * r8 + static_cast<uint64_t>(t >> 64);
}

template <std::size_t base, std::size_t n_chars>
requires ((5 <= base && base <= 7 || 9 <= base && base <= 15) && n_chars == 32)
inline __uint128_t parse(const char* str) noexcept {
	constexpr auto r16 = power<__uint128_t>(base, 16);
	return static_cast<__uint128_t>(parse<base, 16>(str)) * r16 + parse<base, 16>(str + 16);
}
/*----------------------------------------[5, 7]U[9, 15]----------------------------------------*/


/*----------------------------------------[17, 31]U[33, 36]----------------------------------------*/
template <std::size_t base, std::size_t n_chars>
requires ((17 <= base && base <= 31 || 33 <= base && base <= 36) && n_chars == 4)
inline uint32_t parse(const char* str) noexcept {
	constexpr uint32_t r1 = base;
	constexpr uint32_t r2 = r1 * r1;

	auto t = Impl<base, n_chars>(str);
	t = (t & 0x00ff00ff) * r1 + (t >> 8 & 0x00ff00ff);
	t = (t & 0x0000ffff) * r2 + (t >> 16 & 0x0000ffff);
	return t;
}

template <std::size_t base, std::size_t n_chars>
requires ((17 <= base && base <= 31 || 33 <= base && base <= 36) && n_chars == 8)
inline uint64_t parse(const char* str) noexcept {
	constexpr uint64_t r1 = base;
	constexpr uint64_t r2 = r1 * r1;
	constexpr uint64_t r4 = r2 * r2;

	auto t = Impl<base, n_chars>(str);
	t = (t & 0x00ff00ff00ff00ff) * r1 + (t >> 8 & 0x00ff00ff00ff00ff);
	t = (t & 0x0000ffff0000ffff) * r2 + (t >> 16 & 0x0000ffff0000ffff);
	t = (t & 0x00000000ffffffff) * r4 + (t >> 32 & 0x00000000ffffffff);
	return t;
}

template <std::size_t base, std::size_t n_chars>
requires ((17 <= base && base <= 31 || 33 <= base && base <= 36) && n_chars == 16)
inline __uint128_t parse(const char* str) noexcept {
	constexpr __uint128_t r1 = base;
	constexpr __uint128_t r2 = r1 * r1;
	constexpr __uint128_t r4 = r2 * r2;
	constexpr __uint128_t r8 = r4 * r4;

	auto t = Impl<base, n_chars>(str);
	constexpr auto m0 = static_cast<__uint128_t>(0x00ff00ff00ff00ff) << 64 | 0x00ff00ff00ff00ff;
	t = (t & m0) * r1 + (t >> 8 & m0);
	constexpr auto m1 = static_cast<__uint128_t>(0x0000ffff0000ffff) << 64 | 0x0000ffff0000ffff;
	t = (t & m1) * r2 + (t >> 16 & m1);
	constexpr auto m2 = static_cast<__uint128_t>(0x00000000ffffffff) << 64 | 0x00000000ffffffff;
	t = (t & m2) * r4 + (t >> 32 & m2);
	t = static_cast<uint64_t>(t) * r8 + static_cast<uint64_t>(t >> 64);
	return t;
}
/*----------------------------------------[17, 31]U[33, 36]----------------------------------------*/
