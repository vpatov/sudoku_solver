#include "puzzle.hpp"
#include "util.hpp"
#include <iomanip>

std::ostream &operator<<(std::ostream &os, const ScientificNotation &sn)
{
    return os << std::setprecision(2) << sn.m << "e" << sn.exponent;
}

ScientificNotation ScientificNotation::operator*(int operand)
{
    ScientificNotation sn(m, exponent);
    sn.m *= operand;
    if (sn.m > 10)
    {
        sn.m /= 10;
        sn.exponent++;
    }
    return sn;
}

void ScientificNotation::operator*=(int operand)
{
    m *= operand;
    if (m > 10)
    {
        m /= 10;
        exponent++;
    }
}