/* This file will contain your solution. Modify it as you wish. */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include "producerconsumer.h"

/* Declare any variables you need here to keep track of and
   synchronise your bounded buffer. A sample declaration of a buffer is shown
   below. It is an array of pointers to items.
   
   You can change this if you choose another implementation. 
   However, your implementation should accept at least BUFFER_SIZE 
   prior to blocking
*/

#define BUFFLEN (BUFFER_SIZE + 1)

data_item_t * item_buffer[BUFFER_SIZE+1];

volatile int head, tail;
struct cv *buffer_full;
struct cv *buffer_empty;
struct lock *buffer_lock;

/* consumer_receive() is called by a consumer to request more data. It
   should block on a sync primitive if no data is available in your
   buffer. It should not busy wait! */

data_item_t * consumer_receive(void)
{
        data_item_t * item;

        lock_acquire(buffer_lock);
        while(head == tail) {
                cv_wait(buffer_empty, buffer_lock);
        }
        item = item_buffer[tail];
        tail = (tail + 1) % BUFFLEN;
        cv_broadcast(buffer_full, buffer_lock);
        lock_release(buffer_lock);

        return item;
}

/* procucer_send() is called by a producer to store data in your
   bounded buffer.  It should block on a sync primitive if no space is
   available in your buffer. It should not busy wait!*/

void producer_send(data_item_t *item)
{
        lock_acquire(buffer_lock);
        while((head + 1) % BUFFLEN == tail) {
                cv_wait(buffer_full, buffer_lock);
        }
        item_buffer[head] = item;
        head = (head + 1) % BUFFLEN;
        cv_broadcast(buffer_empty, buffer_lock);
        lock_release(buffer_lock);
}




/* Perform any initialisation (e.g. of global data) you need
   here. Note: You can panic if any allocation fails during setup */

void producerconsumer_startup(void)
{
        head = tail = 0;
        buffer_full = cv_create("full");
        buffer_empty = cv_create("empty");
        buffer_lock = lock_create("buffer_lock");

}

/* Perform any clean-up you need here */
void producerconsumer_shutdown(void)
{
        cv_destroy(buffer_empty);
        cv_destroy(buffer_full);
        lock_destroy(buffer_lock);
}

