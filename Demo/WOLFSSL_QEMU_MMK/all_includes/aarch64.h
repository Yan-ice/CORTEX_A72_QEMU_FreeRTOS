#ifndef __ASSEMBLER__

#ifndef AARCH64_H_
#define AARCH64_H_

#include <FreeRTOS.h>

// which core is this?
static inline uint64_t
cpuid()
{
  uint64_t x;
  asm volatile("mrs %0, mpidr_el1" : "=r" (x) );
  return x & 0xff;
}

// Vector Base Address Register in EL1
static inline void
w_vbar_el1(uint64_t x)
{
  asm volatile("msr vbar_el1, %0" : : "r" (x) );
}

static inline uint64_t
r_vbar_el1()
{
  uint64_t x;
  asm volatile("mrs %0, vbar_el1" : "=r" (x) );
  return x;
}

// TODO: fix comment
// supervisor address translation and protection;
// holds the address of the page table.
static inline void
w_ttbr0_el1(uint64_t x)
{
  asm volatile("msr ttbr0_el1, %0" : : "r" (x) );
}

static inline uint64_t
r_ttbr0_el1()
{
  uint64_t x;
  asm volatile("mrs %0, ttbr0_el1" : "=r" (x) );
  return x;
}

static inline void
w_ttbr1_el1(uint64_t x)
{
  asm volatile("msr ttbr1_el1, %0" : : "r" (x));
}

static inline uint64_t
r_ttbr1_el1()
{
  uint64_t x;
  asm volatile("mrs %0, ttbr1_el1" : "=r" (x) );
  return x;
}

static inline uint64_t
r_esr_el1()
{
  uint64_t x;
  asm volatile("mrs %0, esr_el1" : "=r" (x) );
  return x;
}

static inline void
w_esr_el1(uint64_t x)
{
  asm volatile("msr esr_el1, %0" : : "r" (x) );
}

static inline uint64_t
r_elr_el1()
{
  uint64_t x;
  asm volatile("mrs %0, elr_el1" : "=r" (x) );
  return x;
}

static inline uint64_t
r_far_el1()
{
  uint64_t x;
  asm volatile("mrs %0, far_el1" : "=r" (x) );
  return x;
}

// armv8 generic timer
static inline uint64_t
r_cntv_ctl_el0()
{
  uint64_t x;
  asm volatile("mrs %0, cntv_ctl_el0" : "=r" (x) );
  return x;
}

//controller of timer
static inline void
w_cntv_ctl_el0(uint64_t x)
{
  asm volatile("msr cntv_ctl_el0, %0" : : "r" (x) );
}


static inline uint64_t
r_cntv_tval_el0()
{
  uint64_t x;
  asm volatile("mrs %0, cntv_tval_el0" : "=r" (x) );
  return x;
}

//set when to trigger timer intr
static inline void
w_cntv_tval_el0(uint64_t x)
{
  asm volatile("msr cntv_tval_el0, %0" : : "r" (x) );
}

static inline uint64_t
cpu_time()
{
  uint64_t x;
  asm volatile("mrs %0, cntvct_el0" : "=r" (x) );
  return x;
}

//get the conunter
static inline uint64_t
r_cntvct_el0()
{
  uint64_t x;
  asm volatile("mrs %0, cntvct_el0" : "=r" (x) );
  return x;
}

static inline uint64_t
r_cntfrq_el0()
{
  uint64_t x;
  asm volatile("mrs %0, cntfrq_el0" : "=r" (x) );
  return x;
}

// enable device interrupts(irq)
static inline void
intr_on()
{
  asm volatile("msr daifclr, #0xf" ::: "memory");
}

// disable device interrupts(irq)
static inline void
intr_off()
{
  asm volatile("msr daifset, #0xf" ::: "memory");
}

static inline uint64_t
daif()
{
  uint64_t x;
  asm volatile("mrs %0, daif" : "=r" (x) );
  return x;
}

// are device interrupts(irq) enabled?
static inline int
intr_get()
{
  uint64_t x = daif();
  return ((x >> 6) & 0x2) == 0;
}

static inline uint64_t
r_sp()
{
  uint64_t x;
  asm volatile("mov %0, sp" : "=r" (x) );
  return x;
}

static inline uint64_t
r_ra()
{
  uint64_t x;
  asm volatile("mov %0, x30" : "=r" (x) );
  return x;
}

static inline void
isb()
{
  asm volatile("isb");
}

static inline void
dsb()
{
  asm volatile("dsb sy");
}

// flush the TLB.
static inline void
flush_tlb()
{
  asm volatile("dsb ishst");
  asm volatile("tlbi vmalle1is");
  asm volatile("dsb ish");
  isb();
}


#endif

#endif  /* __ASSEMBLER__ */
