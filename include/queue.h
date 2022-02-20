#pragma once
/* queue.h ---
 *
 *
 * Author:  team yeyeye
 * Created: Jan 31
 * Version: 1.0
 *
 * Description: Declares the supporting queue used to construct the OS.
 *
 *
 */

#include <yalnix.h>
#include <ykernel.h>
#include <utils.h>

/* the queue representation is hidden from users of the module */
typedef void queue_t;

/* create an empty queue */
queue_t *qopen(void);

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp);

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise
 */
u_int32_t qput(queue_t *qp, void *elementp);

/* get the first first element from queue, removing it from the queue */
void *qget(queue_t *qp);

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void *elementp));

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void *qsearch(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp);

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void *qremove(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp);

/* concatenates elements of q2 into q1
 * q2 is deallocated, closed, and unusable upon completion
 */
void qconcat(queue_t *q1p, queue_t *q2p);
