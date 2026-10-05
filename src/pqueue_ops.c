
#include "pqueue.h"

static void	swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

/* Move the item at index i up while it must be served before its parent. */
static void	sift_up(t_pqueue *pq, size_t i)
{
	size_t	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (pq->cmp(&pq->items[i], &pq->items[parent]) >= 0)
			return ;
		swap(&pq->items[i], &pq->items[parent]);
		i = parent;
	}
}

/* Move the item at index i down while a child must be served before it. */
static void	sift_down(t_pqueue *pq, size_t i)
{
	size_t	first;
	size_t	child;

	while (1)
	{
		first = i;
		child = 2 * i + 1;
		if (child < pq->len && pq->cmp(&pq->items[child],
				&pq->items[first]) < 0)
			first = child;
		child = 2 * i + 2;
		if (child < pq->len && pq->cmp(&pq->items[child],
				&pq->items[first]) < 0)
			first = child;
		if (first == i)
			return ;
		swap(&pq->items[i], &pq->items[first]);
		i = first;
	}
}

bool	pqueue_push(t_pqueue *pq, t_request item)
{
	if (pq->len >= pq->max)
		return (false);
	pq->items[pq->len] = item;
	pq->len += 1;
	sift_up(pq, pq->len - 1);
	return (true);
}

bool	pqueue_pop(t_pqueue *pq, t_request *out)
{
	if (pq->len == 0)
		return (false);
	if (out)
		*out = pq->items[0];
	pq->len -= 1;
	pq->items[0] = pq->items[pq->len];
	sift_down(pq, 0);
	return (true);
}
