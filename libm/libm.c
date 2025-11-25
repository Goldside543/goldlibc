// SPDX-License-Identifier: LGPL-2.1-only
/*
 * libm/libm.c
 * 
 * Math library.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

#include "libm.h"
#include <stdint.h>

/* Constants */
static const double M_PI   = 3.141592653589793238462643383279502884;
static const double M_PI_2 = 1.570796326794896619231321691639751442;
static const double M_2_PI = 0.636619772367581343075535053490057448; /* 2/pi */
static const double LN2    = 0.693147180559945309417232121458176568;
static const double INV_LN2 = 1.442695040888963407359924681001892137; /* 1/ln2 */

/* Basic helpers */
double fabs(double x) {
    return x < 0.0 ? -x : x;
}

/* floor/ceil: simple, reasonable range, avoids heavy FP intrinsics */
double floor(double x) {
    long long t = (long long)x; /* trunc toward zero */
    if ((double)t > x) --t;
    /* handle edge cases for large x that don't fit in long long are left as-is */
    return (double)t;
}

double ceil(double x) {
    long long t = (long long)x; /* trunc toward zero */
    if ((double)t < x) ++t;
    return (double)t;
}

/* fmod: remainder with sign of dividend (like fmod from C) */
double fmod(double x, double y) {
    if (y == 0.0) return 0.0/0.0; /* NaN */
    double q = (double)((long long)(x / y)); /* crude quotient */
    double r = x - q * y;
    /* adjust until |r| < |y| */
    while (r >= y) r -= y;
    while (r <= -y) r += y;
    return r;
}

/* sqrt: Newton-Raphson with a few iterations */
double sqrt(double x) {
    if (x <= 0.0) return x == 0.0 ? 0.0 : 0.0/0.0; /* handle negative -> NaN */
    /* initial guess: use exponent approximation */
    int exp = (int)(0);
    double mant = x;
    /* scale x to [0.5, 2) by shifting powers of two using builtin */
    int n = 0;
    while (mant >= 2.0) { mant *= 0.5; ++n; }
    while (mant < 0.5) { mant *= 2.0; --n; }
    /* initial guess from mantissa and exponent */
    double g = 1.0;
    /* Newton iterations */
    for (int i = 0; i < 8; ++i) {
        g = 0.5 * (g + x / g);
    }
    return g;
}

/* exp: range reduction + polynomial */
double exp(double x) {
    if (x > 700.0) return 1.0/0.0;  /* overflow -> inf */
    if (x < -700.0) return 0.0;     /* underflow -> 0 */

    /* Reduce: x = n*ln2 + r, r in [-ln2/2, ln2/2] */
    int n = (int)(x * INV_LN2 + (x >= 0 ? 0.5 : -0.5));
    double r = x - n * LN2;

    /* polynomial approximation for exp(r) (r small) */
    /* use a 7-term Taylor-ish (good for |r| <= ln2/2) */
    double r2 = r*r;
    double r3 = r2*r;
    double r4 = r3*r;
    double r5 = r4*r;
    double r6 = r5*r;
    double r7 = r6*r;

    double res = 1.0 + r + r2*0.5 + r3*(1.0/6.0) + r4*(1.0/24.0)
                 + r5*(1.0/120.0) + r6*(1.0/720.0) + r7*(1.0/5040.0);

    /* scale by 2^n using builtin (no external ldexp dependency) */
    return __builtin_ldexp(res, n);
}

double log(double x) {
    if (x <= 0.0) return 0.0/0.0; /* NaN for non-positive */
    int k = 0;
    double m = x;
    while (m >= 2.0) { m *= 0.5; ++k; }
    while (m < 1.0) { m *= 2.0; --k; }

    /* compute log(m) with atanh-like series */
    double z = (m - 1.0) / (m + 1.0);
    double z2 = z*z;
    /* use a few terms of 2*(z + z^3/3 + z^5/5 + z^7/7) */
    double z3 = z2*z;
    double z5 = z3*z2;
    double z7 = z5*z2;
    double logm = 2.0 * (z + z3/3.0 + z5/5.0 + z7/7.0);

    return logm + k * LN2;
}

/* pow: x^y via exp(y*log(x)) with basic edge-case handling */
double pow(double x, double y) {
    if (x == 0.0) {
        if (y > 0.0) return 0.0;
        if (y == 0.0) return 1.0; /* 0^0 -> 1 by convention here */
        return 1.0/0.0; /* inf */
    }
    if (x < 0.0) {
        /* if y integer, ok; otherwise NaN (we don't implement complex) */
        double yi = (long long)y;
        if (yi == y) {
            double r = exp(y * log(-x));
            if (((long long)y) & 1) return -r;
            return r;
        } else {
            return 0.0/0.0; /* NaN */
        }
    }
    return exp(y * log(x));
}

/* Sine/Cosine: range reduction + odd/even polynomial approx */
/* Reduce x to y in [-pi, pi] then to z in [-pi/2, pi/2] using symmetry. */
/* Use 7th-degree minimax-like polynomials (Taylor-ish) */
static double poly_sin(double z) {
    double z2 = z*z;
    /* coefficients for sin approx: z - z^3/6 + z^5/120 - z^7/5040 */
    double s = z + z*z2 * ( -1.0/6.0 + z2*(1.0/120.0 + z2*(-1.0/5040.0)) );
    return s;
}
static double poly_cos(double z) {
    double z2 = z*z;
    /* cos approx: 1 - z^2/2 + z^4/24 - z^6/720 */
    double c = 1.0 + z2 * ( -0.5 + z2 * (1.0/24.0 + z2 * (-1.0/720.0)) );
    return c;
}

static double rem_piby2(double x, int *quadrant) {
    /* reduce to [-pi, pi], then compute quadrant for pi/2 multiples */
    double invpiby2 = 2.0 / M_PI; /* factor */
    double kf = (double)(long long)(x * invpiby2 + (x >= 0 ? 0.5 : -0.5));
    int k = (int)kf;
    double r = x - k * M_PI_2;
    *quadrant = k & 3;
    return r;
}

double sin(double x) {
    /* quick path for small x */
    if (x == 0.0) return 0.0;
    /* reduce */
    int q;
    double r = rem_piby2(x, &q);
    /* r is roughly in [-pi/2, pi/2] */
    switch (q) {
        case 0: return poly_sin(r);
        case 1: return poly_cos(r);
        case 2: return -poly_sin(r);
        default: return -poly_cos(r);
    }
}

double cos(double x) {
    if (x == 0.0) return 1.0;
    int q;
    double r = rem_piby2(x, &q);
    switch (q) {
        case 0: return poly_cos(r);
        case 1: return -poly_sin(r);
        case 2: return -poly_cos(r);
        default: return poly_sin(r);
    }
}

double tan(double x) {
    double c = cos(x);
    if (c == 0.0) return 0.0/0.0; /* NaN/inf-ish */
    return sin(x) / c;
}
