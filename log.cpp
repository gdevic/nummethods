/*  Copyright (C) 2021  Goran Devic

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include <iostream>
#include <iomanip>
#include <cmath>

// Use 6 to match examples from Jacques' web pages
constexpr auto M = 7; // Log table size, affects precision of the result

/// <summary>
/// Compute ln(x) or loge(x)
/// Definition: https://www.wolframalpha.com/input/?i=log
/// Algorithm: http://home.citycable.ch/pierrefleur/Jacques-Laporte/Logarithm_1.htm
/// Domain: x > 0 (all positive real numbers)
/// Range: All real numbers
/// </summary>
double ln1(const double n)
{
    const double ln10 = log(10.0);
    const double logs[] = {log(2.0), log(1.1), log(1.01), log(1.001), log(1.0001), log(1.00001), log(1.000001), log(1.0000001)};
    const double table[] = {2, 1.1, 1.01, 1.001, 1.0001, 1.00001, 1.000001, 1.0000001};

    if (n <= 0)
    {
        return 0; // Error: Invalid input value
    }

    int digits[M] = {0};
    double a = n;

    // Suited to a BCD mantissa, we can calculate ln(mantissa) since its range is (0,10)
    // Exponent contributes to ln(x) by this equality: ln(mant x 10^exp) = ln(mant) + exp x ln(10)
    double kln10 = 0;
    while (a >= 10.0) // With normalized BCD-floating point format, this loop is really a simple assignment of exponent to kln10 variable
    {
        a = a / 10;
        kln10 += ln10;
    }

    for (int j = 0; j < M; j++)
    {
        do
        {
            double p = a * table[j]; // With BCD, this is a fused add/shift: "a = a + (a >> 1)" due to the nature of table[] values
            if (p >= 10.0)
                break;
            a = p;
            digits[j]++;
        } while (a < 10.0);
    }

    double result = (10.0 - a) / 10.0;
    // From LSB to MSB to maintain the precision
    for (int j = M - 1; j >= 0; j--)
        result = result + digits[j] * logs[j];

    result = ln10 - result;
    result += kln10;

    return result;
}

constexpr auto K = 7; // Log table size, affects precision of the result

/// <summary>
/// Compute exp(x)
/// Definition: https://www.wolframalpha.com/input/?i=exp
/// Algorithm: http://home.citycable.ch/pierrefleur/Jacques-Laporte/expx.htm
/// Domain: All real numbers
/// Range: x > 0 (all positive real numbers)
/// </summary>
double exp1(const double n)
{
    const double ln10 = log(10.0);
    const double logs[] = {ln10,log(2.0),log(1.1),log(1.01),log(1.001),log(1.0001),log(1.00001),log(1.000001),log(1.0000001),log(1.00000001)};
    const double table[] = {0, 2, 1.1, 1.01, 1.001, 1.0001, 1.00001, 1.000001, 1.0000001, 1.00000001};

    // XXX Handle extended input range, since log(9e+99) is arount 230, that is the maximum input value into this function
    //     In that case, the first loop below will count digit[0] to 99
    if (n > 230)
    {
        return 0; // Error: Out of range
    }

    int digits[K + 1] = {0};
    double a = fabs(n); // Compute using positive values only
    const bool is_neg = n < 0;

    for (int j = 0; j < K + 1; j++)
    {
        do
        {
            double s = a - logs[j];
            if (s < 0.0)
                break;
            a = s;
            digits[j]++;
        } while (a >= 0.0);
    }
    double result = a;
    result = result * pow(10, K - 1); // Left align the result to form 0.x

    // From LSB to MSB to maintain the precision
    for (int j = K; j > 0; j--)
    {
        for (int c = 0; c < digits[j]; c++)
        {
            result = result * table[j] + 1;
        }
        result = result / 10;
    }

    result = result + 0.1;
    result = result * 10;
    for (int j = 0; j < digits[0]; j++)
        result = result * 10;

    if (is_neg)
        result = 1.0 / result;

    return result;
}

#define LN(x) ln1(x)
#define EXP(x) exp1(x)

void algo_log()
{
    const double tests_ln[] = {0.00000001,0.001,1.0,1.1,4.4,9.99,10,11,12.345,15.873,25.2332,1.234e34};
    std::cout << "\n----- LN(x) -----\n";
    for (int i = 0; i < sizeof(tests_ln) / sizeof(double); i++)
    {
        const double x = tests_ln[i];
        const double verif = log(x);
        const double result = LN(x);
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }

    const double tests_exp[] = {0,-1,0.00000001,0.001,1.0,1.1,4.4,9.99,10,11,12.345,15.873,25.2332,87.2332,1.234e-13,9.999e-15,230};
    std::cout << "\n----- EXP(x) -----\n";
    for (int i = 0; i < sizeof(tests_exp) / sizeof(double); i++)
    {
        const double x = tests_exp[i];
        const double verif = exp(x);
        const double result = EXP(x);
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }

    std::cout << "\n----- LN(x)/EXP(x) SYMMETRY -----\n";
    for (int i = 0; i < sizeof(tests_ln) / sizeof(double); i++)
    {
        const double x = tests_ln[i];
        const double verif = exp(log(x));
        const double result = EXP(LN(x));
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }
}
