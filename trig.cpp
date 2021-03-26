/*  Copyright (C) 2021  Goran Devic

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include <iostream>
#include <iomanip>
#include <cmath>

constexpr double pi = 3.141592653589793;

constexpr auto K = 7;

static const double tans[] = {atan(1),atan(0.1),atan(0.01),atan(0.001),atan(0.0001),atan(0.00001),atan(0.000001)};
static const double table[] = {1, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001};

/// <summary>
/// Reduce a range of the input value (angle) to (0, 2*PI)
/// This needs to be done for all trigonometric functions
/// </summary>
double range_reduction(double n)
{
    // This is much simpler in BCD-float where mantissa and exponents are already separated
    // Repeatedly subtract 2xPI x 10^exp until the exponent part is 0
    int exp = int(log10(n));

    while (exp > 0)
    {
        const double two_pi = 2 * pi * pow(10, exp);
        if (n >= two_pi)
            n = n - two_pi;
        else
            exp--;
    }

    // The second step is to subtract 2xPI until we are within the range
    while (n > 0)
        n = n - 2 * pi;
    n = n + 2 * pi;

    return n;
}

/// <summary>
/// Compute tan(x)
/// Definition: https://www.wolframalpha.com/input/?i=tan
/// Algorithm: http://home.citycable.ch/pierrefleur/Jacques-Laporte/Trigonometry.htm
/// Domain: All real numbers except where x/pi + 1/2 is zero
/// Range: All real numbers
/// </summary>
double tan1(const double n)
{
    double result = 0;
    int digits[K] = {0};

    double y = fabs(n); // Compute using positive values only
    const bool is_neg = n < 0;

    // Reduction of the input value
    y = range_reduction(y);

    for (int i = 0; i < K; i++)
    {
        while (y >= 0)
        {
            y = y - tans[i];
            digits[i]++;
        }
        y += tans[i];
        digits[i]--;
    }

    double x = 1;
    for (int i = K - 1; i >= 0; i--)
    {
        for (int j = 0; j < digits[i]; j++)
        {
            double xnew = x * table[i];
            double ynew = y * table[i];

            x = x - ynew;
            y = y + xnew;
        }
    }

    if (x == 0)
    {
        return 0; // Error: Invalid input value
    }

    result = y / x;

    if (is_neg)
        result = -result;

    return result;
}

/// <summary>
/// Compute atan(x)
/// Definition: https://www.wolframalpha.com/input/?i=arctan
/// Algorithm: http://home.citycable.ch/pierrefleur/Jacques-Laporte/Inverse_Trigonometric_functions.htm
/// Domain: All real numbers
/// Range: (-pi/2, pi/2)
/// </summary>
double atan1(const double n)
{
    double result = 0;
    int digits[K] = {0};

    double x = 1;
    double y = fabs(n); // Compute using positive values only
    const bool is_neg = n < 0;

    for (int i = 0; i < K; i++)
    {
        while (true)
        {
            double xnew = x * table[i];
            double ynew = y * table[i];
            if ((y - xnew) < 0)
                break;
            x = x + ynew;
            y = y - xnew;
            digits[i]++;
        }
    }

    result = y / x; // Remainder

    // From LSB to MSB to maintain the precision
    for (int j = K - 1; j >= 0; j--)
        result = result + digits[j] * tans[j];

    if (is_neg)
        result = -result;

    return result;
}

#define TAN(x) tan1(x)
#define ATAN(x) atan1(x)

void algo_trig()
{
    const double tests_tan[] = {0,0.984736,0.1,0.5,1.5, pi/2, -1.5, 1.234e5};
    std::cout << "\n----- TAN(x) -----\n";
    for (int i = 0; i < sizeof(tests_tan) / sizeof(double); i++)
    {
        const double x = tests_tan[i];
        const double verif = tan(x);
        const double result = TAN(x);
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }

    const double tests_atan[] = {0, 1, 20, -20, -12345e23, pi, pi/2};
    std::cout << "\n----- ATAN(x) -----\n";
    for (int i = 0; i < sizeof(tests_atan) / sizeof(double); i++)
    {
        const double x = tests_atan[i];
        const double verif = atan(x);
        const double result = ATAN(x);
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }

    std::cout << "\n----- TAN(x)/ATAN(x) SYMMETRY -----\n";
    for (int i = 0; i < sizeof(tests_tan) / sizeof(double); i++)
    {
        const double x = tests_tan[i];
        const double verif = atan(tan(x));
        const double result = ATAN(TAN(x));
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }
}
