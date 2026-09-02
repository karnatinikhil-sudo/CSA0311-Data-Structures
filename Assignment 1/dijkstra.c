#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "dijkstra.h"
#include "minheap.h"

#define INF (DBL_MAX / 4)

DijkstraResult *dijkstra_run(const Graph *g, int src_idx) {
    int n = g->num_nodes;
    DijkstraResult *r = malloc(sizeof(DijkstraResult));
    r->dist = malloc(sizeof(double) * (size_t)n);
    r->prev = malloc(sizeof(int) * (size_t)n);
    r->n = n;
    r->src = src_idx;

    /* visited[] gives O(1) "has this intersection already been finalized"
     * checks -- a dense array indexed by the hash-mapped node id, which
     * is the efficient in-memory equivalent of a HashSet<int> here. */
    unsigned char *visited = calloc((size_t)n, 1);

    for (int i = 0; i < n; i++) { r->dist[i] = INF; r->prev[i] = -1; }
    if (src_idx < 0 || src_idx >= n) { free(visited); return r; }
    r->dist[src_idx] = 0.0;

    MinHeap *h = heap_create(n < 16 ? 16 : n);
    heap_push(h, src_idx, 0.0);

    while (!heap_empty(h)) {
        HeapItem top = heap_pop(h);
        int u = top.node;

        if (visited[u]) continue;         /* stale lazy-deleted entry */
        visited[u] = 1;

        for (Edge *e = g->adj[u]; e; e = e->next) {
            int v = e->dest;
            if (visited[v]) continue;
            double nd = r->dist[u] + e->weight;
            if (nd < r->dist[v]) {
                r->dist[v] = nd;
                r->prev[v] = u;
                heap_push(h, v, nd);
            }
        }
    }

    heap_destroy(h);
    free(visited);
    return r;
}

void dijkstra_free(DijkstraResult *r) {
    if (!r) return;
    free(r->dist);
    free(r->prev);
    free(r);
}

static char **build_path(const Graph *g, const DijkstraResult *r, int dest_idx, int *len_out) {
    /* walk predecessors backward, then reverse */
    int count = 0;
    for (int cur = dest_idx; cur != -1; cur = r->prev[cur]) count++;

    char **path = malloc(sizeof(char *) * (size_t)count);
    int i = count - 1;
    for (int cur = dest_idx; cur != -1; cur = r->prev[cur]) {
        path[i--] = strdup(g->names[cur]);
    }
    *len_out = count;
    return path;
}

int path_from_result(const Graph *g, const DijkstraResult *r, int dest_idx,
                      char ***path_out, int *path_len_out, double *distance_out) {
    if (dest_idx < 0 || dest_idx >= r->n || r->dist[dest_idx] >= INF) {
        *path_out = NULL; *path_len_out = 0; *distance_out = -1.0;
        return -2; /* unreachable */
    }
    *path_out = build_path(g, r, dest_idx, path_len_out);
    *distance_out = r->dist[dest_idx];
    return 0;
}

int find_shortest_path(const Graph *g, const char *src_name, const char *dest_name,
                        char ***path_out, int *path_len_out, double *distance_out) {
    int src_idx = graph_get_node_index(g, src_name);
    int dest_idx = graph_get_node_index(g, dest_name);
    if (src_idx == -1 || dest_idx == -1) {
        *path_out = NULL; *path_len_out = 0; *distance_out = -1.0;
        return -1; /* unknown intersection */
    }

    DijkstraResult *r = dijkstra_run(g, src_idx);
    int rc = path_from_result(g, r, dest_idx, path_out, path_len_out, distance_out);
    dijkstra_free(r);
    return rc;
}

void free_path(char **path, int path_len) {
    if (!path) return;
    for (int i = 0; i < path_len; i++) free(path[i]);
    free(path);
}
