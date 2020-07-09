#include <iostream>
#include "lib/solver.h"

/**
 * Return a solver pointer.
 * 
 * It handles user input from the console, mainly board file and min erase condition
 */
PadSolver *handleInput(int, char **);

int main(int argc, char *argv[])
{
    auto soba = handleInput(argc, argv);
    std::cout << soba->solve() << std::endl;
    delete soba;

    return 0;
}

PadSolver *handleInput(int argc, char *argv[])
{
    std::string filePath = "assets/sample_board_65.txt";
    int minEraseCondition = 3;
    int maxStep = 25;
    int maxSize = 100;

    // Read from command line
    if (argc > 1)
    {
        filePath = argv[1];
    }
    if (argc > 2)
    {
        minEraseCondition = atoi(argv[2]);
    }
    if (argc > 3)
    {
        maxStep = atoi(argv[3]);
    }
    if (argc > 4)
    {
        maxSize = atoi(argv[4]);
    }

    return new PadSolver(filePath, minEraseCondition, maxStep, maxSize);
}
