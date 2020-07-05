#include <stdio.h>
#include <stdlib.h>

#include "include/queue.h"

queue_t *create_queue(void (*free_fn)(void *)) {
    queue_t *q;

    q = calloc(1, sizeof *q);
    if (!q) {
        fprintf(stderr, "error allocating memory for the creating queue\n");
        abort();
    }

    q->free_fn = free_fn;

    return q;
}

void release_queue(queue_t *q) {
    if (!q) return;

    for (queue_node_t *n = q->head; n;) {
        if (q->free_fn) q->free_fn(n->val);

        queue_node_t *next = n->next;
        free(n);
        n = next;
    }
    free(q);
}

void enqueue(queue_t *q, void *v) {
    if (!q) return;

    queue_node_t *n = calloc(1, sizeof *n);
    n->val = v;

    if (!q->head) q->head = n;

    if (q->tail) {
        q->tail->next = n;
        q->tail = n;
    } else {
        q->tail = n;
    }
    q->size++;
}

void *dequeue(queue_t *q) {
    if (!q) return NULL;
    if (!q->head) return NULL;

    queue_node_t *n = q->head;

    if (q->head == q->tail) q->tail = q->head->next;

    q->head = q->head->next;
    q->size--;

    void *out = n->val;
    free(n);

    return out;
}
