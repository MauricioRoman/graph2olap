#ifndef _STACK_H_
#define _STACK_H_


typedef struct {
        int *s;		            /* body of stack */
        int top;                /* position of top element */
        int count;              /* number of stack elements */
} stack;

void init_stack(stack *s);
int push(stack *s, int x);
int pop(stack *s);
int empty_stack(stack *s);
void print_stack(stack *s);
int stack_size(stack *s);

#endif
