.section ".text"
.balign 0x800

.macro store_regs
    stp x0, x1, [sp, -16]!
    stp x2, x3, [sp, -16]!
    stp x4, x5, [sp, -16]!
    stp x6, x7, [sp, -16]!
    stp x8, x9, [sp, -16]!
    stp x10, x11, [sp, -16]!
    stp x12, x13, [sp, -16]!
    stp x14, x15, [sp, -16]!
    stp x16, x17, [sp, -16]!
    stp x18, x19, [sp, -16]!
    stp x20, x21, [sp, -16]!
    stp x22, x23, [sp, -16]!
    stp x24, x25, [sp, -16]!
    stp x26, x27, [sp, -16]!
    stp x28, x29, [sp, -16]!
    stp x30, xzr, [sp, -16]!
    //
    mrs x16, spsr_el1
    mrs x17, elr_el1
    stp x16, x17, [sp, -16]!
.endm

.macro load_regs
    ldp x16, x17, [sp], 16
    msr elr_el1, x17
    msr spsr_el1, x16
    //
    ldp x30, xzr, [sp], 16
    ldp x28, x29, [sp], 16
    ldp x26, x27, [sp], 16
    ldp x24, x25, [sp], 16
    ldp x22, x23, [sp], 16
    ldp x20, x21, [sp], 16
    ldp x18, x19, [sp], 16
    ldp x16, x17, [sp], 16
    ldp x14, x15, [sp], 16
    ldp x12, x13, [sp], 16
    ldp x10, x11, [sp], 16
    ldp x8, x9, [sp], 16
    ldp x6, x7, [sp], 16
    ldp x4, x5, [sp], 16
    ldp x2, x3, [sp], 16
    ldp x0, x1, [sp], 16
.endm

.global exception_vector
exception_vector:
    b default_handler
    .balign 0x80
    b irq_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b irq_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b irq_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b irq_handler
    .balign 0x80
    b default_handler
    .balign 0x80
    b default_handler
    .balign 0x80

irq_handler:
    store_regs
    bl exception_handle_irq
    load_regs
    eret

default_handler:
    store_regs
    bl exception_handle_default
    load_regs
    eret
