#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_


#include "sysdep.h"

#define SWAP xchg

/**
 * Simple spinlocks
 *
 * Test-and-test-and-set style spinlocks
 * using local spinning
 */

#define intxCacheline (64/sizeof(int))

typedef struct {
  volatile int l;
  int padding[intxCacheline-1];
} spinlock_t;

static inline void spinlock_init(spinlock_t *v)
{
  v->l = 0;
}

static inline void spinlock_lock(spinlock_t *v)
{
  while (SWAP( (int *)&v->l, 1) != 0) {
    /* spin locally (only reads) - reduces bus traffic */
    while (v->l);
  }
}

static inline void spinlock_unlock(spinlock_t *v)
{
  WMB();
  v->l = 0;
}

#endif /* _SPINLOCK_H_ */