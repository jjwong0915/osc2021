.section ".text.startup"

.global _start
_start:
    ldr x0, =__bss_start__
    ldr x1, =__bss_end__
bss_loop:
    cmp x0, x1
    beq stack_init
    strb wzr, [x0], 1
    b bss_loop
stack_init:
    ldr x0, =_start
    mov sp, x0
    bl main
kernel_susp:
    wfi
    b kernel_susp
