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

static void print_num(int num, int base) {
    char buf[32];
    char *ptr = buf + sizeof(buf) - 1;
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

    asm volatile (
        "mov %[str], %%ebx;"
        "mov $8, %%eax;"
        "int $0x80;"
        :
        : [str] "r"(ptr)
        : "%eax", "%ebx"
    );
}

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p; ++p) {
        if (*p == '%') {
            ++p;
            if (!*p) break;

            switch (*p) {
                case 's': {
                    char *s = va_arg(args, char *);
                    asm volatile (
                        "mov %[str], %%ebx;"
                        "mov $8, %%eax;"
                        "int $0x80;"
                        :
                        : [str] "r"(s)
                        : "%eax", "%ebx"
                    );
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    print_num(n, 10);
                    break;
                }
                case 'x': {
                    int n = va_arg(args, int);
                    print_num(n, 16);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    char buf[2] = {c, '\0'};
                    asm volatile (
                        "mov %[str], %%ebx;"
                        "mov $8, %%eax;"
                        "int $0x80;"
                        :
                        : [str] "r"(buf)
                        : "%eax", "%ebx"
                    );
                    break;
                }
                default:
                    // Just print the unknown specifier literally
                    char buf[2] = {*p, '\0'};
                    asm volatile (
                        "mov %[str], %%ebx;"
                        "mov $8, %%eax;"
                        "int $0x80;"
                        :
                        : [str] "r"(buf)
                        : "%eax", "%ebx"
                    );
                    break;
            }
        } else {
            char buf[2] = {*p, '\0'};
            asm volatile (
                "mov %[str], %%ebx;"
                "mov $8, %%eax;"
                "int $0x80;"
                :
                : [str] "r"(buf)
                : "%eax", "%ebx"
            );
        }
    }

    va_end(args);
}
