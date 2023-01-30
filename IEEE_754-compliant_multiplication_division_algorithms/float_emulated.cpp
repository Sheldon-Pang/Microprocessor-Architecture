//===================================================================
// float_emulated.cpp
//
// This file implments an emulated single-precision floating point
// class that behaves identically to IEEE 754 single precision.
// Arithetic is performed with two extra bits of precision and a
// sticky bit.  These three bits are used to round the final results
// of any computation.
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
#include "float_emulated.h"

//===================================================================
// float_emulated()
//
// Default constructor - set the object to a value of zero.
float_emulated::float_emulated() {
	// initialize to zero
	exponent = 0;
	fractional_bits = 0;
	sign = 0;
}

//===================================================================
// float_emulated()
//
// Copy constructor - set the object to match the parameter.
float_emulated::float_emulated(const float_emulated& fe) {
	// initialize to zero
	exponent = fe.exponent;
	fractional_bits = fe.fractional_bits;
	sign = fe.sign;
}

//===================================================================
// float_emulated()
//
// Initializing constructor - extract the bits from the provided 
// float, and set the values of the float_emulated from the values.
// The location and meaning of the bits within the float are defined
// in IEEE 754.
float_emulated::float_emulated(float f) {
	// get the bits from the float into a long integer
	unsigned long float_bits = *((unsigned long*)&f);

	// parse the bitfield to extract the fractional bits,
	// exponent and sign.
	fractional_bits = float_bits & 0x007FFFFF;
	exponent = ((float_bits >> 23) & 0xFF);
	sign = (float_bits >> 31) & 1;
}

//===================================================================
// float_emulated(int)
//
// Construct an emulated floating point number from the specified
// integer.  This function performs normalization and rounding
// (if required).
//
// In addtion to initilization of new float_emulated objects, this
// function is also called by operator + in order to normalize
// and round the results of the addition step
//
// rounding is accomplished by the following:
//    when shifting the integer to the right (losing precision),
//    the most significant 2 bits of those that were lost will 
//    be temporarily kept. These are the guard and round bits.
//
//    Furthermore, if additional bits beyond these were also
//    non-zero, the value of the "sticky bit is set".  Sticky
//    serves the purpose of representing any addtional precision
//    that is not directly captured by the guard and round bits.
//
//    if guard and round <2,  The guard, round and sticky bits
//    will be truncated.
//
//    if guard and round == 2, then the sticky bit determines 
//    whether or not to round upward (sticky=1) or round up
//    to nearest even.
//
//    if guard and round == 3, the number will be rounded upward
float_emulated::float_emulated(int i) {
	if (i == 0) {
		// special case of zero - IEEE 754 format for zero has
		// exponent and fractional bits both set to 0
		exponent = 0;
		fractional_bits = 0;
		sign = 0;
		return;
	}
	// capture the sign of the number and make it positive if 
	// it is currently negative
	if (i < 0) {
		sign = 1;
		i = -i;
	}
	else sign = 0;

	// normalize the integer so that the most significant 
	// 1 is in bit position 23
	exponent = 127+23;
	unsigned int gr = 0;     // guard and round bits
	unsigned int sticky = 0; 
	while (i >= (1 << 24)) {
		exponent++;
		sticky |= (gr & 1);
		gr = (gr >> 1) + (2*(i&1));
		i= (i >> 1);
	}
	while (i < (1 << 23)) {
		exponent--;
		i = (i << 1);
	}

	// perform rounding based on g/r and sticky
	if (gr == 2) {
		// this is a tie at the lsb - round upward if sticky is set
		if (sticky) {
			i++;
		}
		else {
			// round up to nearest even number
			i = (i+1)&0xFFFFFFFE;
		}
	}
	else if (gr == 3) {
		i++;
	}

	// renormalize again if required
	while (i >= (1 << 24)) {
		exponent++;
		i = (i >> 1);
	}
	fractional_bits = i & 0x007fffff;
}

//===================================================================
// operator =()
//
// Perform the assignment operation from a float (performing conversion)
//
// parameters:
//    f - the floating point to assign from;
// returns:
//    a copy of the newly assigned object;
// changes:
//    the value of this object will be updated to represent the float
float_emulated float_emulated::operator=(const float f) {
	// use constructor to perform conversion
	float_emulated fe = f;

	// now assign the values of the fields
	this->exponent = fe.exponent;
	this->fractional_bits = fe.fractional_bits;
	this->sign = fe.sign;

	// return the newly assigned value
	return fe;
}

//===================================================================
// operator =()
//
// Perform the assignment operation from an int (performing conversion)
//
// parameters:
//    i - the integer to assign from;
// returns:
//    a copy of the newly assigned object;
// changes:
//    the value of this object will be updated to represent the integer
float_emulated float_emulated::operator=(const int i) {
	// use constructor to perform conversion
	float_emulated fe = i;	

	// assign the values of the fields
	this->exponent = fe.exponent;
	this->fractional_bits = fe.fractional_bits;
	this->sign = fe.sign;

	// return the newly assigned value
	return *this;
}

//===================================================================
// operator =()
//
// Perform the assignment operation from another float_emulated object
//
// parameters:
//    fe - a reference to the float_emulated to assign from;
// returns:
//    a copy of the newly assigned object;
// changes:
//    the value of this object will be updated to represent the integer
float_emulated float_emulated::operator=(const float_emulated& fe) {
	// assign the fields
	this->exponent = fe.exponent;
	this->fractional_bits = fe.fractional_bits;
	this->sign = fe.sign;

	// return a copy of the newly assigned object
	return *this;
}

//===================================================================
// operator float()
//
// Cast from this object to a float
//
// parameters:
//    none
// returns:
//    a float representation of this object
// changes:
//    nothing
float_emulated::operator float() const {
	// Compose a bitfield from the fields in this object.  
	// Consult IEEE 754 for meaning of the bits in this bitfield
	unsigned long bitfield = this->fractional_bits;
	bitfield |= (sign << 31);
	bitfield |= (exponent << 23);

	// cast the bitfield to a float and return the result
	return *((float*)&bitfield);
}

//===================================================================
// operator int()
//
// Cast from this object to an int
//
// parameters:
//    none
// returns:
//    a float representation of this object
// changes:
//    nothing
float_emulated::operator int() const {
	// perform a cast from float, then use the normal c
	// code to cast from float to int.
	return (int)((float)*this);
}

//===================================================================
// operator+()
//
// Perform addition of a float_emulated and this object using IEEE 754 
// compliant addition and return the result.  Denormals, infinities
// and NaN are not supported.
//
// parameters:
//    fe - the emulated floating point to add to this object
// returns:
//    an emulated floating point number that expresses the sum
// changes:
//    nothing
float_emulated float_emulated::operator+(const float_emulated& fe) {
	// check for special case of 0 (ignore denorms)
	if (fe.exponent == 0) return *this;
	if (exponent == 0) return fe;

	// create integer representations of the fractional bits, adding
	// in the implied 1 for both operands.
	long s1 = fractional_bits + (1 << 23);
	long s2 = fe.fractional_bits + (1 << 23);

	// for each of the operands, add three additional temporary bits
	// for guard, round and sticky
	s1 = s1 << 3;
	s2 = s2 << 3;

	// align binary points - by shifting the smaller of the two
	// numbers - keep the additional three bits for left 
	// for round, guard and sticky. 
	long working_exponent = 0;
	long sticky = 0;
	if (exponent < fe.exponent) {
		// denormalize s2
		for (int bit = 0;bit < fe.exponent - exponent;bit++) {
			if (s1 & 1) {
				// if additional precision beyond guard and round
				// has been lost, set the sticky bit
				sticky |= 1;
			}
			s1 = (s1 >> 1);
		}
		// set the sticky bit for s1
		s1 = (s1 & 0xfffffffe) + sticky;

		// set the working expoent for this addition
		working_exponent = fe.exponent;
	}
	else if (exponent >= fe.exponent) {
		// denormalize s2
		for (int bit = 0;bit < exponent - fe.exponent;bit++) {
			if (s2 & 1) {
				// if additional precision beyond guard and round
				// has been lost, set the sticky bit
				sticky |= 1;
			}
			s2 = (s2 >> 1);
		}
		// set the working exponent for this addition
		working_exponent = exponent;

		// set the sticky bit for s1
		s2 = (s2 & 0xfffffffe) + sticky;
	}

	// apply signs
	if (sign) s1 = -s1;
	if (fe.sign) s2 = -s2;

	// add the operands, performing rounding toward nearest
	// even (using sticky bit)
	long sum = s1 + s2;
	float_emulated result = (const int)sum;

	// remove the impact of guard, round and sticky from the exponent
	result.exponent -= 3;   

	// Ajust the exponent to the original range.  Ignore over/underflow
	result.exponent = working_exponent + result.exponent - 127 - 23;

	// return result
	return result;
}

//===================================================================
// operator-()
//
// Perform subtraction of a float_emulated and this object using 
// IEEE 754 compliant subtraction and return the result.  
// Denormals, infinities and NaN are not supported.
//
// parameters:
//    fe - the emulated floating point to subtract from this object
// returns:
//    an emulated floating point number that expresses the subtraction
// changes:
//    nothing
float_emulated float_emulated::operator-(const float_emulated& fe) {
	// negate the input
	float_emulated fe2 = fe;
	fe2.sign = (fe.sign) ? 0 : 1;  

	// return the result of an addition with the negated value
	return (*this) + fe2;
}

