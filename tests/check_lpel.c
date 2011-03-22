#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "../lpel.h"




typedef struct {
  lpel_stream_t *in, *out;
  int id;
} channels_t;



void Relay(lpel_task_t *self, void *inarg)
{
  channels_t *ch = (channels_t *)inarg;
  int term = 0;
  int id = ch->id;
  char *item;
  lpel_stream_desc_t *in, *out;

  in = LpelStreamOpen( self, ch->in, 'r');
  out = LpelStreamOpen( self, ch->out, 'w');
  
  printf("Relay %d START\n", id);

  while (!term) {
    item = LpelStreamRead( in);
    assert( item != NULL );
    //printf("Relay %d: %s", id, item );
    if ( 0 == strcmp( item, "T\n")) {
      term = 1;
    }
    LpelStreamWrite( out, item);
  } // end while
  LpelStreamClose( in, 1);
  LpelStreamClose( out, 0);
  free(ch);
  printf("Relay %d TERM\n", id);
}


static channels_t *ChannelsCreate(lpel_stream_t *in, lpel_stream_t *out, int id)
{
  channels_t *ch = (channels_t *) malloc( sizeof(channels_t));
  ch->in = in;
  ch->out = out;
  ch->id = id;
  return ch;
}

lpel_stream_t *PipeElement(lpel_stream_t *in, int depth)
{
  lpel_stream_t *out;
  channels_t *ch;
  lpel_task_t *t;
  int wid = depth % 2;

  out = LpelStreamCreate(0);
  ch = ChannelsCreate( in, out, depth);
  t = LpelTaskCreate( wid, Relay, ch, 4096);
  LpelTaskMonitor( t, NULL, LPEL_MON_TASK_TIMES | LPEL_MON_TASK_STREAMS);
  LpelTaskRun(t);

  printf("Created Relay %d\n", depth );
  return (depth > 0) ? PipeElement( out, depth-1) : out;
}



static void Outputter(lpel_task_t *self, void *arg)
{
  lpel_stream_desc_t *in= LpelStreamOpen( self, (lpel_stream_t*)arg, 'r'); 
  char *item;
  int term = 0;

  printf("Outputter START\n");

  while (!term) {
    item = LpelStreamRead(in);
    assert( item != NULL );
    printf("Out: %s", item );

    if ( 0 == strcmp( item, "T\n")) {
      term = 1;
    }
    free( item);
  } // end while

  LpelStreamClose( in, 1);
  printf("Outputter TERM\n");

  LpelStop();
}


static void Inputter(lpel_task_t *self, void *arg)
{
  lpel_stream_desc_t *out = LpelStreamOpen( self, (lpel_stream_t*)arg, 'w'); 
  char *buf;

  printf("Inputter START\n");
  do {
    buf = (char *) malloc( 120 * sizeof(char) );
    (void) fgets( buf, 119, stdin  );
    LpelStreamWrite( out, buf);
  } while ( 0 != strcmp(buf, "T\n") );

  LpelStreamClose( out, 0);
  printf("Inputter TERM\n");
}

static void testBasic(void)
{
  lpel_stream_t *in, *out;
  lpel_config_t cfg;
  lpel_task_t *intask, *outtask;

  cfg.num_workers = 2;
  cfg.proc_workers = 2;
  cfg.proc_others = 0;
  cfg.flags = 0;
  cfg.node = -1;

  LpelInit(&cfg);

  in = LpelStreamCreate(0);
  out = PipeElement(in, cfg.num_workers*20 - 1);

  outtask = LpelTaskCreate( -1, Outputter, out, 4096);
  LpelTaskMonitor(outtask, "outtask", LPEL_MON_TASK_TIMES);
  LpelTaskRun(outtask);

  intask = LpelTaskCreate( -1, Inputter, in, 4096);
  LpelTaskMonitor(intask, "intask", LPEL_MON_TASK_TIMES);
  LpelTaskRun(intask);

  LpelCleanup();
}




int main(void)
{
  testBasic();
  printf("test finished\n");
  return 0;
}
