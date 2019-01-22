/*Search Algorithms*/

#ifndef SEARCH_H
#define SEARCH_H

#include "DataStructures.h"
#include "Puzzle.h"

#include <string>
#include <sstream>
#include <vector>
#include <iomanip> //used for setw
#include <stack>
#include <atomic>
#include <algorithm>
#include <math.h>

using std::string;
using std::vector;
using std::unordered_map;
using std::stack;

size_t maxQueueSz = 0;
std::atomic_bool solution_found(false);

//Iterative Deepening A/A*
vector<string> IDA(State *init_state) {
    unsigned long id_counter = 0;
    vector<string> solution;
    
    stack<Node*> stack; //frontier

    double initial_heuristic = heuristic(init_state);
    Node *initialNode = new Node(init_state, 0, initial_heuristic, id_counter++, "start");

    stack.push(initialNode);

    double limit = initial_heuristic;
    double min_exceeded = INFINITY;

    while (1) {
        if (stack.empty()) {
            //next iteration with updated limit
            limit = min_exceeded;
            min_exceeded = INFINITY;
            stack.push(initialNode);
            printf("Limit: %f\n", limit);
            continue;
        }

        Node *currNode = stack.top();
        stack.pop();

        if (solution_found) {
            return solution;
        }

        //limit exceeded, discard this search path
        double current_f = currNode->f;
        if (current_f > limit) {
            if (current_f < min_exceeded) {  //only allow increasing limit
                min_exceeded = current_f;
            }

            /// <CLEANUP> Remove useless path
            Node *targetNode;

            if (stack.empty()) {
                targetNode = initialNode;
            } 
            else {
                Node *top_node = stack.top();
                Node *branchable_node = top_node->parent; // the closest ancestor that has more than one child
                targetNode = branchable_node;
            }

            Node *theNode = currNode;

            while (theNode != targetNode) { // delete nodes until targetNode is reached
                Node *tmp = theNode->parent;
                delete theNode;
                theNode = tmp;
            }

            continue;
        }

        State *currState = currNode->state;
        double g = currNode->g;

        if (isGoalState(currState)) { //found solution
            printf("\nf value of %.2f at Node %ld, found within: %.2f\n", currNode->f, currNode->identifier, limit);

            solution_found = true;
            std::stringstream ss;
            Node *theNode = currNode;
            //string initialState_key = init_state->getKey();

            //while (theNode->puzzle_state.getKey() != initialState_key) {
            while (theNode != initialNode) {
                ss << std::setw(6) << std::left << theNode->action
                    << std::setw(4) << std::left << theNode->g
                    << std::setw(5) << std::left << theNode->h
                    << std::setw(5) << std::left << theNode->f
                    << theNode->toString();
                solution.push_back(ss.str());
                Node *parent = theNode->parent;
                
                /// <CLEANUP> delete the nodes along the solution path
                delete theNode;
                
                theNode = parent;
                ss.str(""); //clear the string stream
            }

            /// <CLEANUP> delete initialNode (as part of the solution path)
            delete initialNode;

            /// <CLEANUP> delete rest of the nodes in the stack
            while (!stack.empty()) {
                Node *nodeToDel = stack.top();
                stack.pop();
                delete nodeToDel;
            }

            ss << std::setw(6) << std::left << "Move"
                << std::setw(4) << std::left << "g"
                << std::setw(5) << std::left << "h"
                << std::setw(5) << std::left << "f"
                << "Node ID";
            solution.push_back(ss.str());

            //reverse solution
            std::reverse(solution.begin(), solution.end());
            return solution;
        }

        vector<string> actions = getNextActions(currState);
        for (const string &action : actions) {
            State *childState = operate(currState, action);
            
            double child_g = g + getCost(currState, action);
            double child_h = heuristic(childState);

            Node *childNode = new Node(childState, child_g, child_h, id_counter++, action, currNode);

            stack.push(childNode);
        }
    }

    //return empty solution if no solution found //impossible!
    return solution;
}

vector<string> A(State *init_state) {
    unsigned long id_counter = 0;
    vector<string> solution;
    unordered_map<string, Node*> visited; //explored

    min_heap<Node*> queue; //frontier

    double initial_heuristic = heuristic(init_state);
    Node *initialNode = new Node(init_state, 0, initial_heuristic, id_counter++, "start");
    queue.push(initialNode);

    while (!queue.empty()) {

        //record max queue size
        if (queue.size() > maxQueueSz) maxQueueSz = queue.size();

        Node *currNode = queue.top();
        queue.pop();

        State *currState = currNode->state;
        double g = currNode->g;

        string currState_key = currState->getKey();

        if (visited.find(currState_key) == visited.end()) {
            visited[currState_key] = currNode;
        }

        if (isGoalState(currState)) { //found solution
            std::stringstream ss;
            Node *theNode = currNode;
            string initialState_key = init_state->getKey();

            //while (theNode->puzzle_state.getKey() != initialState_key) {
            while (theNode != initialNode) {
                ss << std::setw(6) << std::left << theNode->action
                    << std::setw(4) << std::left << theNode->g
                    << std::setw(5) << std::left << theNode->h
                    << std::setw(5) << std::left << theNode->f
                    << theNode->toString();
                solution.push_back(ss.str());
                theNode = theNode->parent;
                ss.str(""); //clear the string stream
            }
            ss << std::setw(6) << std::left << "Move"
                << std::setw(4) << std::left << "g"
                << std::setw(5) << std::left << "h"
                << std::setw(5) << std::left << "f"
                << "Node ID";
            solution.push_back(ss.str());

            //reverse solution
            std::reverse(solution.begin(), solution.end());
            printf("max queue size: %ld\n", maxQueueSz);

            /// <CLEANUP>
            while (!queue.empty()) {
                Node *nodeToDel = queue.top();
                queue.pop();
                delete nodeToDel;
            }

            for (auto item : visited) {
                Node *node = item.second;
                delete node;
            }
            visited.clear();
            return solution;
        }

        vector<string> actions = getNextActions(currState);
        for (const string &action : actions) {
            State *childState = operate(currState, action);
            string childState_key = childState->getKey();

            //childState_key not in visited
            if (visited.find(childState_key) == visited.end()) {
                double child_g = g + getCost(currState, action);
                double child_h = heuristic(childState);

                Node *childNode = new Node(childState, child_g, child_h, id_counter++, action, currNode);

                queue.push(childNode);
            }
        }
    }

    //return empty solution if no solution found
    return solution;
}

#endif