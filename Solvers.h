#pragma once
#include "Maze.h"
#include <string>
#include <vector>

struct SolveResult {
    std::string name;
    bool found = false;
    std::vector<Point> path;     // start -> end
    std::vector<Point> visited;  // cells in the order the solver expanded them
    double micros = 0.0;         // wall-clock time of the search
};

SolveResult solveBFS(const Maze& maze);    // guarantees the shortest path
SolveResult solveDFS(const Maze& maze);    // fast if lucky, but path may be long
SolveResult solveAStar(const Maze& maze);  // shortest path, guided by Manhattan distance
