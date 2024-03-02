/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/detail/compare/isinf.hpp"
#include "ccmath/detail/compare/isnan.hpp"

namespace ccm
{
	/**
	 * @brief Checks if the given number has a finite value.
	 * @tparam T The type of the number.
	 * @param num A floating-point or integer value
	 * @return true if the number has a finite value, false otherwise.
	 */
	template <typename T, std::enable_if_t<!std::is_integral_v<T>, bool> = true>
	inline constexpr bool isfinite(T x)
	{
		return (!ccm::isnan(x)) && (!ccm::isinf(x));
	}

	/**
	 * @brief Checks if the given number has a finite value.
	 * @tparam Integer The type of the integer.
	 * @param x A integer value to check.
	 * @return true if the number has a finite value, false otherwise.
	 */
	template <typename Integer, std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
	inline constexpr bool isfinite(Integer /* x */)
	{
		return false; // All integers are finite
	}

} // namespace ccm
