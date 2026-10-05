
#ifndef PQUEUE_H
# define PQUEUE_H

# include <stddef.h>
# include <stdbool.h>
# include "cmp.h"

/**
 * @brief Priority queue of dongle requests, implemented as a binary heap.
 *
 * The heap is stored in an array: the children of index i are at
 * 2i + 1 and 2i + 2. The root (index 0) is always the request that the
 * comparator says must be served first.
 */
typedef struct s_pqueue
{
	/** @brief Array holding the heap. */
	t_request	*items;
	/** @brief Maximum capacity of the queue. */
	size_t		max;
	/** @brief Current number of stored requests. */
	size_t		len;
	/** @brief Scheduler used to order the requests. */
	t_cmp		cmp;
}	t_pqueue;

/**
 * @brief Create a new priority queue.
 *
 * @param size Maximum number of requests in the queue.
 * @param cmp Scheduler used to order the requests.
 * @return A newly allocated priority queue, or `NULL` on allocation failure.
 */
t_pqueue	*pqueue_new(size_t size, t_cmp cmp);

/**
 * @brief Destroy a priority queue.
 *
 * @param pq Priority queue to destroy.
 */
void		pqueue_delete(t_pqueue *pq);

/**
 * @brief Return the request that must be served first, without removing it.
 *
 * @param pq Priority queue.
 * @return The request, or `NULL` if the queue is empty.
 */
t_request	*pqueue_peek(t_pqueue *pq);

/**
 * @brief Insert a request and restore the heap order (sift up).
 *
 * @param pq Priority queue.
 * @param item Request to insert (copied into the queue).
 * @return `true` on success, `false` if the queue is full.
 */
bool		pqueue_push(t_pqueue *pq, t_request item);

/**
 * @brief Remove the first request and restore the heap order (sift down).
 *
 * @param pq Priority queue.
 * @param out If not `NULL`, the removed request is copied here.
 * @return `true` on success, `false` if the queue is empty.
 */
bool		pqueue_pop(t_pqueue *pq, t_request *out);

#endif
