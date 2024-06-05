/*
 * Copyright (c) 2024-Present Ian Pike
 * Copyright (c) 2024-Present ccmath contributors
 *
 * This library is provided under the MIT License.
 * See LICENSE for more information.
 */

#pragma once

#include "ccmath/internal/config/type_support.hpp"
#include "ccmath/internal/runtime/simd/intrin_include.hpp"
#include "ccmath/internal/support/always_false.hpp"

#include <type_traits>

namespace ccm::rt::simd
{


	[[nodiscard]] inline simd_float_t mm_set_ss(float num) noexcept
	{
#ifdef CCMATH_HAS_SIMD
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_set1_ps(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_set_ss(num);
	#elif defined(CCMATH_HAS_SIMD_NEON)
		return vsetq_lane_f32(num, vdupq_n_f32(0.0f), 0)
	#else
		assert(false); // This should never be reached.
		return num;
	#endif
#else
		assert(false); // This should never be reached.
		return num;
#endif
	}

	[[nodiscard]] inline simd_double_t mm_set_ss(double num) noexcept
	{
#ifdef CCMATH_HAS_SIMD
	#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_set1_pd(num);
	#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_set_sd(num);
	#elif defined(CCMATH_HAS_SIMD_NEON)
		return vsetq_lane_f64(num, vdupq_n_f64(0.0), 0);
	#else
		assert(false); // This should never be reached.
		return num;
	#endif
#else
		assert(false); // This should never be reached.
		return num;
#endif
	}

#ifdef CCM_TYPES_LONG_DOUBLE_IS_FLOAT64
	[[nodiscard]] inline simd_long_double_t mm_set_ss(long double num) noexcept
	{
	#ifdef CCMATH_HAS_SIMD
		#if defined(CCMATH_HAS_SIMD_AVX)
		return _mm256_set1_pd(static_assert<double>(num));
		#elif defined(CCMATH_HAS_SIMD_SSE2)
		return _mm_set_sd(static_assert<double>(num));
		#elif defined(CCMATH_HAS_SIMD_NEON)
		return vsetq_lane_f64(static_assert<double>(num), vdupq_n_f64(0.0), 0);
		#else
		assert(false); // This should never be reached.
		return num;
		#endif
	#else
		assert(false); // This should never be reached.
		return num;
	#endif
	}
#endif

} // namespace ccm::rt::simd
