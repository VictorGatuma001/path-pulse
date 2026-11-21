#include "Generators.h"
#include <algorithm>
#include <numeric>

namespace {
constexpr int DR[4] = {-1, 0, 1, 0};
constexpr int DC[4] = {0, 1, 0, -1};

struct DisjointSet {
    std::vector<int> parent;
    explicit DisjointSet(int n) : parent(n) { std::iota(parent.begin(), parent.end(), 0); }
    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        parent[a] = b;
        return true;
    }
};
}  // namespace

void generateBacktracker(Maze& m, std::mt19937& rng) {
    std::vector<Point> stack;
    Point s{1, 1};
    m.setWall(s.r, s.c, false);
    stack.push_back(s);

    while (!stack.empty()) {
        Point cur = stack.back();
        int order[4] = {0, 1, 2, 3};
        std::shuffle(order, order + 4, rng);

        bool moved = false;
        for (int k : order) {
            int nr = cur.r + 2 * DR[k], nc = cur.c + 2 * DC[k];
            if (m.isInterior(nr, nc) && m.isWall(nr, nc)) {
                m.setWall(cur.r + DR[k], cur.c + DC[k], false);  // knock out the wall between
                m.setWall(nr, nc, false);
                stack.push_back({nr, nc});
                moved = true;
                break;
            }
        }
        if (!moved) stack.pop_back();  // dead end: backtrack
    }
}

void generatePrim(Maze& m, std::mt19937& rng) {
    struct Frontier {
        Point wall;
        Point cell;
    };
    std::vector<Frontier> frontier;

    auto addFrontier = [&](Point c) {
        for (int k = 0; k < 4; ++k) {
            Point cell{c.r + 2 * DR[k], c.c + 2 * DC[k]};
            if (m.isInterior(cell.r, cell.c) && m.isWall(cell.r, cell.c))
                frontier.push_back({{c.r + DR[k], c.c + DC[k]}, cell});
        }
    };

    Point s{1, 1};
    m.setWall(s.r, s.c, false);
    addFrontier(s);

    while (!frontier.empty()) {
        std::size_t i = rng() % frontier.size();
        Frontier f = frontier[i];
        frontier[i] = frontier.back();
        frontier.pop_back();
        if (m.isWall(f.cell.r, f.cell.c)) {  // cell not yet part of the maze
            m.setWall(f.wall.r, f.wall.c, false);
            m.setWall(f.cell.r, f.cell.c, false);
            addFrontier(f.cell);
        }
    }
}

void generateKruskal(Maze& m, std::mt19937& rng) {
    const int rows = m.cellRows(), cols = m.cellCols();
    struct Edge {
        int a, b;
        Point wall;
    };
    std::vector<Edge> edges;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            m.setWall(2 * r + 1, 2 * c + 1, false);  // every cell starts open and isolated
            if (c + 1 < cols) edges.push_back({r * cols + c, r * cols + c + 1, {2 * r + 1, 2 * c + 2}});
            if (r + 1 < rows) edges.push_back({r * cols + c, (r + 1) * cols + c, {2 * r + 2, 2 * c + 1}});
        }
    }

    std::shuffle(edges.begin(), edges.end(), rng);
    DisjointSet sets(rows * cols);
    for (const Edge& e : edges)
        if (sets.unite(e.a, e.b)) m.setWall(e.wall.r, e.wall.c, false);
}

void addLoops(Maze& m, std::mt19937& rng, int percent) {
    if (percent <= 0) return;
    std::vector<Point> candidates;
    for (int r = 1; r < m.height() - 1; ++r) {
        for (int c = 1; c < m.width() - 1; ++c) {
            if (!m.isWall(r, c)) continue;
            bool horizontal = (r % 2 == 1 && c % 2 == 0) && !m.isWall(r, c - 1) && !m.isWall(r, c + 1);
            bool vertical = (r % 2 == 0 && c % 2 == 1) && !m.isWall(r - 1, c) && !m.isWall(r + 1, c);
            if (horizontal || vertical) candidates.push_back({r, c});
        }
    }
    std::shuffle(candidates.begin(), candidates.end(), rng);
    std::size_t n = candidates.size() * std::min(percent, 100) / 100;
    for (std::size_t i = 0; i < n; ++i) m.setWall(candidates[i].r, candidates[i].c, false);
}

bool generateByName(const std::string& name, Maze& maze, std::mt19937& rng) {
    if (name == "backtracker") generateBacktracker(maze, rng);
    else if (name == "prim") generatePrim(maze, rng);
    else if (name == "kruskal") generateKruskal(maze, rng);
    else return false;
    return true;
}
