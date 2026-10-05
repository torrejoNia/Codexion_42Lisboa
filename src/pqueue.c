
#include <stdlib.h>
#include "pqueue.h"
#include "traceback.h"

t_pqueue	*pqueue_new(size_t size, t_cmp cmp)
{
	t_pqueue	*pq;

	pq = malloc(sizeof(t_pqueue));
	if (!pq)
		return (traceback(ERR_MEM, "pqueue_new"), NULL);
	pq->items = malloc(size * sizeof(t_request));
	if (!pq->items)
		return (free(pq), traceback(ERR_MEM, "pqueue_new"), NULL);
	pq->max = size;
	pq->len = 0;
	pq->cmp = cmp;
	return (pq);
}

void	pqueue_delete(t_pqueue *pq)
{
	free(pq->items);
	free(pq);
}

t_request	*pqueue_peek(t_pqueue *pq)
{
	if (pq->len == 0)
		return (NULL);
	return (&pq->items[0]);
}
