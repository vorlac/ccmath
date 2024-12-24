/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include <type_traits>

namespace ccm
{
	/**
	 * @brief Check if the given number is NaN.
	 * @tparam T The type of the number to check.
	 * @param num The number to check.
	 * @return True if the number is NaN, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	[[nodiscard]] constexpr bool isnan(T num) noexcept
	{
#if defined(__GNUC__) || defined(__clang__) || defined(CCM_CONFIG_HAS_BUILTIN_ISNAN_CONSTEXPR)
		return __builtin_isnan(num); // GCC and Clang implement this as constexpr
#else							   // If we can't use the builtin, fallback to this comparison and hope for the best.
		using FPBits_t = typename ccm::support::fp::FPBits<T>;
		const FPBits_t num_bits(num);
		return num_bits.is_nan();
#endif
	}

	/**
	 * @brief Check if the given number is NaN.
	 * @tparam Integer The type of the number to check.
	 * @return False, as integers can never be NaN.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	[[nodiscard]] constexpr bool isnan(Integer /* num */)
	{
		return false; // Integers can never be NaN.
	}
} // namespace ccm

/// @ingroup compare
