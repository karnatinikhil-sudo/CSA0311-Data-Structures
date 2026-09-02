# Assignment 1: Real-Time Smart-City Emergency Vehicle Routing System

## Problem Statement
A smart-city traffic management company is developing a real-time emergency vehicle routing system for ambulances and fire engines that receives road-network information containing intersections, roads, distances, and current traffic conditions, and must operate under the following constraints:
1. **Network Scale**: The road network may contain more than **5,000 intersections** and **10,000 roads**.
2. **Real-Time Latency**: The system must compute and provide an optimal route in **under 200 ms**.
3. **Optimality**: Emergency vehicles must be directed through the shortest weighted route taking into account live traffic congestion.
4. **Dynamic Traffic Updates**: The system must support frequent real-time updates to road conditions ($O(1)$ average time).
5. **Memory Efficiency**: Memory usage must remain within moderate computational resources ($O(V + E)$ sparse storage vs wasteful $O(V^2)$ matrix).
6. **Query Flexibility**: The system must support both **one-to-one route queries** and **repeated one-to-many route queries from emergency stations**.

---

## 🏗️ Architecture & Data Structures

| Component | Selected Data Structure / Algorithm | Time Complexity | Space Complexity | Why Selected |
| :--- | :--- | :--- | :--- | :--- |
| **Graph Storage** | Hash-Indexed Adjacency List (`adj[]`, `names[]`) | $O(1)$ avg node lookup | $O(V + E)$ | Eliminates $200\text{ MB}$ dense matrix; uses only $\sim 1.1\text{ MB}$ for 5,000 nodes. |
| **Pathfinding Engine** | Dijkstra's Algorithm + Binary Min-Heap | $O((V + E) \log E)$ | $O(V)$ | Optimal weighted shortest path in $\approx 2\text{--}3\text{ ms}$ ($82\times$ faster than 200ms limit). |
| **Visited Tracking** | Direct Byte Array / HashSet (`visited[]`) | $O(1)$ lookup / write | $O(V)$ | Cache-friendly instant finalized-node check with zero hash collisions. |
| **Live Traffic Updater** | Composite Key Edge Hash Map (`edge_table[]`) | $O(1)$ avg lookup / update | $O(E)$ | Instant pointer mutation in **$0.0017\text{ ms}$** without rebuilding graph. |
| **One-to-Many Routing** | Decoupled Solve Struct (`DijkstraResult`) | $O(\text{hops})$ per target | $O(V)$ | Solves from station once; multiple target scenes extracted in microseconds. |
| **Concurrency Layer** | Reader-Writer Lock (`routing_rwlock_t`) | Non-blocking concurrent reads | $O(1)$ | Multi-client safe: parallel route calculations with thread-safe traffic writes. |

---

## 📊 Measured Benchmark Results

### 1. Scale Benchmark (5,000 Intersections / ~30,000 Roads)
* **Graph Build Time**: $122.43\text{ ms}$
* **Single One-to-One Query (`N0 -> N4999`)**: **$3.373\text{ ms}$** *(Constraint: < 200 ms ✅)*
* **Live Traffic Update ($O(1)$)**: **$0.0017\text{ ms}$** ($1.7\ \mu\text{s}$)
* **100 Repeated One-to-One Queries**: Total: $241.3\text{ ms}$ | **Avg: $2.413\text{ ms}$/query** *(100% within SLA)*

### 2. 100-Route Metropolitan Emergency Evaluation (`metro_100_routes.csv`)
* **Total Routes Evaluated**: 100
* **Successful Routes**: 100 / 100 (100.0%)
* **Total Time for 100 Routes**: **$1.017\text{ ms}$**
* **Average Latency / Route**: **$0.0102\text{ ms}$** ($10.2\ \mu\text{s}$)

---

## 🚀 Build & Execution Instructions

### Prerequisites
* GCC Compiler (`gcc` with C11 support)
* Make / MinGW32-Make

### Compile
```bash
# Clean and build binary
make clean && make
```

### Run Benchmarks & Tests
```bash
# Run 100-route batch evaluation on metropolitan network
./emergency_routing.exe --routes-100 --load metro_100_routes.csv

# Run 5,000-node scale benchmark
./emergency_routing.exe --benchmark

# Run CSV loader test suite
./emergency_routing.exe --test-loader
```

### Run Interactive Web Service & Dashboard
```bash
# Start HTTP Service on Port 8080
./emergency_routing.exe --serve 8080 --load metro_100_routes.csv
```
Open **`http://localhost:8080/`** in your browser to access the real-time Emergency Command Center dashboard, featuring interactive canvas map, node dragging, ambulance siren audio synthesizer, live traffic simulator, and turn-by-turn GPS directions.

### Interactive CLI Mode
```bash
./emergency_routing.exe --cli --load metro_100_routes.csv
```
Commands:
* `ROUTE Hospital_General Incident_Highrise_Fire`
* `TRAFFIC Highway_Junction_101 Metro_Bridge_North 25.0`
* `LIST`
* `BENCHMARK`
