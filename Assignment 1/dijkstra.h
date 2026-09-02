#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

/* Holds a full single-source shortest-path solve, reusable for
 * repeated queries to many destinations from the SAME source
 * (e.g. "nearest hospital from this ambulance" against many hospitals,
 * or "distance from this fire station" to every intersection it covers)
 * without recomputing Dijkstra per destination. */
typedef struct {
    double *dist;   /* dist[i]  = shortest cost from source to node i */
    int    *prev;   /* prev[i]  = predecessor of node i on that path, -1 if none */
    int     n;       /* number of nodes at solve time                */
    int     src;      /* source node index                            */
} DijkstraResult;

/* Runs Dijkstra from src_idx over the whole graph. O((V+E) log E). */
DijkstraResult *dijkstra_run(const Graph *g, int src_idx);
void dijkstra_free(DijkstraResult *r);

/* Convenience one-to-one query: looks up names, runs Dijkstra, and
 * reconstructs the path as an array of intersection names.
 * Returns 0 on success (route found), -1 if either intersection is
 * unknown, -2 if no route exists. Caller must free *path_out and each
 * of its strings (or just call free_path()). */
int find_shortest_path(const Graph *g, const char *src_name, const char *dest_name,
                        char ***path_out, int *path_len_out, double *distance_out);

void free_path(char **path, int path_len);

/* Reconstructs a path to dest_idx from an already-computed DijkstraResult
 * (used for repeated one-to-many queries against a single solve).
 * Returns 0 on success, -2 if unreachable. */
int path_from_result(const Graph *g, const DijkstraResult *r, int dest_idx,
                      char ***path_out, int *path_len_out, double *distance_out);

#endif
