// Dependency-free tests for PathPulse. Run with `ctest` or `./build/pathpulse_tests`.
#include "Generators.h"
#include "Maze.h"
#include "Solvers.h"

#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static int checks = 0, failures = 0;
#define CHECK(cond, msg)                                                              \
    do {                                                                              \
        ++checks;                                                                     \
        if (!(cond)) {                                                                \
            ++failures;                                                               \
            std::cerr << "FAIL: " << (msg) << "  [" << __FILE__ << ":" << __LINE__ << "]\n"; \
        }                                                                             \
    } while (0)

namespace {
const char* GENERATORS[] = {"backtracker", "prim", "kruskal"};

Maze build(const std::string& gen, int rows, int cols, unsigned seed, int loops = 0) {
    Maze m(rows, cols);
    std::mt19937 rng(seed);
    generateByName(gen, m, rng);
    addLoops(m, rng, loops);
    return m;
}

int countOpen(const Maze& m) {
    int n = 0;
    for (int r = 0; r < m.height(); ++r)
        for (int c = 0; c < m.width(); ++c)
            if (!m.isWall(r, c)) ++n;
    return n;
}

bool borderIntact(const Maze& m) {
    for (int r = 0; r < m.height(); ++r)
        for (int c = 0; c < m.width(); ++c)
            if ((r == 0 || c == 0 || r == m.height() - 1 || c == m.width() - 1) && !m.isWall(r, c))
                return false;
    return true;
}

bool sameGrid(const Maze& a, const Maze& b) {
    for (int r = 0; r < a.height(); ++r)
        for (int c = 0; c < a.width(); ++c)
            if (a.isWall(r, c) != b.isWall(r, c)) return false;
    return true;
}

// A path is valid if it runs start -> end and every step moves to an adjacent open cell.
bool validPath(const Maze& m, const SolveResult& res) {
    if (res.path.empty() || !(res.path.front() == m.start()) || !(res.path.back() == m.end())) return false;
    for (std::size_t i = 0; i < res.path.size(); ++i) {
        const Point& p = res.path[i];
        if (m.isWall(p.r, p.c)) return false;
        if (i > 0 && std::abs(p.r - res.path[i - 1].r) + std::abs(p.c - res.path[i - 1].c) != 1) return false;
    }
    return true;
}
}  // namespace

void testGenerators() {
    for (const char* gen : GENERATORS) {
        for (unsigned seed = 1; seed <= 25; ++seed) {
            Maze m = build(gen, 9, 14, seed);
            std::string tag = std::string(gen) + " seed " + std::to_string(seed);

            CHECK(borderIntact(m), tag + ": outer border must stay solid");
            CHECK(!m.isWall(1, 1) && !m.isWall(m.height() - 2, m.width() - 2), tag + ": start/end must be open");

            // A perfect maze is a spanning tree: open cells = cells + (cells - 1) passages.
            int cells = 9 * 14;
            CHECK(countOpen(m) == cells + (cells - 1), tag + ": expected a perfect maze (no loops, no islands)");

            // Every open cell must be reachable from the start.
            SolveResult bfs = solveBFS(m);
            CHECK(bfs.found, tag + ": end must be reachable");
            CHECK(static_cast<int>(bfs.visited.size()) <= countOpen(m), tag + ": visited more cells than exist");
        }
    }
}

void testDeterminism() {
    for (const char* gen : GENERATORS) {
        CHECK(sameGrid(build(gen, 10, 10, 42), build(gen, 10, 10, 42)), std::string(gen) + ": same seed must give same maze");
        CHECK(!sameGrid(build(gen, 10, 10, 42), build(gen, 10, 10, 43)), std::string(gen) + ": different seeds should differ");
    }
}

void testSolversOnPerfectMazes() {
    for (const char* gen : GENERATORS) {
        for (unsigned seed = 1; seed <= 25; ++seed) {
            Maze m = build(gen, 12, 18, seed);
            std::string tag = std::string(gen) + " seed " + std::to_string(seed);
            SolveResult bfs = solveBFS(m), dfs = solveDFS(m), astar = solveAStar(m);

            CHECK(validPath(m, bfs) && validPath(m, dfs) && validPath(m, astar), tag + ": every solver must return a valid path");
            // Only one route exists, so all solvers must agree on its length.
            CHECK(bfs.path.size() == dfs.path.size() && bfs.path.size() == astar.path.size(), tag + ": path lengths must match");
        }
    }
}

void testSolversWithLoops() {
    for (const char* gen : GENERATORS) {
        for (unsigned seed = 1; seed <= 25; ++seed) {
            Maze m = build(gen, 15, 20, seed, 20);
            std::string tag = std::string(gen) + " loops seed " + std::to_string(seed);
            SolveResult bfs = solveBFS(m), dfs = solveDFS(m), astar = solveAStar(m);

            CHECK(validPath(m, bfs) && validPath(m, dfs) && validPath(m, astar), tag + ": valid paths");
            CHECK(bfs.path.size() == astar.path.size(), tag + ": BFS and A* must both be optimal");
            CHECK(dfs.path.size() >= bfs.path.size(), tag + ": DFS can never beat the shortest path");
            CHECK(astar.visited.size() <= bfs.visited.size() + 2 * m.cellRows() * m.cellCols(), tag + ": A* exploration sanity bound");
        }
    }
}

void testHandBuiltMazes() {
    // 2x2 cells: carve only start and end, leaving them disconnected -> unsolvable.
    Maze blocked(2, 2);
    blocked.setWall(1, 1, false);
    blocked.setWall(3, 3, false);
    CHECK(!solveBFS(blocked).found && !solveDFS(blocked).found && !solveAStar(blocked).found,
          "blocked maze: no solver may claim a path");

    // 1x3 straight corridor: path is exactly 5 cells long (cells 1,2,3,4,5 on row 1).
    Maze corridor(1, 3);
    for (int c = 1; c <= 5; ++c) corridor.setWall(1, c, false);
    SolveResult res = solveBFS(corridor);
    CHECK(res.found && res.path.size() == 5, "corridor: expected a 5-cell path");
    CHECK(validPath(corridor, res), "corridor: path must be valid");
}

int main() {
    testGenerators();
    testDeterminism();
    testSolversOnPerfectMazes();
    testSolversWithLoops();
    testHandBuiltMazes();

    std::cout << checks - failures << "/" << checks << " checks passed\n";
    return failures == 0 ? 0 : 1;
}
