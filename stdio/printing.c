// SPDX-License-Identifier: LGPL-2.1-only
/*
 * stdio/printing.c
 * 
 * The part of stdio where you print to the screen.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

#include <stdarg.h>
#include <stddef.h>

// --------------------------------------------------
// Helper: convert number to string
// --------------------------------------------------
static void num_to_str(int num, int base, char *buf) {
    char tmp[32];
    char *ptr = tmp + sizeof(tmp) - 1;
    int is_negative = 0;

    *ptr = '\0';

    if (num == 0) {
        *(--ptr) = '0';
    } else {
        if (base == 10 && num < 0) {
            is_negative = 1;
            num = -num;
        }
        while (num) {
            int digit = num % base;
            *(--ptr) = digit < 10 ? '0' + digit : 'a' + (digit - 10);
            num /= base;
        }
        if (is_negative) {
            *(--ptr) = '-';
        }
    }

    // Copy result to output buffer
    char *out = buf;
    while (*ptr) {
        *out++ = *ptr++;
    }
    *out = '\0';
}

// --------------------------------------------------
// vsprintf: write formatted output to a buffer
// --------------------------------------------------
int vsprintf(char *buf, const char *fmt, va_list args) {
    char *out = buf;

    for (const char *p = fmt; *p; ++p) {
        if (*p == '%') {
            ++p;
            if (!*p) break;

            switch (*p) {
                case 's': {
                    char *s = va_arg(args, char *);
                    while (*s) *out++ = *s++;
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    char numbuf[32];
                    num_to_str(n, 10, numbuf);
                    char *s = numbuf;
                    while (*s) *out++ = *s++;
                    break;
                }
                case 'x': {
                    int n = va_arg(args, int);
                    char numbuf[32];
                    num_to_str(n, 16, numbuf);
                    char *s = numbuf;
                    while (*s) *out++ = *s++;
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    *out++ = c;
                    break;
                }
                default:
                    *out++ = *p;
                    break;
            }
        } else {
            *out++ = *p;
        }
    }

    *out = '\0';
    return out - buf; // number of chars written
}

// --------------------------------------------------
// vprintf: write formatted output to the screen via syscall
// --------------------------------------------------
int vprintf(const char *fmt, va_list args) {
    char buf[512]; // temporary buffer
    vsprintf(buf, fmt, args);

    // Output using syscall #8 (like puts)
    asm volatile (
        "mov %[str], %%ebx;"
        "mov $8, %%eax;"
        "int $0x80;"
        :
        : [str] "r"(buf)
        : "%eax", "%ebx"
    );

    return 0; // in the future, could return number of chars written
}

// --------------------------------------------------
// printf family wrappers
// --------------------------------------------------
int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}

int sprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsprintf(buf, fmt, args);
    va_end(args);
    return ret;
}

int snprintf(char *buf, size_t size, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = vsprintf(buf, fmt, args);
    if ((size_t)written >= size) {
        buf[size - 1] = '\0';
        written = size - 1;
    }
    va_end(args);
    return written;
}

int fprintf(void *stream, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    // For now, ignore stream, just write to screen
    int ret = vprintf(fmt, args);
    va_end(args);
    return ret;
}
