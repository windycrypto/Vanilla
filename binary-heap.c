#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/binary-heap.h"

struct bheap *bheap_new(int capacity) {
    struct bheap *heap = calloc(1, sizeof *heap);
    heap->capacity = capacity;
    heap->elems = calloc(capacity+1, sizeof *heap->elems); // reserve one room for sentinel

    heap->elems[0] = INT_MIN; // sentinel elem

    return heap;
}

void bheap_insert(struct bheap *heap, int key) {
    int idx;

    if (heap->size >= heap->capacity) {
        fprintf(stderr, "heap is full\n");
        return;
    }

    for (idx = ++heap->size; heap->elems[idx>>1] > key ; idx = idx >> 1)
        heap->elems[idx] = heap->elems[idx>>1];
    heap->elems[idx] = key;

    return;
}

/*
 * remove the mininum(the first) element from the heap and return its value
 */
int bheap_delete(struct bheap *heap) {
    int idx, min, child, last;

    if (heap->size <= 0) {
        fprintf(stderr, "heap is empty\n");
        return heap->elems[0];
    }

    min = heap->elems[1];
    last = heap->elems[heap->size--];
    for (idx = 1 ; (idx << 1) <= heap->size; idx = child) {
        child = idx << 1;
        if (child < heap->size && heap->elems[child+1] < heap->elems[child])
            child++;

        if (heap->elems[child] < last)
            heap->elems[idx] = heap->elems[child];
    }
    heap->elems[idx] = last;

    return min;
}

void bheap_dump(struct bheap *heap) {
    for (int i = 1 ; i <= heap->size ; i++)
        if (i < heap->size) printf("%d ", heap->elems[i]);
        else printf("%d\n", heap->elems[i]);
}

int main() {
    struct bheap *heap = bheap_new(30);
    bheap_insert(heap, 3);
    bheap_insert(heap, 8);
    bheap_insert(heap, 10);
    bheap_insert(heap, 30);
    bheap_insert(heap, 50);
    bheap_insert(heap, 24);
    bheap_insert(heap, 27);
    bheap_insert(heap, 13);

    bheap_dump(heap);
}
