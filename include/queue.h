#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct node_ {
    void *val;
    struct node_ *next;
} queue_node_t;

typedef struct queue_ {
    queue_node_t *head;
    queue_node_t *tail;
    int size;
    void (*free_fn)(void *);
} queue_t;

queue_t *create_queue(void (*free_fn)(void *));
void release_queue(queue_t *q);
void enqueue(queue_t *q, void *v);
void *dequeue(queue_t *q);

#endif /* QUEUE_H_ */
