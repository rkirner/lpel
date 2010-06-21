
#ifndef _LPEL_PRIVATE_H_
#define _LPEL_PRIVATE_H_

#include "lpel.h"


/*
 * private LPEL management
 */

extern int LpelGetWorkerId(void);
extern task_t *LpelGetCurrentTask(void);


/* private task management */


typedef enum {
  TASK_RUNNING,
  TASK_READY,
  TASK_WAITING,
  TASK_ZOMBIE
} taskstate_t;


struct task {

  /* TASK CONTROL BLOCK */

  /* type:  IO or normal */
  //TODO

  /* state */
  taskstate_t state;

  /* queue handling: prev, next */
  task_t *prev;
  task_t *next;

  /* signalling write/read events*/
  /*TODO padding */
  volatile bool ev_write;
  volatile bool ev_read;

  /* current/last/assigned worker thread */
  // TODO

  /* scheduling information  */
  void *sched_info;

  /* the handle (or NULL for collector?) */
  //TODO


  /* Accounting information */

  /* processing time: */
  //TODO define generic macros and datatypes
  /* time of creation */
  /* last running time */
  /* total running time */
  /* exponential average running time */
  /* average running time */
  /* variance of running time */
  

  /* simple counters: */
  /* dispatch counter */
  unsigned long cnt_dispatch;
  /* read counters */
  unsigned long cnt_read_total;
  unsigned long cnt_read_last;
  unsigned long cnt_read_avg;
  /* write counters */
  unsigned long cnt_write_total;
  unsigned long cnt_write_last;
  unsigned long cnt_write_avg;


  /* CODE */
  /* a coroutine_t variable, upon creation of that task the coroutine needs to be created */
  /* _or_ generic macros for context switching */
};


#endif /* _LPEL_PRIVATE_H_ */