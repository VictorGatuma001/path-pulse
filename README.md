# PathPulse
 
A C++17 terminal toolkit that **generates mazes with three different algorithms and races three solvers against each other**, then reports how each one performed.
 
> Which maze-solving algorithm is actually best, and does the answer change with the maze?
 
```
##S ##                              ##....******##
##**##########  ##################  ##..##**##**##
##**##******##  ##******##******##  ##..##**##**##
...
Solver    Path length  Cells explored     Time (us)
---------------------------------------------------
BFS               241            6861         274.3
DFS              1201            2961          96.5
A*                241            3798         381.4
```
*(Add a GIF of `--animate` here. It's the best thing on your README.)*
 
## Features
- **Generators:** Recursive Backtracker, Prim's, Kruskal's (with a disjoint-set)
- **Solvers:** BFS, DFS, A* (Manhattan heuristic)
- **Loops mode:** punch extra openings into the maze so several routes exist
- **Animation:** watch each solver explore the maze in colour
- **Benchmark table:** path length, cells explored, and search time
- **Reproducible:** `--seed` recreates the exact same maze
## Build & run
```bash
cmake -S . -B build
cmake --build build
./build/pathpulse --size 20x40 --animate
```
Or without CMake: `g++ -std=c++17 -O2 src/*.cpp -o pathpulse`
 
## Options
| Flag | Meaning | Default |
|---|---|---|
| `--size RxC` | maze size in cells | `12x25` |
| `--generator` | `backtracker`, `prim`, `kruskal` | `backtracker` |
| `--solver` | `bfs`, `dfs`, `astar`, `all` | `all` |
| `--loops P` | open P% of extra walls | `0` |
| `--seed N` | reproducible maze | random |
| `--animate` / `--delay MS` | animate the search | off / `15` |
| `--no-color` | plain ASCII | colour on |
 
## What I learned from the results
- In a **perfect maze** (no loops) there is only one route, so all solvers return the same path. They differ only in how much they explore.
- With **`--loops 15`**, DFS often returns a path several times longer than the optimum, while BFS and A* stay optimal.
- **A\*** usually explores fewer cells than BFS, but on small mazes its priority queue overhead can make it *slower* in wall-clock time. Fewer cells explored is not the same as faster.
- Generators have different "personalities": the backtracker makes long winding corridors, Prim's makes many short dead ends.
Try: `./pathpulse --size 40x80 --seed 1 --generator prim --loops 15`
 
## Testing
```bash
cmake -S . -B build && cmake --build build
cd build && ctest --output-on-failure
```
The test suite (`tests/test_pathpulse.cpp`, no external dependencies) checks that:
- every generator keeps the outer border solid and produces a *perfect* maze (no loops, no unreachable cells)
- the same `--seed` always recreates the same maze
- every solver returns a valid path (starts at `S`, ends at `E`, moves only through open adjacent cells)
- BFS and A* always find the shortest path, and DFS never beats it
- unsolvable mazes are correctly reported as unsolvable
## Project layout
```
src/Maze.*        grid, rendering
src/Generators.*  backtracker, Prim, Kruskal, loop-adder
src/Solvers.*     BFS, DFS, A*
src/main.cpp      CLI parsing, animation, benchmark table
tests/            automated test suite
```
 
## Ideas for next steps
- Export benchmark results to CSV and plot them
- Add Dijkstra and Greedy Best-First solvers
- Add weighted terrain (mud, water) so Dijkstra and A* shine
- Unit tests with Catch2 or GoogleTest
 
