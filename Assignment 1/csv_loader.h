#ifndef CSV_LOADER_H
#define CSV_LOADER_H

#include "graph.h"

/*
 * Reads a CSV file containing road network edges and populates the graph.
 *
 * Supported format:
 * from,to,weight,bidirectional
 *
 * Example:
 * Hospital,I1,2.0,1
 * I1,I2,3.5,1
 *
 * Features:
 * - Automatically skips header line if present.
 * - Handles both Windows (\r\n) and Unix (\n) line endings.
 * - Ignores empty lines and lines starting with '#' (comments).
 * - Trims leading/trailing whitespace around tokens.
 *
 * Returns the number of edges successfully loaded, or -1 on file read error.
 */
int load_graph_csv(Graph *g, const char *path);

#endif /* CSV_LOADER_H */
