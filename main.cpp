#include <iostream>
#include "headers/Controller.h"

int main() {
    Controller controller{};
    string sol = controller.solveProblem(bfs);
    std::cout<<sol;
    return 0;
}