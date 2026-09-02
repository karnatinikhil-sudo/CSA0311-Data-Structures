#ifndef SERVICE_H
#define SERVICE_H

#include "graph.h"

/*
 * Starts the high-performance HTTP service and Web Application on the specified port.
 * Handles concurrent route queries and live traffic updates using Read-Write locking.
 *
 * Supported Endpoints:
 * - GET  /                             -> Interactive Dispatch Command Center Web App
 * - GET  /route?from=X&to=Y            -> JSON route path, hops, and travel time
 * - POST /traffic                      -> JSON/Query body: updates road weight in O(1)
 * - GET  /network                      -> JSON representation of nodes and edges
 * - GET  /benchmark                    -> Runs scale benchmark (5k nodes / 100 queries) and returns JSON metrics
 * - GET  /routes100 (or /routes-batch) -> Evaluates 100 emergency route queries and returns batch telemetry
 * - GET  /health                       -> Service health and stats
 */
int start_http_service(Graph *g, int port);

/*
 * Runs an interactive newline-delimited command-line interface on stdin.
 *
 * Supported Commands:
 * - ROUTE <from> <to>
 * - TRAFFIC <from> <to> <weight>
 * - LIST
 * - BENCHMARK
 * - HELP
 * - QUIT / EXIT
 */
int run_cli_loop(Graph *g);

#endif /* SERVICE_H */
