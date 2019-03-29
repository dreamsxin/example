# Channels in libuv

`uv-channel.h`：

```c
typedef struct uv_chan_s uv_chan_t;

struct uv_chan_s {
  uv_mutex_t mutex;
  uv_cond_t cond;
  void* q[2];
};

int uv_chan_init(uv_chan_t* chan);
void uv_chan_send(uv_chan_t* chan, void* data);
void* uv_chan_receive(uv_chan_t* chan);
void uv_chan_destroy(uv_chan_t* chan);
```

`uv-channel.c`:
```c
typedef struct {
  void* data;
  uv_req_type type;
  void* active_queue[2];
} uv__chan_item_t;

int uv_chan_init(uv_chan_t* chan) {
  int r = uv_mutex_init (&chan->mutex);
  if (r == -1)
    return r;

  QUEUE_INIT(&chan->q);

  return uv_cond_init (&chan->cond);
}

void uv_chan_send(uv_chan_t* chan, void* data) {
  uv__chan_item_t* item = malloc(sizeof(uv__chan_item_t));
  item->data = data;

  uv_mutex_lock (&chan->mutex);
  QUEUE_INSERT_TAIL(&chan->q, &item->active_queue);
  uv_cond_signal (&chan->cond);
  uv_mutex_unlock (&chan->mutex);
}

void* uv_chan_receive(uv_chan_t* chan) {
  uv__chan_item_t* item;
  QUEUE* head;
  void* data = NULL;

  uv_mutex_lock (&chan->mutex);
  while (QUEUE_EMPTY(&chan->q)) {
    uv_cond_wait (&chan->cond, &chan->mutex);
  }

  head = QUEUE_HEAD (&chan->q);
  item = QUEUE_DATA (head, uv__chan_item_t, active_queue);
  data = item->data;
  QUEUE_REMOVE (head);
  free (item);
  uv_mutex_unlock (&chan->mutex);
  return data;
}

void uv_chan_destroy(uv_chan_t* chan) {
  uv_cond_destroy (&chan->cond);
  uv_mutex_destroy (&chan->mutex);
}
```

* Example

```c
#include "uv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void worker(void* arg) {
  int i;
  char* string;
  uv_chan_t* chan = arg;

  for (i = 0; i < 10; i++) {
    string = malloc(sizeof(char) * 2);
    sprintf(string, "%d", i);
    uv_chan_send (chan, string);
    uv_sleep (10);
  }
  uv_chan_send (chan, NULL);
}

int main(int argc, char** argv) {
  int threads_exited = 0;
  char* message;
  uv_chan_t chan;
  uv_thread_t thread1, thread2;

  uv_chan_init (&chan);
  uv_thread_create(&thread1, worker, &chan);
  uv_thread_create(&thread2, worker, &chan);

  while (threads_exited < 2) {
    message = uv_chan_receive(&chan);
    if (message == NULL)
      ++threads_exited;
    else {
      printf("Message : %\n", message);
      free(message);
    }
  }

  uv_chan_destroy (&chan);
  return 0;
}
```
