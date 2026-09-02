# Emergency Vehicle Routing System — Architecture & Decision Log

## Baseline Benchmark (Task 1)

### Build & Environment
- **Compiler**: MinGW GCC 6.3.0 (`gcc -O2 -Wall -Wextra -std=gnu11`)
- **OS**: Windows 11 (AMD64 / x86_64)
- **High-Resolution Clock**: `QueryPerformanceCounter` on Windows / `clock_gettime(CLOCK_MONOTONIC)` on POSIX

### Baseline Verification Output
```text
=====================================================
PART 1: Small readable example
=====================================================
Before traffic update:
  Route (5 hops, cost=9.50): Hospital -> I1 -> I2 -> I3 -> I4 -> Scene

Traffic update: I2->I3 congestion, new cost = 20.0
After traffic update:
  Route (4 hops, cost=11.50): Hospital -> I1 -> I2 -> I4 -> Scene

One-to-many queries from 'Hospital' (single solve reused):
  -> I2         Route (2 hops, cost=5.00): Hospital -> I1 -> I2
  -> I3         Route (2 hops, cost=8.00): Hospital -> I1 -> I3
  -> I4         Route (3 hops, cost=10.00): Hospital -> I1 -> I2 -> I4
  -> Scene      Route (4 hops, cost=11.50): Hospital -> I1 -> I2 -> I4 -> Scene

=====================================================
PART 2: Scale benchmark (5000 intersections)
=====================================================
Graph built: 5000 intersections, ~29998 roads, in 141.71 ms
One-to-one query N0 -> N4999: cost=27.00, hops=10, TIME=8.005 ms
Live traffic update (single road): TIME=0.0022 ms
20 repeated one-to-one queries: TOTAL=57.630 ms (avg 2.881 ms/query)
```

---

## Architectural Decisions

### 1. Service Wrapper & Web Application (Task 2)
- **Dual-Interface Design**: Implemented both an HTTP REST + Web Dashboard Microservice (`start_http_service`) and an interactive newline-delimited CLI mode (`run_cli_loop`).
- **Zero-Dependency Native Server**: Sockets implemented using Winsock2 on Windows and BSD Sockets on POSIX.
- **Embedded Web Application (Single-Page App)**:
  - Serves an interactive dark-mode Emergency Command Center dashboard directly from `GET /` on `http://localhost:8080/`.
  - Includes real-time SVG/Canvas road network graph rendering with drag-to-pan.
  - Interactive road click & slider controls to simulate live traffic congestion with instant O(1) recalculation and route reroute animation.
  - One-click trigger for the 5,000-node scale benchmark directly from the browser.
- **REST Endpoints**:
  - `GET /route?from=X&to=Y` — returns shortest path, hop count, and travel time in JSON (`200 OK`, `404 Not Found`, `422 Unprocessable`).
  - `POST /traffic` — parses JSON or query params `from=X&to=Y&weight=W`, applies O(1) update, returns confirmation JSON.
  - `GET /network` — returns all nodes and directed edges with current & base weights.
  - `GET /benchmark` — executes 5,000-node scale test and returns JSON metrics.
  - `GET /health` — health check and graph telemetry.

### 2. Concurrency Safety Model (Task 3)
- **Problem**: Route calculations (`find_shortest_path` / `dijkstra_run`) only read graph topology and edge weights, while traffic updates (`graph_update_traffic`) mutate edge weights.
- **Decision**: Read-Write Locking layer (`routing_rwlock_t` in `routing_lock.h`).
  - Read access (`routing_rwlock_rdlock`): Used during Dijkstra routing and graph inspection. Multiple concurrent client queries execute in parallel without blocking each other.
  - Write access (`routing_rwlock_wrlock`): Used during `graph_update_traffic` and CSV loading. Acquired exclusively for O(1) duration (~microseconds), ensuring thread safety with zero dirty reads.
  - Platform primitives: `CRITICAL_SECTION` / `SRWLOCK` on Windows and `pthread_rwlock_t` on POSIX.

### 3. CSV Network Loader (Task 4)
- **File**: `csv_loader.c` / `csv_loader.h` (`load_graph_csv`).
- **Format**: `from,to,weight,bidirectional`.
- **Robustness**: Automatically skips header rows, comments (`#`), whitespace, and CRLF/LF newlines.
- **Dataset**: Created `sample_network.csv` containing 23 road edges across 14 metropolitan emergency intersections.

---

## Performance Comparison & Regression History (Task 5)

| Metric | Task 1 Baseline | Task 5 Final Regression | Real-Time Budget | Status |
| :--- | :--- | :--- | :--- | :--- |
| **Compilation Warnings** | 0 warnings | 0 warnings (`-Wall -Wextra`) | 0 warnings | ✅ PASS |
| **Graph Build (5k nodes / ~30k edges)** | 141.71 ms | 125.94 ms | N/A | ✅ PASS |
| **Single 1-to-1 Query (`N0 -> N4999`)** | 8.005 ms | 3.435 ms | < 200 ms | ✅ PASS |
| **Avg 1-to-1 Query (20 queries)** | 2.881 ms | 2.614 ms | < 200 ms | ✅ PASS |
| **Live Traffic Update (single road)** | 0.0022 ms | 0.0017 ms | O(1) avg | ✅ PASS |
| **Concurrent Load Test (20 parallel jobs)** | N/A | 20 / 20 OK (100%) | Thread-safe | ✅ PASS |
| **CSV Loader Verification** | N/A | 23 edges loaded in < 1ms | Functional | ✅ PASS |
