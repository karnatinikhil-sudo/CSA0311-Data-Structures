#include <stdio.h>
#include <stdlib.h>
#include "minheap.h"

MinHeap *heap_create(int initial_capacity) {
    if (initial_capacity < 16) initial_capacity = 16;
    MinHeap *h = malloc(sizeof(MinHeap));
    h->items = malloc(sizeof(HeapItem) * (size_t)initial_capacity);
    h->size = 0;
    h->capacity = initial_capacity;
    return h;
}

void heap_destroy(MinHeap *h) {
    if (!h) return;
    free(h->items);
    free(h);
}

int heap_empty(const MinHeap *h) { return h->size == 0; }

static void swap(HeapItem *a, HeapItem *b) {
    HeapItem tmp = *a; *a = *b; *b = tmp;
}

static void sift_up(MinHeap *h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->items[parent].dist <= h->items[i].dist) break;
        swap(&h->items[parent], &h->items[i]);
        i = parent;
    }
}

static void sift_down(MinHeap *h, int i) {
    while (1) {
        int left = 2 * i + 1, right = 2 * i + 2, smallest = i;
        if (left < h->size && h->items[left].dist < h->items[smallest].dist) smallest = left;
        if (right < h->size && h->items[right].dist < h->items[smallest].dist) smallest = right;
        if (smallest == i) break;
        swap(&h->items[i], &h->items[smallest]);
        i = smallest;
    }
}

void heap_push(MinHeap *h, int node, double dist) {
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->items = realloc(h->items, sizeof(HeapItem) * (size_t)h->capacity);
        if (!h->items) { fprintf(stderr, "heap_push: out of memory\n"); exit(1); }
    }
    h->items[h->size].node = node;
    h->items[h->size].dist = dist;
    sift_up(h, h->size);
    h->size++;
}

HeapItem heap_pop(MinHeap *h) {
    HeapItem top = h->items[0];
    h->size--;
    h->items[0] = h->items[h->size];
    sift_down(h, 0);
    return top;
}
