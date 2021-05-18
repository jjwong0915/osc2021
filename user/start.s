.section ".text.startup"

.global _start
_start:
    bl main
    svc 0
