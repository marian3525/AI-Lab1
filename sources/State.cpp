//
// Created by marian on 3/2/19.
//

#include <iostream>
#include <algorithm>
#include "../headers/State.h"

State::State() {

}

/**
 *
 */
int State::getGapValue(int i, int j) const {
    auto it = std::find_if(state.begin(), state.end(),[&](auto elem){ return elem.second.first == i and elem.second.second == j; });

    if(it != state.end()) {
        return it->first;
    }
    else {
        return -1;
    }
}

vector<State*> State::expand() {
    vector<State*> expanded;
    // copy by value. New elements will be inserted in current and state needs to remain untouched
    auto current = this->state;

    if(getCurrentSize() == solSize) {
        // nothing to expand, vector complete
        return expanded;
    }

    // find the first location available
    int pos = getCurrentSize();
    if(pos == -1) {
       return expanded;
    }

    State* aux;

    for(int i=1; i<=solSize; i++) {
        current[pos].first = i;

        aux = new State{current, solSize};
        expanded.push_back(aux);
    }
    return expanded;
}

int State::getSolutionSize() {
    int size = 0;

    for(pair<int, pair<int, int>> p : this->state) {
        if(p.first != -1) {
            size++;
        }
    }
    return size;
}

const vector<pair<int, pair<int, int>>> State::getState() const {
    return this->state;
}

State::~State() {

}

int State::getCurrentSize() {
    auto it = find_if(state.begin(), state.end(), [&](auto elem){ return elem.first == -1; });
    if(it != state.end()) {
        return static_cast<int>(it - state.begin());
    }
    else {
        return -1;
    }
}
