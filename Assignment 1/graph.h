#ifndef GRAPH_H
#define GRAPH_H

/*
 * Hash-backed graph storage.
 *
 * - Intersections are referred to by string names (e.g. "I1042").
 * - node_table maps name -> dense integer index (O(1) average lookup),
 *   so the hot path of Dijkstra works on plain arrays (cache friendly,
 *   no string hashing inside the algorithm itself).
 * - Each node's roads are stored as a singly linked adjacency list
 *   (adj[index]), which is the standard O(V+E) space representation
 *   recommended for sparse road networks (10,000 roads over 5,000+
 *   intersections is very sparse -- an adjacency matrix would waste
 *   25,000,000 cells for ~10,000 real edges).
 * - edge_table maps (src_index, dest_index) -> Edge* directly, so a
 *   live traffic update ("road X is now congested") is an O(1) average
 *   hash lookup + a single field write, with NO need to rebuild or
 *   re-scan any adjacency list.
 */

#define NODE_HASH_SIZE  15013   /* prime, sized for >5000 intersections   */
#define EDGE_HASH_SIZE  20011   /* prime, sized for >10000 roads          */
#define MAX_NAME_LEN    64

typedef struct Edge {
    int dest;               /* destination node index               */
    double weight;          /* current traversal cost (traffic-adjusted) */
    double base_weight;     /* free-flow cost, kept for reference/reset  */
    struct Edge *next;
} Edge;

typedef struct NodeHashEntry {
    char name[MAX_NAME_LEN];
    int index;
    struct NodeHashEntry *next;   /* separate chaining */
} NodeHashEntry;

typedef struct EdgeHashEntry {
    int src, dest;
    Edge *edge_ptr;                /* direct pointer -> O(1) update */
    struct EdgeHashEntry *next;    /* separate chaining */
} EdgeHashEntry;

typedef struct Graph {
    int num_nodes;
    int capacity;               /* size of names[]/adj[] arrays */
    char **names;                /* index -> intersection name   */
    Edge **adj;                  /* index -> adjacency list head */
    NodeHashEntry *node_table[NODE_HASH_SIZE];
    EdgeHashEntry *edge_table[EDGE_HASH_SIZE];
} Graph;

Graph *graph_create(int initial_capacity);
void   graph_destroy(Graph *g);

/* Returns existing index for `name`, or creates a new node and returns it. */
int graph_get_or_add_node(Graph *g, const char *name);

/* Returns index of `name`, or -1 if the intersection does not exist. */
int graph_get_node_index(const Graph *g, const char *name);

/* Adds a road. If bidirectional != 0, adds the reverse road too.
 * Both directions are registered in edge_table for O(1) updates. */
void graph_add_edge(Graph *g, const char *from, const char *to,
                     double weight, int bidirectional);

/* O(1)-average live traffic update: retimes an existing road.
 * Returns 0 on success, -1 if the road does not exist. */
int graph_update_traffic(Graph *g, const char *from, const char *to,
                          double new_weight);

#endif
