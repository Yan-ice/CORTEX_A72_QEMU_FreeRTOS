
// Mutual exclusion lock.
struct spinlock {
  int locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.

  int noff;
  int intena;
};

// spinlock.c
void            acquire(struct spinlock*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            push_off(struct spinlock *lk);
void            pop_off(struct spinlock *lk);