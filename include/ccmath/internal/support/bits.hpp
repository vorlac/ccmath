/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

// Support header that brings C++20's <bit> header to C++17.

#pragma once

#include "ccmath/internal/config/type_support.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "ccmath/internal/math/runtime/simd/simd_vectorize.hpp"
#include "ccmath/internal/predef/has_builtin.hpp"
#include "ccmath/internal/support/ctz.hpp"
#include "ccmath/internal/support/is_constant_evaluated.hpp"
#include "ccmath/internal/support/type_traits.hpp"

#include <cstdint>

#if defined(_MSC_VER) && !defined(__clang__)
	#include <cstdlib>
#endif

namespace ccm::support
{
	template <typename To, typename From>
	[[nodiscard]] constexpr std::enable_if_t<
		sizeof(To) == sizeof(From) && std::is_trivially_constructible_v<To> && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>, To>
	// ReSharper disable once CppDFAUnreachableFunctionCall
	bit_cast(const From & from) noexcept
	{
		return __builtin_bit_cast(To, from);
	}

	template <class T,
	          std::enable_if_t<traits::ccm_is_integral_v<T> && traits::ccm_is_unsigned_v<T> && !traits::is_char_v<T> && !std::is_same_v<T, bool>, bool>  = true>
	constexpr bool has_single_bit(T x) noexcept
	{
		return x && !(x & (x - 1));
	}

	// TODO: Have the below function replace all other top_bits func.

	template <typename T, std::size_t TopBitsToTake, std::enable_if_t<std::is_floating_point_v<T> && !std::is_same_v<T, long double>, bool>  = true>
	constexpr std::uint32_t top_bits(T x) noexcept
	{
		// This function does not work with long double. May support it later though.
		if constexpr (std::is_same_v<T, double>)
		{
			return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> (std::numeric_limits<std::uint64_t>::digits - TopBitsToTake));
		}
		else { return bit_cast<std::uint32_t>(x) >> (std::numeric_limits<std::uint32_t>::digits - TopBitsToTake); }
	}

	/**
	 * @brief Helper function to get the top 16-bits of a double.
	 * @param x Double to get the bits from.
	 * @return
	 */
	constexpr std::uint32_t top16_bits_of_double(double x) noexcept
	{
		return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> 48);
	}

	constexpr std::uint32_t top12_bits_of_double(double x) noexcept
	{
		return static_cast<std::uint32_t>(bit_cast<std::uint64_t>(x) >> 52);
	}

	constexpr std::uint32_t top12_bits_of_float(float x) noexcept
	{
		return bit_cast<std::uint32_t>(x) >> 20;
	}

	constexpr std::uint64_t double_to_uint64(double x) noexcept
	{
		return bit_cast<std::uint64_t>(x);
	}

	constexpr std::int64_t double_to_int64(double x) noexcept
	{
		return bit_cast<std::int64_t>(x);
	}

	constexpr double uint64_to_double(std::uint64_t x) noexcept
	{
		return bit_cast<double>(x);
	}

	constexpr double int64_to_double(std::int64_t x) noexcept
	{
		return bit_cast<double>(x);
	}

	constexpr std::uint32_t float_to_uint32(float x) noexcept
	{
		return bit_cast<std::uint32_t>(x);
	}

	constexpr std::int32_t float_to_int32(float x) noexcept
	{
		return bit_cast<std::int32_t>(x);
	}

	constexpr float uint32_to_float(std::uint32_t x) noexcept
	{
		return bit_cast<float>(x);
	}

	constexpr float int32_to_float(std::int32_t x) noexcept
	{
		return bit_cast<float>(x);
	}

	/**
	 * @brief Rotates unsigned integer bits to the right.
	 * https://en.cppreference.com/w/cpp/numeric/rotr
	 */
	template <class T, std::enable_if_t<traits::ccm_is_unsigned_v<T>, bool>  = true>
	constexpr T rotr(T t, int cnt) noexcept
	{
		#if defined(_MSC_VER) && !defined(__clang__)
		// Allow for the use of compiler intrinsics if we are not being evaluated at compile time in msvc.
		if (!is_constant_evaluated())
		{
			// These func are not constexpr in msvc.
			if constexpr (std::is_same_v<T, unsigned int>) { return _rotr(t, cnt); }
			else if constexpr (std::is_same_v<T, std::uint64_t>) { return _rotr64(t, cnt); }
		}
		#endif
		const unsigned int dig = std::numeric_limits<T>::digits;

		if ((static_cast<unsigned int>(cnt) % dig) == 0) { return t; }

		if (cnt < 0)
		{
			cnt *= -1;
			return (t << (static_cast<unsigned int>(cnt) % dig)) |
			       (t >> (dig - (static_cast<unsigned int>(cnt) % dig))); // rotr with negative cnt is similar to rotl
		}

		return (t >> (static_cast<unsigned int>(cnt) % dig)) | (t << (dig - (static_cast<unsigned int>(cnt) % dig)));
	}

	/**
	 * @brief Rotates unsigned integer bits to the left.
	 * https://en.cppreference.com/w/cpp/numeric/rotl
	 */
	template <class T, std::enable_if_t<traits::ccm_is_unsigned_v<T>, bool>  = true>
	constexpr T rotl(T t, int cnt) noexcept
	{
		#if defined(_MSC_VER) && !defined(__clang__)
		// Allow for the use of compiler intrinsics if we are not being evaluated at compile time in msvc.
		if (!is_constant_evaluated())
		{
			// These func are not constexpr in msvc.
			if constexpr (std::is_same_v<T, unsigned int>) { return _rotl(t, cnt); }
			else if constexpr (std::is_same_v<T, std::uint64_t>) { return _rotl64(t, cnt); }
		}
		#endif
		return rotr(t, -cnt);
	}

	// Macro to allow simplified creation of specializations
	// NOLINTBEGIN(bugprone-macro-parentheses)
	#define INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                               \
	template <>                                                                                                                                                \
	[[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                                         \
	{                                                                                                                                                          \
		static_assert(ccm::support::traits::ccm_is_unsigned_v<TYPE>);                                                                                          \
		return value == 0 ? std::numeric_limits<TYPE>::digits : BUILTIN(value);                                                                                \
	}
	// NOLINTEND(bugprone-macro-parentheses)

	#if CCM_HAS_BUILTIN(__builtin_ctzg)
	/**
	 * @brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit ("right").
	 * https://en.cppreference.com/w/cpp/numeric/countr_zero
	 */
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<ccm::support::traits::ccm_is_unsigned_v<T>, int> countr_zero(T value)
	{
		return __builtin_ctzg(value, std::numeric_limits<T>::digits); // NOLINT
	}
	#else  // !CCM_HAS_BUILTIN(__builtin_ctzg)
	/**
	 * @brief Returns the number of consecutive 0 bits in the value of x, starting from the least significant bit ("right").
	 * https://en.cppreference.com/w/cpp/numeric/countr_zero
	 */
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countr_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }
		if (value & 0x1) { return 0; }
		// Bisection method
		unsigned zero_bits = 0;
		unsigned shift     = std::numeric_limits<T>::digits >> 1;
		T mask             = std::numeric_limits<T>::max() >> shift;
		while (shift)
		{
			if ((value & mask) == 0)
			{
				value >>= shift;
				zero_bits |= shift;
			}
			shift >>= 1;
			mask >>= shift;
		}
		return zero_bits;
	}
	#endif // CCM_HAS_BUILTIN(__builtin_ctzg)

	#if CCM_HAS_BUILTIN(__builtin_ctzs)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned short, __builtin_ctzs)
	#endif // CCM_HAS_BUILTIN(__builtin_ctzs)
	#if CCM_HAS_BUILTIN(__builtin_ctz)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned int, __builtin_ctz)
	#endif // CCM_HAS_BUILTIN(__builtin_ctz)
	#if CCM_HAS_BUILTIN(__builtin_ctzl)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned long, __builtin_ctzl)
	#endif // CCM_HAS_BUILTIN(__builtin_ctzl)
	#if CCM_HAS_BUILTIN(__builtin_ctzll)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countr_zero, unsigned long long, __builtin_ctzll)
	#endif // CCM_HAS_BUILTIN(__builtin_ctzll)

	#if CCM_HAS_BUILTIN(__builtin_clzg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countl_zero(T value)
	{
		return __builtin_clzg(value, std::numeric_limits<T>::digits); // NOLINT
	}
	#else  // !CCM_HAS_BUILTIN(__builtin_clzg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countl_zero(T value)
	{
		if (value == 0) { return std::numeric_limits<T>::digits; }
		// Bisection method
		unsigned zero_bits = 0;
		if (is_constant_evaluated())
		{
			for (unsigned shift = std::numeric_limits<T>::digits >> 1; shift; shift >>= 1)
			{
				T tmp = value >> shift;
				if (tmp) { value = tmp; }
				else { zero_bits |= shift; }
			}
		}
		else
		{
			CCM_SIMD_VECTORIZE for (unsigned shift = std::numeric_limits<T>::digits >> 1; shift; shift >>= 1)
			{
				T tmp = value >> shift;
				if (tmp) { value = tmp; }
				else { zero_bits |= shift; }
			}
		}
		return zero_bits;
	}
	#endif // CCM_HAS_BUILTIN(__builtin_clzg)

	#if CCM_HAS_BUILTIN(__builtin_clzs)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned short, __builtin_clzs)
	#endif // CCM_HAS_BUILTIN(__builtin_clzs)
	#if CCM_HAS_BUILTIN(__builtin_clz)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned int, __builtin_clz)
	#endif // CCM_HAS_BUILTIN(__builtin_clz)
	#if CCM_HAS_BUILTIN(__builtin_clzl)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned long, __builtin_clzl)
	#endif // CCM_HAS_BUILTIN(__builtin_clzl)
	#if CCM_HAS_BUILTIN(__builtin_clzll)
	INTERNAL_CCM_ADD_CTZ_SPECIALIZATION(countl_zero, unsigned long long, __builtin_clzll)
	#endif // CCM_HAS_BUILTIN(__builtin_clzll)

	#undef INTERNAL_CCM_ADD_CTZ_SPECIALIZATION

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countr_one(T value)
	{
		return support::countr_zero<T>(~value);
	}

	template <typename T, std::enable_if_t<traits::is_unsigned_integer_v<T>, bool>  = true>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> countl_one(T value)
	{
		return value != std::numeric_limits<T>::max() ? countl_zero(static_cast<T>(~value)) : std::numeric_limits<T>::digits;
	}

	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> bit_width(T value)
	{
		return std::numeric_limits<T>::digits - countl_zero(value);
	}

	#if CCM_HAS_BUILTIN(__builtin_popcountg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<std::is_unsigned_v<T>, int> popcount(T value)
	{
		return __builtin_popcountg(value); // NOLINT
	}
	#else // !CCM_HAS_BUILTIN(__builtin_popcountg)
	template <typename T>
	[[nodiscard]] constexpr std::enable_if_t<traits::ccm_is_unsigned_v<T>, int> popcount(T value)
	{
		int count = 0;
		if (is_constant_evaluated())
		{
			for (int i = 0; i != std::numeric_limits<T>::digits; ++i)
			{
				if ((value >> i) & 0x1) { ++count; }
			}
		}
		else
		{
			CCM_SIMD_VECTORIZE for (int i = 0; i != std::numeric_limits<T>::digits; ++i)
			{
				if ((value >> i) & 0x1) { ++count; }
			}
		}
		return count;
	}

	// We don't want these overloads to be defined if the builtins are available, so we check for the builtins first.
	// Provides optimizations for common types.
	// All provided optimizations are based on the Hammering Weight algorithm except for unsigned char.
	// https://en.wikipedia.org/wiki/Hamming_weight
	#if !CCM_HAS_BUILTIN(__builtin_popcount)

	#endif // !CCM_HAS_BUILTIN(__builtin_popcount)

	#if !CCM_HAS_BUILTIN(__builtin_popcountl)

	#endif // !CCM_HAS_BUILTIN(__builtin_popcount)

	#if !CCM_HAS_BUILTIN(__builtin_popcountll)

	#endif // !CCM_HAS_BUILTIN(__builtin_popcountll)

	#endif // CCM_HAS_BUILTIN(__builtin_popcountg)

	// Macro to allow simplified creation of specializations
	// NOLINTBEGIN(bugprone-macro-parentheses)
	#define INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(FUNC, TYPE, BUILTIN)                                                                                          \
	template <>                                                                                                                                                \
	[[nodiscard]] constexpr int FUNC<TYPE>(TYPE value)                                                                                                         \
	{                                                                                                                                                          \
		static_assert(ccm::support::traits::ccm_is_unsigned_v<TYPE>);                                                                                          \
		return BUILTIN(value);                                                                                                                                 \
	}

	// NOLINTEND(bugprone-macro-parentheses)
	// If the compiler has builtins for popcount, then create specializations that use the builtins.
	#if CCM_HAS_BUILTIN(__builtin_popcount)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned char, __builtin_popcount)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned short, __builtin_popcount)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned, __builtin_popcount)
	#else
	// If we don't have builtins, then provide optimizations for common types.
	// All provided optimizations are based on the Hamming Weight algorithm except for unsigned char.
	// https://en.wikipedia.org/wiki/Hamming_weight
	constexpr int popcount(unsigned char n)
	{
		// The algorithm is specific to 8-bit input, and avoids using 64-bit register for code size.
		std::uint32_t r = static_cast<std::uint32_t>(n * 0x08040201U);
		r				= static_cast<std::uint32_t>(((r >> 3) & 0x11111111U) * 0x11111111U) >> 28;
		return r;
	}

	constexpr int popcount(unsigned short n)
	{
		n = n - (n >> 1) & 0x5555;
		n = (n & 0x3333) + (n >> 2) & 0x3333;
		n = (n + (n >> 4)) & 0x0F0F;
		return (n * 0x0101) >> 16;
	}

	constexpr int popcount(unsigned int n)
	{
		// int can be 32 or 16 bits, so we need to check.
		if constexpr (constexpr int bits = std::numeric_limits<unsigned int>::digits; bits == 32) // 32 bit int
		{
			n = n - (n >> 1) & 0x55555555;
			n = (n & 0x33333333) + (n >> 2) & 0x33333333;
			n = (n + (n >> 4)) & 0x0F0F0F0F;
			return (n * 0x01010101) >> 24;
		}
		else // 16 bit int
		{
			n = n - (n >> 1) & 0x5555;
			n = (n & 0x3333) + (n >> 2) & 0x3333;
			n = (n + (n >> 4)) & 0x0F0F;
			return (n * 0x0101) >> 16;
		}
	}
	#endif

	#if CCM_HAS_BUILTIN(__builtin_popcountl)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned long, __builtin_popcountl)
	#else
	constexpr int popcount(unsigned long n)
	{
		// long can be 32 or 64 bits, so we need to check.
		if constexpr (constexpr long bits = std::numeric_limits<unsigned long>::digits; bits == 32) // 32 bit long
		{
			n = n - (n >> 1) & 0x55555555;
			n = (n & 0x33333333) + (n >> 2) & 0x33333333;
			n = (n + (n >> 4)) & 0x0F0F0F0F;
			return (n * 0x01010101) >> 24;
		}
		else // 64-bit long
		{
			n = n - ((n >> 1) & 0x5555555555555555);
			n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333);
			n = (n + (n >> 4)) & 0xF0F0F0F0F0F0F0F;
			return (n * 0x101010101010101) >> 56;
		}
	}
	#endif

	#if CCM_HAS_BUILTIN(__builtin_popcountll)
	INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION(popcount, unsigned long long, __builtin_popcountll)
	#else
	constexpr int popcount(unsigned long long n)
	{
		n = n - ((n >> 1) & 0x5555555555555555);
		n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333);
		n = (n + (n >> 4)) & 0xF0F0F0F0F0F0F0F;
		return (n * 0x101010101010101) >> 56;
	}
	#endif

	#undef INTERNAL_CCM_ADD_POPCOUNT_SPECIALIZATION

} // namespace ccm::support
