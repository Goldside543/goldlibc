// SPDX-License-Identifier: GPL-2.0-only
/*
 * stdlib/exit.c
 * 
 * Exiting a program.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

void exit() {
    asm volatile (
            "movl $6, %%eax\n\t"
            "int $0x80"
            :
            :
            : "eax"
        );
}
