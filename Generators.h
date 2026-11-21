#pragma once
#include "Maze.h"
#include <random>
#include <string>

// Each generator carves a "perfect" maze (exactly one path between any two cells)
// into a Maze that starts out as solid wall.
void generateBacktracker(Maze& maze, std::mt19937& rng);  // long, winding corridors
void generatePrim(Maze& maze, std::mt19937& rng);         // lots of short dead ends
void generateKruskal(Maze& maze, std::mt19937& rng);      // uniform, branchy texture

// Knock down `percent` % of the walls that separate two open cells, creating loops
// so that more than one route exists (this is where solvers start to differ!).
void addLoops(Maze& maze, std::mt19937& rng, int percent);

// Returns false if the name is unknown.
bool generateByName(const std::string& name, Maze& maze, std::mt19937& rng);
