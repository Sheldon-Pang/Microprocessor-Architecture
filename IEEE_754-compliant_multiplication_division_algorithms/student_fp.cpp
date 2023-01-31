//===================================================================
// student_fp.cpp
//
// This file implements provides implementation for multiplication
// and division operations for an emulated single-precision floating 
// point class that behaves identically to IEEE 754 single precision.
//
// More information on IEEE 754-2019 "IEEE Standard for Floating-Point
// Arithmetic"
//
#include "float_emulated.h"
#include <cstdio>
#include <chrono>
#include <iostream>
#include <bitset>

//===================================================================
// operator*()
//
// Perform multiplication of a float_emulated and this object using IEEE 754 compliant subtraction and return the result.
// Denormals, infinities and NaN are not supported.
//
// parameters:
//    fe - the emulated floating point to multiply with this object
// returns:
//    an emulated floating point number that expresses the product
// changes:
//    nothing
float_emulated float_emulated::operator*(const float_emulated& fe) {
	// reconstruct the multiplication operands from this and fe by adding the implied 1 back into the operands.
	// NOTE: you may want to use a larger precision value these values so that you don't lose bits during the shifting operations.
    long long s1 = this -> fractional_bits + (1 << 23);
    long long s2 = (long long)fe.fractional_bits + (1 << 23);

	// multiply the significands using the bitwise shift method mentioned in the text.
	// Hint: Make sure that the data type for your product variable is large enough to hold a 46-bit product.
    long long product = 0;
    for (int bit = 0; bit < 24; bit++) {
        if (s1 & 1) {
            product += s2;
        }
        s2 = (s2 << 1);
        s1 = (s1 >> 1);
    }

	// The sticky bit should be true if any of the product bits b0 through b19 are non-zero.
    int sticky = 0;
    if (product & 0xFFFFF) {
        sticky = 1;
    }

	// shift the product right by 20 bits and set bit 0 to the value of sticky. The value of the product bitfield should now hold
	//  bit 0     = the value of sticky
	//  bit 1,2   = extra precision bits (guard and round)
	//  bit 3-25  = fractional bits from the product
	//  bit 26-27 = non-fractional bits from the product
    product = (product >> 20) | sticky;

	// place the value of the product in the fractional bits field of the result, performing rounding and normalization as required.
	// HINT: using the copy constructor or assigning from an int will do this for you using the instructor-provided code
    float_emulated result = (int)product;

	// remove the result of guard and round and sticky from the exponent by subtracting three from the result's exponent
    result.exponent -= 3;

	// subtract 23 from the exponent related to the binary point of the product
    result.exponent -= 23;

	// Add the sum of the original exponents to the result (making sure that extra bias is not included).
    result.exponent += (exponent - 127) + (fe.exponent - 127);

	// set the sign of the result
    if (sign != fe.sign) {
        result.sign = 1;
    }

	// return the result
    return result;
}

//===================================================================
// operator/()
//
// Perform division of a float_emulated and this object using 
// IEEE 754 compliant division and return the result.  
// Denormals, infinities and NaN are not supported.
//
// parameters:
//    fe - the emulated floating point to divide into this object
// returns:
//    an emulated floating point number that expresses the division
// changes:
//    nothing
float_emulated float_emulated::operator/(const float_emulated& fe) {
	// reconstruct the division operands from this and fe by adding the implied 1 back into the operands.
    long numerator = this -> fractional_bits + (1 << 23);
    long denominator = fe.fractional_bits + (1 << 23);
	// divide the significands using the bitwise shift method mentioned in the text.
    // Perform the division to 25 bits of precision (for guard and round bits)
    long quotient = 0;
    for (int bit = 0; bit < 26; bit++) {
        quotient = (quotient << 1);
        if (numerator > denominator) {
            numerator = numerator - denominator;
            quotient = quotient + 1;
        }
        numerator = numerator << 1;
    }
	// shift the quotient to the left by one bit to make a place for the sticky bit.
    quotient <<= 1;

	// if there is a remainder to the division, set the sticky bit in the quotient (bit 0)
    if (numerator) {
        quotient |= 1;
    }

	// place the value of the quotient in the fractional bits field of the result, performing rounding and normalization
	// as required.
	// HINT: using the copy constructor or assigning from an int will do this for you using the instructor-provided code
    float_emulated result = (int)quotient;

	// remove the result of guard and round and sticky from the exponent by subtracting three from the result's exponent
    result.exponent -= 3;

	// subtract 23 from the exponent related to the binary point of the quotient
    result.exponent -= 23;

	// Add the difference of the original exponents to the result (making sure that extra bias is not included).
    result.exponent += (exponent - 127) - (fe.exponent - 127);

	// set the sign of the result
    result.sign = this->sign ^ fe.sign;

	// return the result
    return result;
}

float approximate_integral_float(float start_x, float end_x, long steps) {
    float x = start_x;
    float integral = 0;
    float delta = (end_x - start_x) / (float)((const int)steps);
    while ((float)x < (float)end_x) {
        integral = integral + x * delta;
        x = x + delta;
    }
    return integral;
}

float_emulated approximate_integral_float_emulated(float_emulated start_x, float_emulated end_x, long steps) {
    float_emulated x = start_x;
    float_emulated integral = 0;
    float_emulated delta = (end_x - start_x) / (float_emulated)((const int)steps);
    while ((float)x < (float)end_x) {
        integral = integral + x * delta;
        x = x + delta;
    }
    return integral;
}

int main() {
    printf("SER450 - Project 3 - Bichen Pang\n");

    // time for approximate_integral_float
    auto start = std::chrono::high_resolution_clock::now();
    // code to be timed
    float built_in = approximate_integral_float(0,10,10000000);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Total execution time float: " << duration.count() << " microseconds" << std::endl;

    // time for approximate_integral_float_emulated
    auto start2 = std::chrono::high_resolution_clock::now();
    // code to be timed
    float float_emulated = approximate_integral_float_emulated(0,10,10000000);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    std::cout << "Total execution time float_emulated: " << duration2.count() << " microseconds" << std::endl;

//    class float_emulated x = 10;
//    class float_emulated y = 2;
//    float test =  x / y;
//    printf("%f\n", test);

    // display result
    printf("%f\n", built_in);
    printf("%f\n", float_emulated);
    return 0;
}
