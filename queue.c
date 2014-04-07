/* Rutinas de grafos tomadas de Springer Verlag Programming Challenges (2003)
   Funciones para operar con colas
*/
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int init_queue(queue *q)
{
		q->first = 0;
		q->last = QUEUESIZE-1;
		q->count = 0;
		return 0;
}

int enqueue(queue *q, int x)
{
		if (q->count >= QUEUESIZE)
			printf("Warning: queue overflow enqueue x=%d\n",x);
		else {
			q->last = (q->last+1) % QUEUESIZE;
			q->q[ q->last] = x;
			q->count = q->count +1;
		}
		return 0;
}

int dequeue(queue *q)
{
		int x;

		if (q->count <= 0) printf("Warning: empty queue dequeue.\n");
		else {
			x = q->q[q->first];
			q->first = (q->first+1) % QUEUESIZE;
			q->count = q->count -1;
		}
		return(x);
}

int is_empty(queue *q)
{
		if (q->count <=0) return (TRUE);
		else return (FALSE);
}
