# SPDX-License-Identifier: LGPL-2.1-only

.text

.globl _start

_start: # _start is the entry point known to the linker
    xor %ebp, %ebp            # EBP := 0, because we like tidy stack frames
    movl (%esp), %eax          # Get argc from the stack
    lea 4(%esp), %ecx         # Get the address of argv
    lea 4(%ecx,%eax,4), %edx  # Get the address of envp
    xor %ebx, %ebx            # Clear EBX because it's polite, and also ABI-compliant

    pushl %edx                 # Push envp
    pushl %ecx                 # Push argv
    pushl %eax                 # Push argc
    call main                 # Call main(argc, argv, envp)

    movl %eax, %ebx            # Transfer main's return value into EBX
    xor %eax, %eax            # Clear EAX for syscall cleanliness
    movl $6, %eax             # Exit syscall is 6
    int $0x80                 # Call _exit via syscall
