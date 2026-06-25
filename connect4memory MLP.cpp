#include <stdio.h>
#include <iostream> 
#include <vector>
using namespace std;

#include "connect4memory MLP.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(connect4memory_MLP, m) {
    py::class_<MoveResult>(m, "MoveResult")
    .def_readonly("validMove", &MoveResult::validMove)
    .def_readonly("win", &MoveResult::win)
    .def_readonly("draw", &MoveResult::draw)
    .def_readonly("reward", &MoveResult::reward);

    py::class_<Connect4>(m, "Connect4")
    .def(py::init<>())
    .def("reset", &Connect4::reset)
    .def("step", &Connect4::step)
    .def("checkWinner", &Connect4::checkWinner)
    .def("encodeState", &Connect4::encodeState)
    .def("currentPlayer", &Connect4::currentPlayer)
    .def("legalMoves", &Connect4::legalMoves)
    .def("isGameOver", &Connect4::isGameOver)
    .def("stepAction", &Connect4::stepAction)
    .def("getTurnNumber", &Connect4::getTurnNumber)
    .def("getGrid", &Connect4::getGrid);

}


MoveResult Connect4::stepAction(int action) {

    int row = 3 - (action / 4);
    int col = action % 4;

    return step(col, row);
}

vector<int> Connect4::legalMoves() {

    vector<int> moves;
    if (isTerminal) {
        return moves;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j] == '-') {
                //the grid coordinate is encoded as a single number
                moves.push_back(i*4 + j);
            }
        }
    }
    return moves;
}

char Connect4::currentPlayer() {
    return (turnNumber % 2 == 0) ? 'X':'O';
}

int Connect4::isGameOver() {
    if (checkWinner() != '-' || (turnNumber >= maxTurns && checkWinner() == '-'))
    {
        return 1;
    }
    else {
        return 0;
    }
}

vector<char> Connect4::getGrid() {
    vector<char> gridvector;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j ++) {
            gridvector.push_back(grid[i][j]);
        }
    }

    return gridvector;
}

int Connect4::getTurnNumber() {
    return turnNumber;
}

vector<float> Connect4::encodeState() {
    vector<float> state;
    char currentMarker = (turnNumber % 2 == 0) ? 'X':'O';
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j ++) {

            float value = 0.0f;
            if (grid[i][j] == 'X' && currentMarker == 'X') {
                //check which order it was played in
                for (int ip = 0; ip < 4; ip++) {
                    if (played_history_x[ip][0] == i && played_history_x[ip][1] == j) {
                        //state.push_back(0.25f + 0.25f*ip);
                        value = 0.25f + 0.25f*ip;
                    }
                }
            }
            else if (grid[i][j] == 'X' && currentMarker != 'X') {
                for (int ip = 0; ip < 4; ip++) {
                    if (played_history_x[ip][0] == i && played_history_x[ip][1] == j) {
                        //state.push_back(0.25f + 0.25f*ip);
                        value = -(0.25f + 0.25f*ip);
                    }
                }
            }

            else if (grid[i][j] == 'O' && currentMarker == 'O') {
                //check which order it was played in
                for (int ip = 0; ip < 4; ip++) {
                    if (played_history_o[ip][0] == i && played_history_o[ip][1] == j) {
                        //state.push_back(-(0.25f + 0.25f*ip));
                        value = 0.25f + 0.25f*ip;
                    }
                }
            }

            else if (grid[i][j] == 'O' && currentMarker != 'O') {
                //check which order it was played in
                for (int ip = 0; ip < 4; ip++) {
                    if (played_history_o[ip][0] == i && played_history_o[ip][1] == j) {
                        //state.push_back(-(0.25f + 0.25f*ip));
                        value = -(0.25f + 0.25f*ip);
                    }
                }
            }

            state.push_back(value);
            
        }
    }
    
    // X = 1, O = -1, empty = 0

    // newest = 1.0, oldest = 0.25, empty = 0

    return state;
}

MoveResult Connect4::step(int x, int y) {

    if (checkWinner() != '-') {
        isTerminal = 1;
        return{false, true, false, -1};
    }
    
    else if (turnNumber >= maxTurns && checkWinner() == '-') 
    {
        isTerminal = 1;
        return {false, false, true, -1};
    }

    MoveResult moveResult;
    
    marker = (turnNumber % 2 == 0) ? 'X':'O';
    
    int row = 3 - y;
    int col = x;
  
    if (row < 0 || row >= 4 || col < 0 || col >= 4 || grid[row][col]!='-') {
        //moveResult.validMove = false;
        return {false, false, false, -1};
    }

    grid[row][col] = marker;


    if (marker == 'X') {
        
        if (played_count_x >= 4) {
            //make first marker disappear
            grid[played_history_x[0][0]][played_history_x[0][1]] = '-';
        }
        //shift the array and add the latest coordinate
        for (int i = 0; i < 3; i++) {
            played_history_x[i][0] = played_history_x[i+1][0];
            played_history_x[i][1] = played_history_x[i+1][1];
        }
        played_history_x[3][0] = 3 - y;
        played_history_x[3][1] = x;
        
        played_count_x++;

    }

    else {
        
        if (played_count_o >= 4) {
            grid[played_history_o[0][0]][played_history_o[0][1]] = '-';
        }

        for (int i = 0; i < 3; i++) {
            played_history_o[i][0] = played_history_o[i+1][0];
            played_history_o[i][1] = played_history_o[i+1][1];
        }
        played_history_o[3][0] = 3 - y;
        played_history_o[3][1] = x;
        
        played_count_o++;

    }
    moveResult.validMove = true;
    turnNumber++;
    moveResult.win = checkWinner() != '-';
    bool draw = turnNumber >= maxTurns && checkWinner() == '-';
    if (moveResult.win || draw) {
        isTerminal = 1;
    }
    moveResult.draw = draw;
    moveResult.reward = moveResult.win ? 1 : 0;

    return moveResult;

}

char Connect4::checkWinner() {
    //check row
    
    for(int i = 0; i < 4; i++) {

        if (grid[i][0] == '-') {
            continue;
        }

        bool winFlag = true;

        for(int j = 0; j < 4; j++) {
            if (grid[i][j] != grid[i][0]) {
                winFlag = false;
                break;
            }
        }

        if (winFlag) {
            return grid[i][0];
        }
    }

    //check cols
    for(int j = 0; j < 4; j++) {

        bool winFlag = true;
        
        if (grid[0][j] == '-') {
            continue;
        }
        for(int i = 0; i < 4; i++) {
            if (grid[i][j] != grid[0][j]) {
                winFlag = false;
                break;
            }
        }

        if (winFlag) {
            return grid[0][j];
        }
    }

    
    //check left to right diagonal
    bool winFlag = true;
    if (grid[0][0] == '-') {
        winFlag = false;
    }
    else {
        for(int i = 0; i < 4; i++) {

            if (grid[i][i] != grid[0][0]) {
                winFlag = false;
                break;
            }
        }
    }
    if (winFlag) {
        return grid[0][0];
    }

    //check right to left diagonal
    winFlag = true;
    if (grid[0][3] == '-') {
        winFlag = false;
    }
    else {
        for(int i = 0; i < 4; i++) {

            if (grid[i][3-i] != grid[0][3]) {
                winFlag = false;
                break;
            }
        }
    }
        
    if (winFlag) {
        return grid[0][3];
    }
    
    return '-';
}

//Functions to help testing

void Connect4::reset() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            grid[i][j] = '-';
        }

        for (int j = 0; j < 2; j++) {
            played_history_x[i][j] = -1;
            played_history_o[i][j] = -1;
        }
    }
    isTerminal = 0;
    turnNumber = 0;
    played_count_x = 0;
    played_count_o = 0;

}

int main() {

    Connect4 game;

    game.reset();

    return 0;
}
