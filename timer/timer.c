#include "timer/timer.h"

#include "printf/printf.h"

#define CORE0_TIMER_IRQCNTL ((volatile unsigned*)0x40000040)
#define CORE1_TIMER_IRQCNTL ((volatile unsigned*)0x40000044)
#define CORE2_TIMER_IRQCNTL ((volatile unsigned*)0x40000048)
#define CORE3_TIMER_IRQCNTL ((volatile unsigned*)0x4000004C)

void timer_init() {
  // enable timer
  asm("msr cntp_ctl_el0, %0" ::"r"(0x1));
  // setup remaining time
  timer_reset();
  // enable timer interrupt
  *CORE0_TIMER_IRQCNTL = 0x2;
}

void timer_reset() {
  unsigned frequency = 0;
  asm("mrs %0, cntfrq_el0" : "=r"(frequency));
  asm("msr cntp_tval_el0, %0" ::"r"(frequency * 2));
}
