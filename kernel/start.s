.section ".text.startup"

.global _start
_start:
    mov x0, (1 << 31)
    msr hcr_el2, x0
    mov x0, 0x3c5
    msr spsr_el2, x0
    ldr x0, =bss_init
    msr elr_el2, x0
    eret
bss_init:
    ldr x0, =__bss_start__
    ldr x1, =__bss_end__
bss_loop:
    cmp x0, x1
    beq stack_init
    str xzr, [x0], 8
    b bss_loop
stack_init:
    ldr x0, =_start
    mov sp, x0
    bl main
kernel_susp:
    wfi
    b kernel_susp
