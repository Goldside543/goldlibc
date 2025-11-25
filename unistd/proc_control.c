// SPDX-License-Identifier: LGPL-2.1-only
/*
 * unistd/proc_control.c
 * 
 * Process control.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

void _exit() {
    asm volatile (
            "movl $6, %%eax\n\t"
            "int $0x80\n\t"
            :
            :
            : "eax"
        );
}

void spawn(void *path, void *argv) {
    asm volatile (
        "movl %0, %%ebx\n\t"
        "movl %1, %%ecx\n\t"
        "movl $4, %%eax\n\t"
        "int $0x80\n\t"
        :
        : "r"(path), "r"(argv)
        : "%eax", "%ebx", "%ecx"
    );
}
