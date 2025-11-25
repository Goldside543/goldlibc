# SPDX-License-Identifier: LGPL-2.1-only

CC := gcc
AR := ar
CFLAGS := -ffreestanding -fno-stack-protector

LIB := libm.a
OBJS := libm.o

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

libm.o: libm.c libm.h
	$(CC) $(CFLAGS) -c libm.c -o libm.o

clean:
	rm -f $(OBJS) $(LIB) mintest

.PHONY: all clean
