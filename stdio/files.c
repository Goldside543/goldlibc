// SPDX-License-Identifier: LGPL-2.1-only
/*
 * stdio/files.c
 * 
 * File related functions.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

#include <stddef.h>

struct stat {
    size_t st_size;  // File size in bytes
    unsigned int st_mode;  // File permissions (you can use `S_IFREG`, `S_IFDIR` for regular files, directories, etc.)
    unsigned int st_uid;   // User ID of file owner
    unsigned int st_gid;   // Group ID of file owner
};

int open(const char *path, int flags) {
    int fd;
    __asm__ volatile (
        "movl $0, %%eax\n\t"    // syscall number for open
        "movl %1, %%ebx\n\t"    // path
        "movl %2, %%ecx\n\t"    // flags
        "int $0x80\n\t"         // syscall
        "movl %%eax, %0\n\t"    // store return in fd
        : "=r"(fd)
        : "r"(path), "r"(flags)
        : "%eax", "%ebx", "%ecx"
    );
    return fd;
}

ssize_t read(int fd, void *buf, size_t size) {
    ssize_t ret;
    __asm__ volatile (
        "movl $2, %%eax\n\t"    // syscall number for read
        "movl %1, %%ebx\n\t"    // fd
        "movl %2, %%ecx\n\t"    // buf
        "movl %3, %%edx\n\t"    // size
        "int $0x80\n\t"
        "movl %%eax, %0\n\t"
        : "=r"(ret)
        : "r"(fd), "r"(buf), "r"(size)
        : "%eax", "%ebx", "%ecx", "%edx"
    );
    return ret;
}

ssize_t write(int fd, const void *buf, size_t size) {
    ssize_t ret;
    __asm__ volatile (
        "movl $1, %%eax\n\t"    // syscall number for write
        "movl %1, %%ebx\n\t"    // fd
        "movl %2, %%ecx\n\t"    // buf
        "movl %3, %%edx\n\t"    // size
        "int $0x80\n\t"
        "movl %%eax, %0\n\t"
        : "=r"(ret)
        : "r"(fd), "r"(buf), "r"(size)
        : "%eax", "%ebx", "%ecx", "%edx"
    );
    return ret;
}

int close(int fd) {
    int ret;
    __asm__ volatile (
        "movl $3, %%eax\n\t"    // syscall number for close
        "movl %1, %%ebx\n\t"    // fd
        "int $0x80\n\t"
        "movl %%eax, %0\n\t"
        : "=r"(ret)
        : "r"(fd)
        : "%eax", "%ebx"
    );
    return ret;
}

int stat(const char *path, struct stat *st) {
    int ret;
    __asm__ volatile (
        "movl $7, %%eax\n\t"    // syscall number for stat
        "movl %1, %%ebx\n\t"    // path
        "movl %2, %%ecx\n\t"    // struct stat pointer
        "int $0x80\n\t"
        "movl %%eax, %0\n\t"
        : "=r"(ret)
        : "r"(path), "r"(st)
        : "%eax", "%ebx", "%ecx"
    );
    return ret;
}
