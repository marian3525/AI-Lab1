#pragma once

#include "Problem.h"
#include "State.h"
#include <vector>
enum Mode{bfs, gbfs};

class Controller {
public:
    Controller();
    string solveProblem(Mode mode);
private:
    Problem problem;
};