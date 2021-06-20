.section ".text.startup"

.global _start
_start:
    stp x29, x30, [sp, -16]!
    mov x29, sp
    //
    bl main
    //
    ldp x29, x30, [sp], 16
    ret
