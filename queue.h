#ifndef _QUEUE_H_
#define _QUEUE_H_

#define QUEUESIZE   100000
#define TRUE        1
#define FALSE       0

typedef struct {
	int q[QUEUESIZE+1];				/* cuerpo de la cola */
	int first;						/* posici�n del primer elemento */
	int last;						/* posici�n del �ltimo elemento */
	int count;						/* n�mero de elementos en la cola */
} queue;

int init_queue(queue *q);
int enqueue(queue *q, int x);
int dequeue(queue *q);
int is_empty(queue *q);

#endif
