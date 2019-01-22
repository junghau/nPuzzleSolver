//#include <iostream>
#include <time.h>
#include <stdlib.h>

#include "DataStructures.h"
#include "Search.h"

//timer
#include "timeit.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

#include <thread>
#include <condition_variable>

//using std::cout;
//using std::endl;

#pragma warning(disable:4996)

State *parsePuzzle(const string &filename) {
    std::ifstream input;
    input.open(filename);

    if (!input.good()) {
        throw "The file does not exist!";
    }

    vector<vector<int>> vect_state;

    string line;

    Coord blankPos;
    int row_count = 0, col_count = 0, prev_col_count = 0;

    while (std::getline(input, line)) {
        std::stringstream lineStream(line);

        vector<int> vect_line;
        string item;

        //reset column
        if (prev_col_count*col_count != 0 && prev_col_count != col_count) {
            throw "The column sizes are not consistent!";
        }
        prev_col_count = col_count;
        col_count = 0;

        while (std::getline(lineStream, item, ',')) {
            int num = std::stoi(item);
            vect_line.push_back(num);

            if (num == 0) {
                blankPos.x = row_count;
                blankPos.y = col_count;
            }
            col_count++;
        }

        vect_state.push_back(vect_line);

        row_count++;
    }

    if (row_count != col_count) {
        throw "The n-Puzzle is not a square!";
    }

    return new nPuzzleState(vect_state, blankPos);
}

vector<string> solution;

int main(int argc, char* argv[]) {

    string filename;

    if (argc == 1)
        filename = "puzzle.txt";
    else
        filename = argv[1];

    try {

        timeit timer;

        State *initialState = parsePuzzle(filename);

        timer.start();
        vector<string> solution = IDA(initialState); //simply replace IDA with A
        timer.stop();

        printf("Time taken: %.8fs\n", timer.elapsed());

        if (solution.size() > 0) { //solution found
            printf("Solution found!\n");
            for (string step : solution) {
                printf("%s\n", step.c_str());
            }
        } else {
            printf("No solution\n");
        }

    } catch (char *e) {
        printf("%s\n", e);
        exit(1);
    }

    printf("Created: %ld\nDestroyed: %ld\n\n", create_count, destroy_count);
    system("pause");

    return 0;
}