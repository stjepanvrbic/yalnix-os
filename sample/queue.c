/* queue.c ---
 *
 *
 * Author: team yeyeye
 * Created: Tue Feb 22
 * Version: 1.0
 *
 * Description: Implements a queue.
 *
 */

#include <yalnix.h>
#include <ykernel.h>
#include "../include/utils.h"

typedef struct node
{
    void *data;
    struct node *next;
} node_t;

typedef struct queue
{
    node_t *front;
    node_t *back;
} queue_t;

//---------------------------- qopen -------------------------------
// Description:   create an empty queue
// Inputs:        nothing
// Outputs:       returns pointer to an empty queue
//------------------------------------------------------------------
queue_t *qopen(void)
{
    queue_t *empty_q = malloc(sizeof(queue_t));
    empty_q->front = NULL;
    empty_q->back = NULL;
    return empty_q;
}

//---------------------------- qclose -------------------------------
// Description:   deallocate a queue, frees everything in it
// Inputs:        queue pointer
// Outputs:       deallocate queue, free memory
//-------------------------------------------------------------------
void qclose(queue_t *qp)
{
    // If the given queue is empty free the queue memory and return
    if (qp->front == NULL)
    {
        free(qp);
        return;
    }
    node_t *cnode; // current node
    node_t *pnode; // previous node

    pnode = qp->front;
    // Loop through the queue and free memory
    for (cnode = pnode->next; cnode != NULL; cnode = cnode->next)
    {
        free(pnode);
        pnode = cnode;
    }
    free(pnode);
    free(qp);
}

//---------------------------- qput --------------------------------
// Description:   put element at the end of the queue
// Inputs:        queue pointer and element to add to the queue
// Outputs:       returns 0 is successful; nonzero otherwise
//------------------------------------------------------------------
u_int32_t qput(queue_t *qp, void *elementp)
{
    node_t *element = malloc(sizeof(node_t));

    // If the given queue is empty...
    if (qp->front == NULL)
    {
        qp->front = element;
        qp->back = element;
        element->data = elementp;
        element->next = NULL;
        return 0;
    }
    // If the given queue is non-empty...
    else
    {
        qp->back->next = element;
        qp->back = element;
        element->data = elementp;
        element->next = NULL;
        return 0;
    }
    return 1;
}

//---------------------------- qget --------------------------------
// Description:   get the first element from queue,
//                removing it from the queue
// Inputs:        queue pointer
// Outputs:       pointer to first element of the queue
//                remove first element from the queue
//------------------------------------------------------------------
void *qget(queue_t *qp)
{
    // If the given queue is empty...
    if (qp->front == NULL)
    {
        return NULL;
    }
    // Remove and return node
    node_t *cnode;
    void *element;

    cnode = qp->front;
    qp->front = cnode->next;
    element = cnode->data;
    free(cnode);
    return element;
}

//---------------------------- qapply -------------------------------
// Description:   apply a function to every element of the queue
// Inputs:        function to apply to every element in the queue
//                queue pointer
// Outputs:       apply a function to every element of the queue
//-------------------------------------------------------------------
void qapply(queue_t *qp, void (*fn)(void *elementp))
{
    // If empty queue return
    if (qp->front == NULL)
    {
        return;
    }

    node_t *cnode;
    // Loop thru the entire queue
    for (cnode = qp->front; cnode != NULL; cnode = cnode->next)
    {
        fn(cnode->data);
    }
}

//---------------------------- qsearch -------------------------------
// Description:   search a queue using a supplied boolean function
// Inputs:        a search function, queue pointer, and key
// Outputs:       returns a pointer to an element, or NULL if not found
//--------------------------------------------------------------------
void *qsearch(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp)
{
    node_t *cnode;
    void *element;

    // If empty queue return
    if (qp->front == NULL)
    {
        return NULL;
    }

    bool find;
    // Loop through the queue and search each element
    for (cnode = qp->front; cnode != NULL; cnode = cnode->next)
    {
        find = searchfn(cnode->data, skeyp);
        if (find == true)
        {
            element = cnode->data;
            return element;
        }
    }
    return NULL;
}

//---------------------------- qremove -------------------------------
// Description:   search a queue using a supplied boolean function
//                (as in qsearch), removes the element from the queue
// Inputs:        a search function, queue pointer, and key
// Outputs:       returns a pointer to the element, or NULL if not found
//--------------------------------------------------------------------
void *qremove(queue_t *qp,
              bool (*searchfn)(void *elementp, const void *keyp),
              const void *skeyp)
{
    // If empty queue return
    if (qp->front == NULL)
    {
        return NULL;
    }

    node_t *cnode;
    node_t *pnode;
    void *element;
    bool find;

    cnode = qp->front;
    // If queue has only one element
    if (qp->front == qp->back)
    {
        find = searchfn(cnode->data, skeyp);
        if (find)
        {
            element = cnode->data;
            qp->front = NULL;
            qp->back = NULL;
            free(cnode);
            return element;
        }
    }

    // Check first element of multi element queue
    find = searchfn(cnode->data, skeyp);
    if (find)
    {
        element = cnode->data;
        qp->front = cnode->next;
        free(cnode);
        return element;
    }

    pnode = qp->front;
    // Loop through the queue and check each element
    for (cnode = pnode->next; cnode != NULL; cnode = cnode->next)
    {
        find = searchfn(cnode->data, skeyp);
        if (find)
        {
            // If the desired element is at the end of the queue
            if (cnode == qp->back)
            {
                pnode->next = NULL;
                qp->back = pnode;
                element = cnode->data;
                free(cnode);
                return element;
            }

            pnode->next = cnode->next;
            element = cnode->data;
            free(cnode);
            return element;
        }
        pnode = pnode->next;
    }
    return NULL;
}

//---------------------------- qconcat -------------------------------
// Description:   concatenates elements of q2 into q1
// Inputs:        pointers to both queues
// Outputs:       q2 is deallocated
//--------------------------------------------------------------------
void qconcat(queue_t *q1p, queue_t *q2p)
{
    node_t *cnode;

    // If empty queue return
    if (q2p->front == NULL)
    {
        return;
    }

    // Loop through queue2 and put each element into queue1
    for (cnode = q2p->front; cnode != NULL; cnode = cnode->next)
    {
        qput(q1p, (void *)cnode->data);
    }
    qclose(q2p);
}

//---------------------------- qisempty -------------------------------
// Description:   Checks if a queue is empty or not.
// Inputs:        pointers to on queue
// Outputs:       Returns TRUE if empty, FALSE if not.
//--------------------------------------------------------------------
bool qisempty(queue_t *qp)
{
    if (qp->front != NULL)
    {
        return false;
    }
    else
    {

        return true;
    }
}
