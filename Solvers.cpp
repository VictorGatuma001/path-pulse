#include "Solvers.h"
#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <queue>
#include <stack>
#include <utility>

namespace {
constexpr int DR[4] = {-1, 0, 1, 0};
constexpr int DC[4] = {0, 1, 0, -1};

using Clock = std::chrono::steady_clock;

double elapsedMicros(Clock::time_point t0) {
    return std::chrono::duration<double, std::micro>(Clock::now() - t0).count();
}

std::vector<Point> buildPath(const Maze& m, const std::vector<int>& parent) {
    std::vector<Point> path;
    const int w = m.width();
    int cur = m.end().r * w + m.end().c;
    while (cur != -1) {
        path.push_back({cur / w, cur % w});
        cur = parent[cur];
    }
    std::reverse(path.begin(), path.end());
    return path;
}
}  // namespace

SolveResult solveBFS(const Maze& m) {
    SolveResult res;
    res.name = "BFS";
    const int w = m.width();
    std::vector<int> parent(m.height() * w, -2);  // -2 = unseen, -1 = root
    std::queue<int> q;

    auto t0 = Clock::now();
    int s = m.start().r * w + m.start().c, e = m.end().r * w + m.end().c;
    parent[s] = -1;
    q.push(s);
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        res.visited.push_back({cur / w, cur % w});
        if (cur == e) {
            res.found = true;
            break;
        }
        for (int k = 0; k < 4; ++k) {
            int nr = cur / w + DR[k], nc = cur % w + DC[k];
            if (!m.inBounds(nr, nc) || m.isWall(nr, nc)) continue;
            int n = nr * w + nc;
            if (parent[n] != -2) continue;
            parent[n] = cur;
            q.push(n);
        }
    }
    res.micros = elapsedMicros(t0);
    if (res.found) res.path = buildPath(m, parent);
    return res;
}

SolveResult solveDFS(const Maze& m) {
    SolveResult res;
    res.name = "DFS";
    const int w = m.width();
    std::vector<int> parent(m.height() * w, -2);
    std::vector<char> seen(m.height() * w, 0);
    std::stack<std::pair<int, int>> st;  // (node, who pushed it)

    auto t0 = Clock::now();
    int s = m.start().r * w + m.start().c, e = m.end().r * w + m.end().c;
    st.push({s, -1});
    while (!st.empty()) {
        auto [cur, from] = st.top();
        st.pop();
        if (seen[cur]) continue;
        seen[cur] = 1;
        parent[cur] = from;
        res.visited.push_back({cur / w, cur % w});
        if (cur == e) {
            res.found = true;
            break;
        }
        for (int k = 0; k < 4; ++k) {
            int nr = cur / w + DR[k], nc = cur % w + DC[k];
            if (!m.inBounds(nr, nc) || m.isWall(nr, nc)) continue;
            int n = nr * w + nc;
            if (!seen[n]) st.push({n, cur});
        }
    }
    res.micros = elapsedMicros(t0);
    if (res.found) res.path = buildPath(m, parent);
    return res;
}

SolveResult solveAStar(const Maze& m) {
    SolveResult res;
    res.name = "A*";
    const int w = m.width();
    const int total = m.height() * w;
    std::vector<int> parent(total, -2), g(total, INT_MAX);
    std::vector<char> closed(total, 0);
    using Node = std::pair<int, int>;  // (f = g + h, index)
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open;

    auto t0 = Clock::now();
    const Point goal = m.end();
    auto h = [&](int idx) { return std::abs(idx / w - goal.r) + std::abs(idx % w - goal.c); };

    int s = m.start().r * w + m.start().c, e = goal.r * w + goal.c;
    g[s] = 0;
    parent[s] = -1;
    open.push({h(s), s});
    while (!open.empty()) {
        int cur = open.top().second;
        open.pop();
        if (closed[cur]) continue;
        closed[cur] = 1;
        res.visited.push_back({cur / w, cur % w});
        if (cur == e) {
            res.found = true;
            break;
        }
        for (int k = 0; k < 4; ++k) {
            int nr = cur / w + DR[k], nc = cur % w + DC[k];
            if (!m.inBounds(nr, nc) || m.isWall(nr, nc)) continue;
            int n = nr * w + nc;
            int tentative = g[cur] + 1;
            if (tentative < g[n]) {
                g[n] = tentative;
                parent[n] = cur;
                open.push({tentative + h(n), n});
            }
        }
    }
    res.micros = elapsedMicros(t0);
    if (res.found) res.path = buildPath(m, parent);
    return res;
}
