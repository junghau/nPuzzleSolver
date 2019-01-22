#ifndef PUZZLE_H
#define PUZZLE_H

#include <string>
#include <vector>
#include <math.h>

#include "DataStructures.h"

using std::string;
using std::vector;

//assuming square n-puzzle
inline double heuristic(State *puzzle_state) {
    nPuzzleState &npState = *static_cast<nPuzzleState*>(puzzle_state);
    size_t size = npState.size();

    double total = 0;
    size_t goal_row, goal_col;

    /*
    //allocate linear conflict table
    int *linear_conflict_table = new int[size*size];
    for (size_t i = 0; i < size*size; i++) {
        linear_conflict_table[i] = '\0';
    }*/

    for (size_t actual_row = 0; actual_row < size; actual_row++) {
        for (size_t actual_col = 0; actual_col < size; actual_col++) {
            int curr_tile = npState[actual_row][actual_col];

            if (curr_tile == 0)
                continue;

            if (curr_tile >= size) {
                goal_col = curr_tile % size;
                goal_row = (curr_tile - goal_col) / size;
            }
            else {
                goal_row = 0;
                goal_col = curr_tile - 1;
            }

            //block of code below for setting up linear conflict calculation
            /*
            char flag = '\0';
            //find if curr_tile's goal position is at the same row or col
            if (actual_row != goal_row || actual_col != goal_col) {
                if (actual_row == goal_row)
                    flag = 'H'; //horizontal
                else if (actual_col == goal_col)
                    flag = 'V'; //vertical
            }
            */

            //linear_conflict_table[actual_row * size + actual_col] = flag;

            double tile_heuristic = fabs((double)goal_row - actual_row) + fabs((double)goal_col - actual_col);
            total += tile_heuristic;
        }
    }

    /*double original = total;
    int counter;

    //look for 'H'orizontal in each row
    for (size_t row = 0; row < size; row++) {

        counter = 0;
        for (size_t k = 0; k < size; k++) {
            if (linear_conflict_table[row * size + k] == 'H')
                counter++;
        }
        if (counter >= 2) {
            total += 2;
        }

    }

    //look for 'V'ertical in each column
    for (size_t col = 0; col < size; col++) {

        counter = 0;
        for (size_t k = 0; k < size; k++) {
            if (linear_conflict_table[k * size + col] == 'V')
                counter++;
        }
        if (counter >= 2) {
            total += 2;
        }

    }

    //deallocate memory heap for linear_conflict_table
    delete[] linear_conflict_table;
    
    if (original != total) {
        //printf("LINEAR CONFLICT!\n");
    }
    
    //return total;
    return original;
    */
    return total;
}

//assume blank tile is at top-right
inline bool isGoalState(State *puzzle_state) {
    nPuzzleState &npState = *static_cast<nPuzzleState*>(puzzle_state);
    size_t size = npState.size();

    size_t counter = 1;

    for (size_t i = 0; i < size - 1; i++) {
        if (npState[0][i] != counter++)
            return false;
    }

    if (npState[0][size - 1] != 0) {
        return false;
    }

    for (size_t i = 1; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (npState[i][j] != counter++)
                return false;
        }
    }

    return true;
}


inline vector<string> getNextActions(State *puzzle_state) {
    nPuzzleState &npState = *static_cast<nPuzzleState*>(puzzle_state);
    size_t size = npState.size();
    Coord blankPos = npState.blank_pos;

    bool find_vertical = npState.last_direction != 'V';
    bool find_horizontal = npState.last_direction != 'H';

    vector<string> nextActions;

    //blockSz is the number of tiles move together as a block
    for (int blockSz = 1; blockSz < size; blockSz++) {
        string blockSz_str = std::to_string(blockSz);

        //vertical
        if (find_vertical) {
            if (blankPos.x - blockSz >= 0)
                nextActions.push_back(blockSz_str + "D");
            if (blankPos.x + blockSz < size)
                nextActions.push_back(blockSz_str + "U");
        }

        //horizontal
        if (find_horizontal) {
            if (blankPos.y - blockSz >= 0)
                nextActions.push_back(blockSz_str + "R");
            if (blankPos.y + blockSz < size)
                nextActions.push_back(blockSz_str + "L");
        }
    }

    return nextActions;
}

inline State *operate(State *old_state, string action) {
    nPuzzleState &np_oldState = *static_cast<nPuzzleState*>(old_state);
    // size_t size = np_oldState.size();

    nPuzzleState &np_newState = *new nPuzzleState(np_oldState);

    Coord old_blankPos = np_oldState.blank_pos;
    Coord new_blankPos;

    size_t blockSz = stoi(action.substr(0, action.length() - 1));
    char direction = action[action.length() - 1];

    for (size_t i = 0; i < blockSz; i++) {
        switch (direction) {
        case 'D':
            new_blankPos = old_blankPos - Coord(1, 0);
            break;
        case 'U':
            new_blankPos = old_blankPos + Coord(1, 0);
            break;
        case 'R':
            new_blankPos = old_blankPos - Coord(0, 1);
            break;
        case 'L':
            new_blankPos = old_blankPos + Coord(0, 1);
            break;
        }

        //swap the tile at old_blankPos to new_blankPos
        int tmp = np_newState[old_blankPos];
        np_newState[old_blankPos] = np_newState[new_blankPos];
        np_newState[new_blankPos] = tmp;

        old_blankPos = new_blankPos;
    }

    char last_direction;

    //get the last direction
    switch (direction) {
    case 'L':
    case 'R':
        last_direction = 'H'; //horizontal
        break;
    case 'U':
    case 'D':
        last_direction = 'V'; //vertical
        break;
    default:
        last_direction = '\0';
    }
    np_newState.blank_pos = new_blankPos;
    np_newState.last_direction = last_direction;

    return &np_newState;
}

//cost per move is always one regardless of the number of tile being moved together
inline double getCost(State *puzzle_state, string action) {
    //return stoi(action.substr(0, action.length() - 1));
    return 1;
}

#endif