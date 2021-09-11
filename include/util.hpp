#pragma once
#include <iostream>

/**
 * Convenience struct for representing numbers in scientific notation. 
 */
struct ScientificNotation
{
    float m;
    int exponent;

    ScientificNotation() : m(1), exponent(0) {}
    ScientificNotation(float _m, int _exponent) : m(_m), exponent(_exponent) {}

    ScientificNotation operator*(int x);
    void operator*=(int x);

    friend std::ostream &operator<<(std::ostream &os, const ScientificNotation &sn);
};
