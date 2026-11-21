#pragma once
#include <cstddef>
#include <string>
#include <vector>

struct Point {
    int r;
    int c;
    bool operator==(const Point& o) const { return r == o.r && c == o.c; }
};

// A maze stored as a character-free boolean grid. The grid is (2*rows+1) x (2*cols+1):
// "cells" sit on odd coordinates and the walls between them on even coordinates.
class Maze {
public:
    Maze(int cellRows, int cellCols);

    int height() const { return h_; }
    int width() const { return w_; }
    int cellRows() const { return (h_ - 1) / 2; }
    int cellCols() const { return (w_ - 1) / 2; }

    bool inBounds(int r, int c) const { return r >= 0 && r < h_ && c >= 0 && c < w_; }
    // Inside the outer border (the border itself must always stay a wall).
    bool isInterior(int r, int c) const { return r >= 1 && r <= h_ - 2 && c >= 1 && c <= w_ - 2; }
    bool isWall(int r, int c) const { return wall_[r * w_ + c]; }
    void setWall(int r, int c, bool wall) { wall_[r * w_ + c] = wall; }

    Point start() const { return {1, 1}; }
    Point end() const { return {h_ - 2, w_ - 2}; }

    // Draw the maze. `visited` is shown up to `visitedCount` entries (for animation),
    // `path` is the final route (may be empty).
    std::string render(const std::vector<Point>& visited, std::size_t visitedCount,
                       const std::vector<Point>& path, bool color) const;

private:
    int h_;
    int w_;
    std::vector<char> wall_;
};
