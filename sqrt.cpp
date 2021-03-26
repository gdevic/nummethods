/*  Copyright (C) 2021  Goran Devic

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include <iostream>
#include <iomanip>
#include <cmath>

/// <summary>
/// Compute sqrt(x)
/// Definition: https://www.wolframalpha.com/input/?i=sqrt
/// Algorithm: https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
/// Domain: x >= 0 (all non-negative real numbers)
/// Range: All non-negative real numbers
/// </summary>
double sqrt1(const double n)
{
    if (n < 0)
    {
        return 0; // Error: Invalid input value
    }

    // XXX here we adjust the exponent to be even, possibly shifting the mantissa
    // ...

    if (n == 0)
        return 0; // Handle zero as a special case

    double last;
    double result = n / 10; // Initial guess: a simple BCD shift right
    int loop_cnt = 0; // Convergence loop counter, only used for stats
    do
    {
        last = result;
        double sx = n / last;
        result = (last + sx) / 2;

        loop_cnt++;

        // Implement as tracking of how many digits remained the same between the last and [new] result
        // Once all digits are the same, the requred degree of convergence has been reached
    } while (fabs(last - result) > 1e-15); // Pick a digit on the LSB side

    //std::cout << "Converged in " << loop_cnt << " iterations\n";

    return result;
}

#define SQRT(x) sqrt1(x)

void algo_sqrt()
{
    const double tests_sqrt[] = {0,54757,125348,0.5,0.00035,0.02,1,1.234e78};

    std::cout << "\n----- SQRT(x) -----\n";
    for (int i = 0; i < sizeof(tests_sqrt) / sizeof(double); i++)
    {
        const double x = tests_sqrt[i];
        const double verif = sqrt(x);
        const double result = SQRT(x);
        std::cout << std::setprecision(15) << "x=" << x << " result=" << result << "  verif=" << verif << " error=" << verif - result << "\n";
    }
}
