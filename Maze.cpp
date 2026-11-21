#include "Maze.h"
#include <sstream>

Maze::Maze(int cellRows, int cellCols)
    : h_(2 * cellRows + 1), w_(2 * cellCols + 1), wall_(static_cast<std::size_t>(h_) * w_, 1) {}

std::string Maze::render(const std::vector<Point>& visited, std::size_t visitedCount,
                         const std::vector<Point>& path, bool color) const {
    // 0 = nothing, 1 = visited by the solver, 2 = on the final path
    std::vector<char> mark(static_cast<std::size_t>(h_) * w_, 0);
    for (std::size_t i = 0; i < visitedCount && i < visited.size(); ++i)
        mark[visited[i].r * w_ + visited[i].c] = 1;
    for (const Point& p : path) mark[p.r * w_ + p.c] = 2;

    const std::string RESET = "\033[0m";
    std::ostringstream out;
    for (int r = 0; r < h_; ++r) {
        for (int c = 0; c < w_; ++c) {
            Point here{r, c};
            if (here == start()) {
                out << (color ? "\033[1;32mS " + RESET : "S ");
            } else if (here == end()) {
                out << (color ? "\033[1;31mE " + RESET : "E ");
            } else if (isWall(r, c)) {
                out << (color ? "\033[90m\xE2\x96\x88\xE2\x96\x88" + RESET : "##");
            } else if (mark[r * w_ + c] == 2) {
                out << (color ? "\033[93m\xE2\x96\x88\xE2\x96\x88" + RESET : "**");
            } else if (mark[r * w_ + c] == 1) {
                out << (color ? "\033[36m\xE2\x96\x91\xE2\x96\x91" + RESET : "..");
            } else {
                out << "  ";
            }
        }
        out << '\n';
    }
    return out.str();
}
