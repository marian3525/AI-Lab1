
#pragma once
#include "State.h"
#include <vector>
using namespace std;

class Problem {
public:
    Problem();
    bool bfs();
    bool gbfs();
    State getSolution();
    string getPrintable(State state);
    void readFromFile();

private:
    float heuristic(const State* state1);

    void generateRegions();
    bool isSolution(State* state);
    vector<vector<int>> table;
    int size;
    int solutionSize=0;

    int lineSum = 0;
    vector<int> regions;

    int root_size;
    State initialState;
    State solution;     // in this case solution is a state, may be a list of states or something else
};
