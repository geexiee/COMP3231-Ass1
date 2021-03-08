/* This file will contain your solution. Modify it as you wish. */
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <kern/errno.h>
#include "client_server.h"

#define BUFFSIZE 1000
#define BUFFLEN (BUFFSIZE+1)

// array of requests that we're using as a queue/buffer
request_t * buffer[BUFFLEN];
volatile int head, tail;

struct cv *buffer_full;
struct cv *buffer_empty;
struct lock *buffer_lock;

/* work_queue_enqueue():
 *
 * req: A pointer to a request to be processed. You can assume it is
 * a valid pointer or NULL. You can't assume anything about what it
 * points to, i.e. the internals of the request type.
 *
 * This function is expected to add requests to a single queue for
 * processing. The queue is a queue (FIFO). The function then returns
 * to the caller. It can be called concurrently by multiple threads.
 *
 * Note: The above is a high-level description of behaviour, not
 * detailed psuedo code. Depending on your implementation, more or
 * less code may be required. 
 */

   

void work_queue_enqueue(request_t *req)
{
        lock_acquire(buffer_lock);
        while((head + 1) % BUFFLEN == tail) {
                cv_wait(buffer_full, buffer_lock);
        }
        buffer[head] = req;
        head = (head + 1) % BUFFLEN;
        cv_broadcast(buffer_empty, buffer_lock);
        lock_release(buffer_lock);
}

/* 
 * work_queue_get_next():
 *
 * This function is expected to block on a synchronisation primitive
 * until there are one or more requests in the queue for processing.
 *
 * A pointer to the request is removed from the queue and returned to
 * the server.
 * 
 * Note: The above is a high-level description of behaviour, not
 * detailed psuedo code. Depending on your implementation, more or
 * less code may be required.
 */


request_t *work_queue_get_next(void)
{
        request_t * req;

        lock_acquire(buffer_lock);
        while(head == tail) {
                cv_wait(buffer_empty, buffer_lock);
        }
        req = buffer[tail];
        tail = (tail + 1) % BUFFLEN;
        cv_broadcast(buffer_full, buffer_lock);
        lock_release(buffer_lock);

        return req;
}




/*
 * work_queue_setup():
 * 
 * This function is called before the client and server threads are started. It is
 * intended for you to initialise any globals or synchronisation
 * primitives that are needed by your solution.
 *
 * In returns zero on success, or non-zero on failure.
 *
 * You can assume it is not called concurrently.
 */

int work_queue_setup(void)
{
        // initialising head/tail tracker and synchronisation primitives
        head = tail = 0;
        buffer_full = cv_create("full");
        if (buffer_full == NULL) 
                return 1;
        buffer_empty = cv_create("empty");
        if (buffer_empty == NULL)
                return 1;
        buffer_lock = lock_create("buffer_lock");
        if (buffer_lock == NULL) 
                return 1;
        return 0;

}


/* 
 * work_queue_shutdown():
 * 
 * This function is called after the participating threads have
 * exited. Use it to de-allocate or "destroy" anything allocated or created
 * on setup.
 *
 * You can assume it is not called concurrently.
 */

void work_queue_shutdown(void)
{
        cv_destroy(buffer_empty);
        cv_destroy(buffer_full);
        lock_destroy(buffer_lock);
}
