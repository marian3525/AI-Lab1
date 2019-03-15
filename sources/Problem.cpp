//
// Created by marian on 3/2/19.
//

#include "../headers/Problem.h"
#include <fstream>
#include <queue>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <map>

using namespace std;

Problem::Problem() {
    readFromFile();
    generateRegions();
}

/**
 * Read the input data from file
 */
void Problem::readFromFile() {
    ifstream file = ifstream{"/home/marian/Documents/CS/AI/lab1_C++/data4.in"};
    int n, aux;

    file >> n;

    size = n;

    vector<pair<int, pair<int, int>>> states;

    for (int i = 0; i < n; i++) {
        table.emplace_back();
        for (int j = 0; j < n; j++) {
            file >> aux;
            if (aux == -1) {
                states.push_back({-1, {i, j}});
                solutionSize += 1;
            }
            table[i].push_back(aux);
        }
    }

    this->initialState = State{states, solutionSize};
    this->root_size = int(sqrt(this->size));

    file.close();
}

/**
 *
 * @param state The state to check
 * @return True if state is a solution, false otherwise
 */
bool Problem::isSolution(State* state) {
    /**
     * Check if the current state represents a solution.
     * Iterate through the table and take the values from state when a -1 is found in the table.
     * A line/column must have the sum 1+2+...+9=45
     * for a solution to be valid, check if each sub-region contains each element once
     */
    if (state->getSolutionSize() != solutionSize) {
        return false;
    }

    // gaps = state

    int lineSum = 0, colSum = 0, regionSum = 0;

    // check the line and column sum
    for (int i = 0; i < size; i++) {

        for (int j = 0; j < size; j++) {
            lineSum += (table[i][j] != -1 ? table[i][j] : state->getGapValue(i, j));

            colSum += (table[j][i] != -1 ? table[j][i] : state->getGapValue(j, i));
        }

        if (lineSum != this->lineSum or colSum != this->lineSum) {
            return false;
        }

        lineSum = 0;
        colSum = 0;
    }

    // check all regions for duplicates
    for (int ir = regions[0]; ir < regions[root_size]; ir += root_size) {
        for (int jr = regions[0]; jr < regions[root_size]; jr += root_size) {
            for (int i = ir; i < ir + root_size; i++) {
                for (int j = jr; j < jr + root_size; j++) {
                    // i and j are indexes in a region
                    int value = (table[i][j] != -1 ? table[i][j] : state->getGapValue(i, j));
                    regionSum += value;
                }
            }
            if(regionSum != this->lineSum) {
                return false;
            }
            else {
                // print if found
                for (int i = ir; i < ir + root_size; i++) {
                    for (int j = jr; j < jr + root_size; j++) {
                        // i and j are indexes in a region
                        int value = (table[i][j] != -1 ? table[i][j] : state->getGapValue(i, j));
                        cout<<value<<" ";
                    }
                    cout<<endl;
                }
                cout<<endl<<endl;
            }
            regionSum = 0;
        }
    }

    return true;
}

/**
 *
 * Check if the state doesn't break any rules. If it does, return +inf.
 * Else, check the line, column and region for duplicates
 * If any were found, return -1
 * Else, check the frequencies of the element (last element in the state, which was added in the expansion)
 * used in the row, column and region.
 *
 * @param state1 The state to check
 * @param state2 The target state
 * @return A value representing the distance from the state to the target
 */
float Problem::heuristic(const State* state) {
    float h = 0.0f;

    map<int, int> lineMap;
    map<int, int> colMap;
    map<int, int> regionMap;
    map<int, int> tableMap;

    int li, co;

    // populate the frequency map for the lines and columns
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            li = (table[i][j] != -1 ? table[i][j] : state->getGapValue(i, j));
            co = (table[j][i] != -1 ? table[j][i] : state->getGapValue(j, i));

            lineMap.find(li) != lineMap.end() ? lineMap[li]++ : lineMap[li] = 1;
            colMap.find(co) != colMap.end() ? colMap[co]++ : colMap[co] = 1;
            tableMap.find(li) != tableMap.end() ? tableMap[li]++ : tableMap[li] = 1;
        }
        //check and reset here
        if(find_if(lineMap.begin(), lineMap.end(), [](pair<int, int> elem){ return elem.second >= 2;}) != lineMap.end()
           or find_if(colMap.begin(), colMap.end(), [](pair<int, int> elem){return elem.second >= 2;}) != colMap.end()) {
            // if there are duplicates on rows on columns, the state cannot lead to a solution
            return MAXFLOAT;
        }
        else {
            lineMap.clear();
            colMap.clear();
        }
    }

    // populate the frequency map on regions
    for (int ir = regions[0]; ir < regions[root_size]; ir += root_size) {
        for (int jr = regions[0]; jr < regions[root_size]; jr += root_size) {
            for (int i = ir; i < ir + root_size; i++) {
                for (int j = jr; j < jr + root_size; j++) {
                    // i and j are indexes in a region
                    int value = (table[i][j] != -1 ? table[i][j] : state->getGapValue(i, j));
                    regionMap.find(value) != regionMap.end() ? regionMap[value]++ : regionMap[value] = 1;
                }
            }
            // check if the last element added in the expansion has duplicates in the region
            if(find_if(regionMap.begin(), regionMap.end(), [](pair<int, int> elem){ return elem.second >= 2;}) != regionMap.end()) {
                // if there are duplicates, the state cannot lead to a solution
                return MAXFLOAT;
            }
            else {
                regionMap.clear();
            }
        }
    }

    // the current state is a candidate to result in a solution
    // assign a float value to the last element in the state and return it

    // Assign the highest probability to lead to a solution to elements with a relatively low frequency in the whole table
    // no std::reduce?
    // sort and check the index of the last elem. from state and assign decreasing values. If not in the list, return 0
    vector<pair<int, int>> tmp;
    tmp.resize(10);

    copy(tableMap.begin(), tableMap.end(), tmp.begin());
    sort(tmp.begin(), tmp.end(), [](pair<int, int> p1, pair<int, int> p2){ return p1.second < p2.second; });

    auto it = find_if(tmp.begin(), tmp.end(), [&](pair<int, int> p){ return p.second == state->getState().back().first; });

    if(it == tmp.end())
        return 0;
    else
        return it - tmp.begin();
}

/**
 * Best fit search
 */
bool Problem::gbfs() {
    auto comparator = [&] (const State* first, const State* second) {return heuristic(first) > heuristic(second);};
    priority_queue<State*, vector<State*>, decltype(comparator)> todo(comparator);

    int loops = 0;
    State* current;
    todo.push(&initialState);

    while(!todo.empty()) {
        current = todo.top();
        todo.pop();
        loops++;

        if(isSolution(current)) {
            solution = *current;
            cout<<"Loops: "<<loops<<endl;
            return true;
        }

        for(State* s : current->expand()) {
            todo.push(s);
        }
    }
    return false;
}


/**
 *
 * @return
 */
bool Problem::bfs() {
    queue<State*> todo;
    todo.push(&initialState);

    bool found = false;
    State* current;
    int loops = 0;

    while (!found and !todo.empty()) {
        loops++;

        current = todo.front();
        todo.pop();

        cout<<"{";
        for(auto e : current->getState()) {
            cout<<e.first<<", ";
        }
        cout<<"}"<<endl;


        if (isSolution(current)) {
            found = true;
            solution = *current;
            cout << "steps: " << loops << endl;
        }
        else {
            for (State* s : current->expand()) {
                todo.push(s);
            }
        }
    }
    solution = *current;
    return found;
}

/**
 *
 * @return
 */
State Problem::getSolution() {
    return solution;
}

/**
 *
 * @param state
 * @return
 */
string Problem::getPrintable(State state) {
    string output;

    for (int i = 0; i < table.size(); i++) {
        for (int j = 0; j < table.size(); j++)
            output += to_string(table[i][j] == -1 ? state.getGapValue(i, j) : table[i][j]) + " ";
        output += "\n";
    }
    return output;
}

/**
 *
 *
 */
void Problem::generateRegions() {
    regions.push_back(0);

    for (int i = 1; i < int(sqrt(size)) + 1; i++) {
        regions.push_back(i * this->root_size - 1);
    }

    for (int i = 1; i <= size; i++) {
        lineSum += i;
    }
}
