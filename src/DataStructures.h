#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES

#include <unordered_map>
#include <string>
#include <vector>

#include "minHeap.h"

size_t create_count = 0;
size_t destroy_count = 0;

using std::string;
using std::vector;

//only working special case, i.e. < 15puzzle
//#define Fifteen_Puzzle

struct Coord {
    int x;
    int y;
    Coord(int x=0, int y=0) : x(x), y(y) {}

    Coord operator+(const Coord &right) {
        return Coord(x + right.x, y + right.y);
    }

    Coord operator-(const Coord &right) {
        return Coord(x - right.x, y - right.y);
    }

};

class State {

protected:
    size_t puzzle_size;

public:
    State(size_t size=0) :puzzle_size(size) {}

    virtual ~State() {}

    /*virtual auto &operator[](size_t index) = 0;

    virtual auto &operator[](const Coord &coord) = 0;*/

    virtual string getKey() = 0;

    size_t size() {
        return puzzle_size;
    }

};


//this is just a normal list in the Python3 implementation of n-puzzle solver
struct nPuzzleState : public State {
    int **puzzle_state;
    Coord blank_pos;
    char last_direction; //to reduce redundancy and in turns reduce memory consumption and better performance

    nPuzzleState(int **puzzle_state, size_t size, Coord &blank_pos, char last_direction = '\0') :
        State(size), 
        puzzle_state(puzzle_state),
        blank_pos(blank_pos), 
        last_direction(last_direction) 
    { 
        create_count++; 
    }

    nPuzzleState(const vector<vector<int>> &vector_state, Coord &blank_pos, char last_direction = '\0') :
        State(vector_state.size()),
        puzzle_state(convertFromVector(vector_state)),
        blank_pos(blank_pos),
        last_direction(last_direction)
    {
        create_count++;
    }

    nPuzzleState(nPuzzleState &right):
        State(right.size()),
        blank_pos(right.blank_pos),
        last_direction(right.last_direction)
    
    {
        create_count++;
        puzzle_state = new int*[puzzle_size];

        for (size_t i = 0; i < puzzle_size; i++) {

            puzzle_state[i] = new int[puzzle_size];

            for (size_t j = 0; j < puzzle_size; j++) {
                puzzle_state[i][j] = right.puzzle_state[i][j];
            }
        }
    }

    ~nPuzzleState() {
        destroy_count++;
        if (puzzle_state) {
            for (size_t i = 0; i < puzzle_size; i++) {
                delete[] puzzle_state[i];
            }
            delete[] puzzle_state;
        }
    }

    int* &operator[](size_t index) {
        return puzzle_state[index];
    }

    int &operator[](const Coord &coord) {
        return puzzle_state[coord.x][coord.y];
    }

#ifdef Fifteen_Puzzle
    string getKey() override {
        constexpr static char table[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

        char hash_str[16 + 1];
        size_t counter = 0;
        for (size_t i = 0; i < puzzle_size; i++) {
            for (size_t j = 0; j < puzzle_size; j++) {
                hash_str[counter++] = table[puzzle_state[i][j]];
            }
        }
        hash_str[16] = '\0';
        return hash_str;
    }
#else
    string getKey() override {
        string result;
        for (size_t i = 0; i < puzzle_size; i++) {
            for (size_t j = 0; j < puzzle_size; j++) {
                result += std::to_string(puzzle_state[i][j]) + " ";
            }
            result += "|";
        }
        return result;
    }
#endif

private:
    static int **convertFromVector(const vector<vector<int>> &vect_state) {
        size_t vectorSz = vect_state.size();
        int **tmp_state = new int*[vectorSz];

        for (size_t i = 0; i < vectorSz; i++) {

            if (vect_state[i].size() != vectorSz) {
                throw "Invalid vector state size!";
            }

            tmp_state[i] = new int[vectorSz];

            for (size_t j = 0; j < vectorSz; j++) {
                tmp_state[i][j] = vect_state[i][j];
            }
        }
        return tmp_state;
    }
};


struct Node {
    State *state;
    double f, g, h;
    unsigned long identifier;
    string action;
    Node *parent;

    //vector<Node*> children;

    Node(State *state, double g, double h, unsigned long identifier, const string &action, Node *parent = nullptr) :
        state(state),
        g(g),
        h(h),
        identifier(identifier),
        action(action),
        parent(parent)
    {
        f = g + h;
    }

    ~Node() {
        delete state;
    }

    /*
    void addChild(Node *child) {
        children.push_back(child);
    }

    bool hasChild(Node *child) {
        for (Node *curr_child: children) {
            if (curr_child == child) {
                return true;
            }
        }
        return false;
    }

    void clearChildren() {
        children.clear();
    }
    */

    friend bool operator > (const Node &left, const Node &right) {
        return left.f > right.f;
    }

    string toString() {
        return "N" + std::to_string(identifier);
    }

    operator string() {
        return this->toString();
    }

};

#endif