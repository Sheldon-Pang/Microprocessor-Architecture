//===================================================================
// float_emulated.h
//
// This file defines an emulated single-precision floating point
// class that behaves identically to IEEE 754 single precision.
// Arithetic is performed with two extra bits of precision and a
// sticky bit.  These three bits are used to round the final results
// of any computation.
//
// The multiplication and division functions are missing from this
// file and are expected to be provided by a separate student-
// provided file.
//
// More information on IEEE 754-2019 "IEEE Standard for Floating-Point
// Arithmetic"
//
// Copyright (C) 2020, Arizona State University
// This document may not be copied or distributed except for the 
// purposes described in the assignment instructions within the 
// course shell. Posting the document, derivative works, in full or 
// in part, on web sites other than that used by its associated 
// course is expressly prohibited without the written consent of 
// the author.
#pragma once
class float_emulated
{
	// internal representation
	unsigned long fractional_bits;
	int           exponent;
	unsigned char sign;
public:
	// construction
	float_emulated();
	float_emulated(float f);
	float_emulated(int i);
	float_emulated(const float_emulated&);

	// assignment
	float_emulated operator=(const float f);
	float_emulated operator=(const int i);
	float_emulated operator=(const float_emulated&);

	// numeric operators
	float_emulated operator+(const float_emulated&);
	float_emulated operator-(const float_emulated&);
	float_emulated operator*(const float_emulated&);
	float_emulated operator/(const float_emulated&);

	// casts from other numeric types
	operator float() const;
	operator int() const;
};

