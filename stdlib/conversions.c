// SPDX-License-Identifier: LGPL-2.1-only
/*
 * stdlib/conversions.c
 * 
 * Conversion functions.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

int atoi(const char *str) {
    int result = 0;
    int sign = 1;

    // skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;

    // handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

long atol(const char *str) {
    long result = 0;
    int sign = 1;

    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

long strtol(const char *str, char **endptr, int base) {
    long result = 0;
    int sign = 1;

    // skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    if (base == 0) base = 10; // minimal: default to decimal only

    const char *start = str;
    while (*str >= '0' && *str <= '9') {
        int digit = *str - '0';
        if (digit >= base) break;
        result = result * base + digit;
        str++;
    }

    if (endptr) *endptr = (char *)(str == start ? str : str);

    return sign * result;
}

double strtod(const char *str, char **endptr) {
    double result = 0.0;
    double frac = 0.0;
    double div = 10.0;
    int sign = 1;

    // skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n')
        str++;

    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // integer part
    while (*str >= '0' && *str <= '9') {
        result = result * 10.0 + (*str - '0');
        str++;
    }

    // fraction part
    if (*str == '.') {
        str++;
        while (*str >= '0' && *str <= '9') {
            frac += (*str - '0') / div;
            div *= 10.0;
            str++;
        }
    }

    if (endptr) *endptr = (char *)str;

    return sign * (result + frac);
}
