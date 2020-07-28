#include <stdio.h>
#include <stdlib.h>

typedef struct node_ {
  void *val;
  struct node_ *next;
} stack_node_t;

typedef struct stack_ {
  stack_node_t *top;
  int size;
  void (*free_fn)(void *);
} stack0_t;  // name it to stack0_t instead of stack0_t because the later has
             // been defined in system header file

stack0_t *create_stack(void (*free_fn)(void *)) {
  stack0_t *q;

  q = calloc(1, sizeof *q);
  if (!q) {
    fprintf(stderr, "error allocating memory for the creating queue\n");
    abort();
  }

  q->free_fn = free_fn;

  return q;
}

void release_stack(stack0_t *q) {
  if (!q) return;

  for (stack_node_t *n = q->top; n;) {
    if (q->free_fn) q->free_fn(n->val);

    stack_node_t *next = n->next;
    free(n);
    n = next;
  }
  free(q);
}

void push(stack0_t *q, void *v) {
  if (!q) return;

  stack_node_t *n, *prev;

  n = calloc(1, sizeof *n);
  n->val = v;

  prev = q->top;
  q->top = n;
  n->next = prev;

  q->size++;
}

void *pop(stack0_t *q) {
  if (!q || !q->top) return NULL;

  stack_node_t *n = q->top;
  q->top = q->top->next;

  void *out = n->val;
  free(n);

  return out;
}

int main() {}
