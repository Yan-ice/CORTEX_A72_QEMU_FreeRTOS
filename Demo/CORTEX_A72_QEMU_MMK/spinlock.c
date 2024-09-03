// Mutual exclusion spin locks.

#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "aarch64.h"
#include <FreeRTOS.h>

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->noff = 0;
  lk->intena = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
void
acquire(struct spinlock *lk)
{
  unsigned char tmp, l = 1;

  push_off(lk); // disable interrupts to avoid deadlock.
  if(holding(lk)){
    while(1);
    //panic("acquire");
  }
    

  asm volatile(
    "sevl\n"
    "1: wfe\n"
    "2: ldaxrb %w0, [%1]\n"
    "cbnz   %w0, 1b\n"
    "stxrb  %w0, %w2, [%1]\n"
    "cbnz   %w0, 2b\n"
    : "=&r"(tmp) : "r"(&lk->locked), "r"(l) : "memory"
  );

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen strictly after the lock is acquired.
  __sync_synchronize();

}

// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk)){
    while(1);
    //panic("release");
  }
    

  // Tell the C compiler and the CPU to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other CPUs before the lock is released,
  // and that loads in the critical section occur strictly before
  // the lock is released.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code doesn't use a C assignment, since the C standard
  // implies that an assignment might be implemented with
  // multiple store instructions.
  __sync_lock_release(&lk->locked);

  pop_off(lk);
}

// Check whether this cpu is holding the lock.
// Interrupts must be off.
int
holding(struct spinlock *lk)
{
  return lk->locked;
}

// push_off/pop_off are like intr_off()/intr_on() except that they are matched:
// it takes two pop_off()s to undo two push_off()s.  Also, if interrupts
// are initially off, then push_off, pop_off leaves them off.

void
push_off(struct spinlock *lk)
{
  int old;
  portINQUIRE_INTERRUPTS(old);

  portDISABLE_INTERRUPTS();
  if(lk->noff == 0)
    lk->intena = old;
  lk->noff += 1;
}

void
pop_off(struct spinlock *lk)
{
  int old;
  portINQUIRE_INTERRUPTS(old);

  if(old){
    while(1);
    //panic("pop_off - interruptible");
  }
    
  if(lk->noff < 1){
    while(1);
    //panic("pop_off");
  }
    
  lk->noff -= 1;
  if(lk->noff == 0 && lk->intena){
    portENABLE_INTERRUPTS();
  }
    
}
