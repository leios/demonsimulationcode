/*===- VectorCompatibility.h - libSimulation -==================================
*
*                                  DEMON
*
* This file is distributed under the BSD Open Source License. See LICENSE.TXT 
* for details.
*
*===-----------------------------------------------------------------------===*/

#ifndef VECTORCOMPATIBILITY_H
#define VECTORCOMPATIBILITY_H

#include <immintrin.h>

#ifdef __AVX__
#define FLOAT_STRIDE  8
#define DOUBLE_STRIDE 4
#else
#define FLOAT_STRIDE  4
#define DOUBLE_STRIDE 2
#endif

typedef __m128d doubleV;
typedef __m128 floatV;

static inline void plusEqual_pd(double * const a, const doubleV b) {
	_mm_store_pd(a, _mm_load_pd(a) + b);
}

static inline void minusEqual_pd(double * const a, const doubleV b) {
	_mm_store_pd(a, _mm_load_pd(a) + b);
}

static inline void plusEqualr_pd(double * const a, const doubleV b) {
	_mm_storer_pd(a, _mm_loadr_pd(a) + b);
}

static inline void minusEqualr_pd(double * const a, const doubleV b) {
	_mm_storer_pd(a, _mm_loadr_pd(a) + b);
}

static inline const doubleV fmadd_pd(const doubleV a, const doubleV b, const doubleV c) {
	return a*b + c;
}

#if !defined(__GNUC__) && !defined(__clang__)
doubleV operator+(const doubleV &a, const doubleV &b) {
	return _mm_add_pd(a, b);
}

doubleV operator-(const doubleV &a, const doubleV &b) {
	return _mm_sub_pd(a, b);
}

doubleV operator*(const doubleV &a, const doubleV &b) {
	return _mm_mul_pd(a, b);
}

doubleV operator/(const doubleV &a, const doubleV &b) {
	return _mm_div_pd(a, b);
}

doubleV operator&&(const doubleV &a, const doubleV &b) {
	return _mm_and_pd(a, b);
}
#endif

#endif // VECTORCOMPATIBILITY_H
