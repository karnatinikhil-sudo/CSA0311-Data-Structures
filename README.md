# CSA0311 — Data Structures & Algorithms

Welcome to the **CSA0311: Data Structures & Algorithms** coursework repository. This repository contains laboratory exercises, hands-on activities, assessment challenges, and comprehensive course assignments implemented in C and other modern computing tools.

---

## 📚 Repository Structure

```
CSA0311-Data-Structures/
├── README.md                      # Repository overview & curriculum index
├── Assignment 1/                  # Real-Time Smart-City Emergency Vehicle Routing System
│   ├── README.md                  # In-depth technical documentation & proofs
│   ├── DECISIONS.md               # Architecture decision log & data structure trade-offs
│   ├── AGENT_TASK.md              # Formal problem specification
│   ├── Makefile                   # C11 MinGW/GCC compilation system
│   ├── graph.h / graph.c          # Hash-Indexed Adjacency List & O(1) live traffic updater
│   ├── minheap.h / minheap.c      # Binary Min-Heap Priority Queue (Lazy Deletion)
│   ├── dijkstra.h / dijkstra.c    # Dijkstra SSSP solver with one-to-many solve reuse
│   ├── routing_lock.h             # Thread-safe Reader-Writer Lock concurrency layer
│   ├── csv_loader.h / csv_loader.c# Road network CSV parser
│   ├── service.h / service.c      # Embedded HTTP server, Siren synthesizer & Command Center UI
│   ├── main.c                     # Scale benchmark (5k nodes) & 100-route test suite
│   ├── metro_100_routes.csv       # 100-Route Metropolitan Emergency Road Network dataset
│   ├── sample_network.csv         # Metro Sector 4 network
│   ├── highway_corridor.csv       # Expressway corridor network
│   ├── downtown_grid.csv          # 16-block downtown grid network
│   └── baseline_demo.csv          # 6-node baseline worked example
├── Activities/                    # Data Structures Lab Activities & Concept Worksheets
│   ├── DS Activity-1.pdf
│   ├── DS Activity-2.jpeg
│   └── DS Activity-3.pdf
└── Assessment tools/              # Course Outcome (CO) Assessment Challenges & Exercises
    ├── C0-1 Concept Mapping.pdf
    ├── CO-1 Output prediction.pdf
    ├── CO-2 Analytical Problem Solving.pdf
    ├── CO-2 Debugging.pdf
    ├── CO-3 MCQ.jpeg
    ├── CO-3 Tree Traversal Challenges.pdf
    ├── CO-4 Coding Golf Competition.docx
    └── CO-4 ZigZaw Activity.jpeg
```

---

## 🚨 Featured Project: Assignment 1 — Real-Time Smart-City Emergency Vehicle Routing System

> 📖 **Full Documentation**: [Assignment 1 README](file:///c:/Users/karna/.gemini/antigravity-ide/scratch/CSA0311-Data-Structures/Assignment%201/README.md)

### Highlights:
* **Scale Target**: Operates seamlessly over networks with **$> 5,000$ intersections** and **$> 10,000$ roads**.
* **Sub-200 ms SLA**: Single-query Dijkstra shortest path executes in **$3.37\text{ ms}$**; 100-route batch executes in **$1.02\text{ ms}$** (average **$0.010\text{ ms}$ / route**).
* **$O(1)$ Live Traffic Updates**: Updates road congestion in **$0.0017\text{ ms}$ ($1.7\ \mu\text{s}$)** using a composite key edge hash table.
* **$165\times$ Memory Reduction**: Replaces wasteful $200\text{ MB}$ adjacency matrix with a cache-friendly $\sim 1.15\text{ MB}$ hash-indexed adjacency list.
* **Interactive Command Center**: Real-time canvas map, draggable intersection pins, Web Audio dual-tone ambulance siren synthesizer, voice dispatch narration, and turn-by-turn GPS directions.

---

## 🛠️ How to Compile & Run Assignment 1

```bash
# Navigate to Assignment 1 directory
cd "Assignment 1"

# Build with GCC C11
mingw32-make clean && mingw32-make

# Run 100-Route Metropolitan Evaluation
./emergency_routing.exe --routes-100 --load metro_100_routes.csv

# Run 5,000-Node Scale Benchmark
./emergency_routing.exe --benchmark

# Launch Web Application on Port 8080
./emergency_routing.exe --serve 8080 --load metro_100_routes.csv
```

Then open **`http://localhost:8080/`** in any web browser.

---

## 👥 Contributors & Institutional Repositories

| Contributor / Organization | Repository | Description |
|---|---|---|
| **Karnati Nikhil** | [karnatinikhil-sudo/CSA0311-Data-Structures](https://github.com/karnatinikhil-sudo/CSA0311-Data-Structures) | Main Coursework & Production Systems Repository |
| **SIMATS Tech** | [192521353simats-tech/CSA0311-DATA-STRUCTURES](https://github.com/192521353simats-tech/CSA0311-DATA-STRUCTURES) | Academic Evaluation & DSA Repository |
| **SIMATS Collab** | [192525173simats-collab/CSA0311-DATA-STRUCTURE](https://github.com/192525173simats-collab/CSA0311-DATA-STRUCTURE) | Collaborative Coursework & Lab Solutions |
