#ifndef MINHEAP_H
#define MINHEAP_H

/*
 * Plain binary min-heap, used as Dijkstra's priority queue.
 *
 * Design choice: "lazy deletion" instead of decrease-key.
 * Every time a shorter distance to a node is found we simply push a new
 * (node, dist) pair; we never mutate an existing heap entry. When we pop,
 * we check the entry's distance against the authoritative dist[] array
 * and skip it if it is stale (a shorter one was already processed).
 * This is a few lines shorter and much easier to get right than an
 * indexed decrease-key heap, and for 10,000 edges the extra log-factor
 * (O(E log E) instead of O(E log V)) is unmeasurable in practice.
 */

typedef struct {
    int node;
    double dist;
} HeapItem;

typedef struct {
    HeapItem *items;
    int size;
    int capacity;
} MinHeap;

MinHeap *heap_create(int initial_capacity);
void     heap_destroy(MinHeap *h);
void     heap_push(MinHeap *h, int node, double dist);
HeapItem heap_pop(MinHeap *h);      /* caller must check heap_empty() first */
int      heap_empty(const MinHeap *h);

#endif
