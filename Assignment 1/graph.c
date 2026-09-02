#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* djb2 string hash -- fast, well distributed for short identifiers. */
static unsigned long hash_str(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++)) {
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash*33 + c */
    }
    return hash;
}

static unsigned long hash_edge(int src, int dest) {
    /* combine two ints into one well-mixed key */
    unsigned long key = (unsigned long)src * 1000003UL + (unsigned long)dest;
    return key;
}

Graph *graph_create(int initial_capacity) {
    if (initial_capacity < 16) initial_capacity = 16;
    Graph *g = calloc(1, sizeof(Graph));   /* calloc zeroes both hash tables */
    if (!g) { fprintf(stderr, "graph_create: out of memory\n"); exit(1); }
    g->num_nodes = 0;
    g->capacity = initial_capacity;
    g->names = malloc(sizeof(char *) * (size_t)initial_capacity);
    g->adj   = malloc(sizeof(Edge *) * (size_t)initial_capacity);
    if (!g->names || !g->adj) { fprintf(stderr, "graph_create: out of memory\n"); exit(1); }
    for (int i = 0; i < initial_capacity; i++) g->adj[i] = NULL;
    return g;
}

static void graph_grow(Graph *g) {
    int new_cap = g->capacity * 2;
    g->names = realloc(g->names, sizeof(char *) * (size_t)new_cap);
    g->adj   = realloc(g->adj,   sizeof(Edge *) * (size_t)new_cap);
    if (!g->names || !g->adj) { fprintf(stderr, "graph_grow: out of memory\n"); exit(1); }
    for (int i = g->capacity; i < new_cap; i++) g->adj[i] = NULL;
    g->capacity = new_cap;
}

int graph_get_node_index(const Graph *g, const char *name) {
    unsigned long idx = hash_str(name) % NODE_HASH_SIZE;
    for (NodeHashEntry *e = g->node_table[idx]; e; e = e->next) {
        if (strcmp(e->name, name) == 0) return e->index;
    }
    return -1;
}

int graph_get_or_add_node(Graph *g, const char *name) {
    int existing = graph_get_node_index(g, name);
    if (existing != -1) return existing;

    if (g->num_nodes == g->capacity) graph_grow(g);

    int new_index = g->num_nodes++;
    g->names[new_index] = strdup(name);
    g->adj[new_index] = NULL;

    NodeHashEntry *entry = malloc(sizeof(NodeHashEntry));
    strncpy(entry->name, name, MAX_NAME_LEN - 1);
    entry->name[MAX_NAME_LEN - 1] = '\0';
    entry->index = new_index;

    unsigned long h = hash_str(name) % NODE_HASH_SIZE;
    entry->next = g->node_table[h];
    g->node_table[h] = entry;

    return new_index;
}

/* Registers (src,dest) -> edge pointer for O(1) future traffic updates. */
static void edge_table_insert(Graph *g, int src, int dest, Edge *edge_ptr) {
    EdgeHashEntry *entry = malloc(sizeof(EdgeHashEntry));
    entry->src = src;
    entry->dest = dest;
    entry->edge_ptr = edge_ptr;
    unsigned long h = hash_edge(src, dest) % EDGE_HASH_SIZE;
    entry->next = g->edge_table[h];
    g->edge_table[h] = entry;
}

static void add_directed_edge(Graph *g, int from_idx, int to_idx, double weight) {
    Edge *e = malloc(sizeof(Edge));
    e->dest = to_idx;
    e->weight = weight;
    e->base_weight = weight;
    e->next = g->adj[from_idx];
    g->adj[from_idx] = e;
    edge_table_insert(g, from_idx, to_idx, e);
}

void graph_add_edge(Graph *g, const char *from, const char *to,
                     double weight, int bidirectional) {
    int from_idx = graph_get_or_add_node(g, from);
    int to_idx   = graph_get_or_add_node(g, to);
    add_directed_edge(g, from_idx, to_idx, weight);
    if (bidirectional) {
        add_directed_edge(g, to_idx, from_idx, weight);
    }
}

int graph_update_traffic(Graph *g, const char *from, const char *to,
                          double new_weight) {
    int from_idx = graph_get_node_index(g, from);
    int to_idx   = graph_get_node_index(g, to);
    if (from_idx == -1 || to_idx == -1) return -1;

    unsigned long h = hash_edge(from_idx, to_idx) % EDGE_HASH_SIZE;
    for (EdgeHashEntry *e = g->edge_table[h]; e; e = e->next) {
        if (e->src == from_idx && e->dest == to_idx) {
            e->edge_ptr->weight = new_weight;   /* O(1): single field write */
            return 0;
        }
    }
    return -1; /* road not found */
}

void graph_destroy(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->num_nodes; i++) {
        Edge *e = g->adj[i];
        while (e) { Edge *nxt = e->next; free(e); e = nxt; }
        free(g->names[i]);
    }
    free(g->names);
    free(g->adj);
    for (int i = 0; i < NODE_HASH_SIZE; i++) {
        NodeHashEntry *e = g->node_table[i];
        while (e) { NodeHashEntry *nxt = e->next; free(e); e = nxt; }
    }
    for (int i = 0; i < EDGE_HASH_SIZE; i++) {
        EdgeHashEntry *e = g->edge_table[i];
        while (e) { EdgeHashEntry *nxt = e->next; free(e); e = nxt; }
    }
    free(g);
}
