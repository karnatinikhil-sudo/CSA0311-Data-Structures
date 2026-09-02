# AGENT TASK BRIEF — Emergency Vehicle Routing System

## Goal
Extend this existing C implementation of a real-time emergency vehicle
routing system into a runnable service, without changing the core
algorithm files (`graph.c/h`, `minheap.c/h`, `dijkstra.c/h`) unless a
verification step below fails.

## What already exists (do not re-derive from scratch)
- `graph.c/h`    — hash-indexed adjacency-list graph, O(1)-avg node lookup,
                    O(1)-avg live traffic updates via an edge hash table.
- `minheap.c/h`  — binary min-heap (lazy-deletion) used as Dijkstra's PQ.
- `dijkstra.c/h` — single-source shortest path, O((V+E) log E); supports
                    one-to-one (`find_shortest_path`) and one-to-many from
                    a single solve (`dijkstra_run` + `path_from_result`).
- `main.c`       — worked example + a benchmark that builds a 5,000-node /
                    ~30,000-directed-edge graph and times queries/updates.
- `Makefile`     — `make` builds `./emergency_routing`.

Baseline measured performance (already verified, re-check on this machine):
one-to-one query ≈ 2–3 ms; live traffic update ≈ 0.001 ms; both far under
the 200 ms budget.

## Tasks for the agent
1. **Build & verify baseline**
   - Run `make clean && make` — must compile with zero warnings under
     `-Wall -Wextra`.
   - Run `./emergency_routing` — confirm Part 1 output shows the route
     changing after the `I2->I3` traffic update, and Part 2 shows a
     one-to-one query time under 200 ms at 5,000 nodes.
   - If either check fails, treat it as a regression and fix before
     proceeding to the tasks below.

2. **Wrap as a queryable service** (pick one, note the choice in a
   `DECISIONS.md` you create):
   - a. A minimal TCP/HTTP server (POSIX sockets or a small C HTTP lib)
     exposing:
     - `GET /route?from=X&to=Y` → JSON `{ path: [...], cost: N }`
     - `POST /traffic {from, to, weight}` → 200 OK / 404 if road unknown
   - b. A CLI loop reading newline-delimited commands from stdin
     (`ROUTE from to`, `TRAFFIC from to weight`) if a network service is
     out of scope for this environment.
   - Either way: the graph must be built once at startup and held in
     memory; do not rebuild it per request.

3. **Concurrency safety**
   - If requests can arrive concurrently, protect `graph_update_traffic`
     writes with a mutex (readers of `find_shortest_path` may run
     concurrently with each other but not with a write). Document the
     chosen concurrency model in `DECISIONS.md`.

4. **Load real network data**
   - Add a loader (`load_graph_csv(Graph*, const char *path)`) that reads
     `from,to,weight,bidirectional` rows and calls `graph_add_edge` per
     row. Do not change `graph_add_edge`'s signature.
   - Add a small sample CSV (~20 rows) and a loader test in `main.c` or a
     new `test_loader.c`, guarded so it doesn't run inside the existing
     benchmark path.

5. **Regression test after every change**
   - Re-run `make clean && make && ./emergency_routing` after each task
     above and paste the timing lines into `DECISIONS.md` so performance
     drift is visible over time.

## Hard constraints (do not violate)
- No adjacency matrix, no Floyd-Warshall — graph stays sparse
  (~10,000 edges over 5,000+ nodes); do not introduce O(V^2) or O(V^3)
  structures.
- Traffic updates must remain O(1) average — do not replace the edge
  hash table with a linear scan.
- One-to-one queries must stay under 200 ms at 5,000+ nodes /
  10,000+ edges — treat any regression above this as a blocking bug.
- Keep the public function signatures in `graph.h`/`dijkstra.h` stable
  unless a task explicitly requires changing them; if you must change
  one, update every call site and re-run the full build+benchmark.

## Definition of done
- `make` builds clean, `./emergency_routing` runs and prints timings
  under budget.
- The new service/CLI wrapper can serve a route query and accept a
  traffic update without restarting or rebuilding the graph.
- `DECISIONS.md` exists summarizing what was built, why, and the
  before/after timing numbers.
