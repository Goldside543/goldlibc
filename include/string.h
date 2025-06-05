/* SPDX-License-Identifier: LGPL-2.1-only */

#ifndef STRING_H
#define STRING_H

void* memset(void* ptr, int value, size_t num);
void* memcpy(void* dest, const void* src, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);

#endif // STRING_H
