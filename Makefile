# SPDX-License-Identifier: LGPL-2.1-only

CC      := gcc
AS      := as
AR      := ar
CFLAGS  := -Iinclude -ffreestanding -fno-stack-protector

LIBC_DIRS := stdlib string unistd

LIBC_SRCS := $(foreach d,$(LIBC_DIRS),$(wildcard $(d)/*.c))
LIBC_OBJS := $(LIBC_SRCS:.c=.o)

LIBC := libc.a
CRT0 := crt0.o

all: $(LIBC) $(CRT0)

$(LIBC): $(LIBC_OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble crt0.s
$(CRT0): crt0.s
	$(AS) -o $@ $<

clean:
	rm -f $(LIBC_OBJS) $(LIBC) $(CRT0)

.PHONY: all clean
