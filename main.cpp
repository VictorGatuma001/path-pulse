#include "Generators.h"
#include "Maze.h"
#include "Solvers.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace {
struct Options {
    int rows = 12;
    int cols = 25;
    std::string generator = "backtracker";
    std::string solver = "all";
    unsigned seed = std::random_device{}();
    int loops = 0;
    bool animate = false;
    int delayMs = 15;
    bool color = true;
};

void printUsage() {
    std::cout <<
        "PathPulse - maze generator & solver visualizer\n\n"
        "Usage: pathpulse [options]\n"
        "  --size RxC            maze size in cells, e.g. 20x40      (default 12x25)\n"
        "  --generator NAME      backtracker | prim | kruskal        (default backtracker)\n"
        "  --solver NAME         bfs | dfs | astar | all             (default all)\n"
        "  --loops PERCENT       open extra walls to create loops    (default 0)\n"
        "  --seed N              reproducible maze                   (default random)\n"
        "  --animate             animate the solver exploring the maze\n"
        "  --delay MS            animation frame delay               (default 15)\n"
        "  --no-color            plain ASCII output\n"
        "  --help                show this message\n";
}

bool parseArgs(int argc, char** argv, Options& o) {
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> const char* { return (i + 1 < argc) ? argv[++i] : nullptr; };
        if (a == "--help" || a == "-h") {
            printUsage();
            std::exit(0);
        } else if (a == "--size") {
            const char* v = next();
            if (!v || std::sscanf(v, "%dx%d", &o.rows, &o.cols) != 2 || o.rows < 2 || o.cols < 2) {
                std::cerr << "Invalid --size (use e.g. 20x40, minimum 2x2)\n";
                return false;
            }
        } else if (a == "--generator") {
            const char* v = next();
            if (!v) return false;
            o.generator = v;
        } else if (a == "--solver") {
            const char* v = next();
            if (!v) return false;
            o.solver = v;
        } else if (a == "--loops") {
            const char* v = next();
            if (!v) return false;
            o.loops = std::atoi(v);
        } else if (a == "--seed") {
            const char* v = next();
            if (!v) return false;
            o.seed = static_cast<unsigned>(std::strtoul(v, nullptr, 10));
        } else if (a == "--delay") {
            const char* v = next();
            if (!v) return false;
            o.delayMs = std::atoi(v);
        } else if (a == "--animate") {
            o.animate = true;
        } else if (a == "--no-color") {
            o.color = false;
        } else {
            std::cerr << "Unknown option: " << a << "\n";
            return false;
        }
    }
    return true;
}

void animate(const Maze& maze, const SolveResult& res, const Options& o) {
    // Show ~150 frames at most so big mazes don't take forever.
    std::size_t step = std::max<std::size_t>(1, res.visited.size() / 150);
    std::cout << "\033[2J";
    for (std::size_t n = 0; n <= res.visited.size(); n += step) {
        std::cout << "\033[H" << "Solving with " << res.name << "  (" << n << "/" << res.visited.size()
                  << " cells explored)      \n"
                  << maze.render(res.visited, n, {}, o.color) << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(o.delayMs));
    }
    std::cout << "\033[H" << "Solving with " << res.name << "  - done!                          \n"
              << maze.render(res.visited, res.visited.size(), res.path, o.color) << std::flush;
}

void printTable(const std::vector<SolveResult>& results) {
    std::cout << "\n" << std::left << std::setw(8) << "Solver" << std::right << std::setw(13) << "Path length"
              << std::setw(16) << "Cells explored" << std::setw(14) << "Time (us)" << "\n"
              << std::string(51, '-') << "\n";
    for (const auto& r : results) {
        std::cout << std::left << std::setw(8) << r.name << std::right << std::setw(13)
                  << (r.found ? std::to_string(r.path.size()) : "n/a") << std::setw(16) << r.visited.size()
                  << std::setw(14) << std::fixed << std::setprecision(1) << r.micros << "\n";
    }
}
}  // namespace

int main(int argc, char** argv) {
    Options opt;
    if (!parseArgs(argc, argv, opt)) {
        printUsage();
        return 1;
    }

    std::mt19937 rng(opt.seed);
    Maze maze(opt.rows, opt.cols);
    if (!generateByName(opt.generator, maze, rng)) {
        std::cerr << "Unknown generator: " << opt.generator << "\n";
        return 1;
    }
    addLoops(maze, rng, opt.loops);

    std::vector<SolveResult> results;
    if (opt.solver == "bfs" || opt.solver == "all") results.push_back(solveBFS(maze));
    if (opt.solver == "dfs" || opt.solver == "all") results.push_back(solveDFS(maze));
    if (opt.solver == "astar" || opt.solver == "all") results.push_back(solveAStar(maze));
    if (results.empty()) {
        std::cerr << "Unknown solver: " << opt.solver << "\n";
        return 1;
    }

    std::cout << "Maze " << opt.rows << "x" << opt.cols << " | generator: " << opt.generator
              << " | loops: " << opt.loops << "% | seed: " << opt.seed << "\n";

    if (opt.animate) {
        for (const auto& r : results) animate(maze, r, opt);
    } else if (results.size() == 1) {
        std::cout << maze.render(results[0].visited, results[0].visited.size(), results[0].path, opt.color);
    } else {
        std::cout << maze.render({}, 0, {}, opt.color);
    }

    printTable(results);
    return 0;
}
