// SPDX-License-Identifier: LGPL-2.1-only
/*
 * string/string.c
 * 
 * String related functions.
 *
 * Copyright (C) 2025 Goldside543
 *
 */

#include <stddef.h>
#include <stdint.h>

short strcspn(const char *s1, const char *s2);

char *strpbrk(const char *str1, const char *str2) {
    const char *sc1;
    const char *sc2;

    for (sc1 = str1; *sc1 != '\0'; ++sc1) {
        for (sc2 = str2; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return NULL;  /* terminating nulls match */
}

size_t strlen(const char *str) {
    size_t len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char strcat(char *dest, const char *src)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while (*dest++ = *src++);
    return ret;
}

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

char *strcpy(char *dest, const char* src)
{
    char *ret = dest;
    while (*dest++ = *src++);
    return ret;
}

short strspn(const char *s1, const char *s2)
{
    short ret=0;
    while(*s1 && my_strchr(s2,*s1++))
        ret++;
    return ret;
}

char *strtok(char* str, const char* delim) {
    static char* last;
    if (str == NULL) {
        str = last;
    }
    if (str == NULL) {
        return NULL;
    }

    // Skip leading delimiters
    while (*str && my_strchr(delim, *str)) {
        str++;
    }

    if (*str == '\0') {
        last = NULL;
        return NULL;
    }

    char* token_start = str;

    // Find the end of the token
    while (*str && !my_strchr(delim, *str)) {
        str++;
    }

    if (*str == '\0') {
        last = NULL;
    } else {
        *str = '\0';
        last = str + 1;
    }

    return token_start;
}

short strcspn(const char *s1, const char *s2)
{
    short ret=0;
    while(*s1)
        if(my_strchr(s2,*s1) != NULL)
            return ret;
        else
            s1++,ret++;
    return ret;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    // If src has less than n characters, append null bytes to the end of dest
    for (; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}
