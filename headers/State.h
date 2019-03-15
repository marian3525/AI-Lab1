#include <utility>

#pragma once
#include <unordered_map>
#include <map>
#include <vector>

using namespace std;

class State {
public:
    State();
    State(vector<pair<int, pair<int, int>>> state, int size): state{state}, solSize{size}, currentSize{0} {};
    ~State();

    vector<State*> expand();

    int getGapValue(int i, int j) const;
    int getSolutionSize();
    const vector<pair<int, pair<int, int>>> getState() const;

private:
    int getCurrentSize();
    vector<pair<int, pair<int, int>>> state;
    int solSize;
    int currentSize;

};
