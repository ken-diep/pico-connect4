#include <stdio.h>
#include <iostream>
using namespace std;

char grid[4][4];
int played_history_x[4][2];
int played_history_o[4][2];
int round_number = 0;

void updateBoard(int x, int y, char marker) {
    
    int row = 3 - y;
    int col = x;

    grid[row][col] = marker;
  
    if (marker == 'X') {
        
        if (round_number/2 >= 4) {
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
        
    }

    else if (marker == 'O') {
        
        if (round_number/2 >= 4) {
            grid[played_history_o[0][0]][played_history_o[0][1]] = '-';
        }

        for (int i = 0; i < 3; i++) {
            played_history_o[i][0] = played_history_o[i+1][0];
            played_history_o[i][1] = played_history_o[i+1][1];
        }
        played_history_o[3][0] = 3 - y;
        played_history_o[3][1] = x;
        
    }
    round_number++;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

}

bool checkWin() {
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
            //cout << "row win";
            return true;
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
            //cout << "col win";
            return true;
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
        return true;
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
        cout << "RL win";
        return true;
    }
    
    return false;
}

//Functions to help testing

void resetGrid() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            grid[i][j] = '-';
        }

        for (int j = 0; j < 2; j++) {
            played_history_x[i][j] = 0;
            played_history_o[i][j] = 0;
        }
    }
    
    round_number = 0;
}

void runTest(const char testName[], bool expected) {
    bool result = checkWin();

    cout << testName << ": ";

    if (result == expected) {
        cout << "PASS";
    }
    else {
        cout << "FAIL";
    }

    cout << " expected " << expected << ", got " << result << endl;
}

int countMarker(char marker) {
    int count = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j] == marker) {
                count++;
            }
        }
    }

    return count;
}

void runCellTest(const char testName[], int row, int col, char expected) {
    char result = grid[row][col];

    cout << testName << ": ";

    if (result == expected) {
        cout << "PASS";
    }
    else {
        cout << "FAIL";
    }

    cout << " expected " << expected << ", got " << result << endl;
}

void runIntTest(const char testName[], int expected, int result) {
    cout << testName << ": ";

    if (result == expected) {
        cout << "PASS";
    }
    else {
        cout << "FAIL";
    }

    cout << " expected " << expected << ", got " << result << endl;
}

int main() {
    // updateBoard should translate x/y game coordinates into grid row/col
    resetGrid();
    updateBoard(0, 0, 'X');
    runCellTest("X placed bottom left", 3, 0, 'X');
    runCellTest("top left still empty", 0, 0, '-');

    resetGrid();
    updateBoard(3, 3, 'O');
    runCellTest("O placed top right", 0, 3, 'O');
    runCellTest("bottom right still empty", 3, 3, '-');

    // Four markers per player should stay on the board
    resetGrid();
    updateBoard(0, 0, 'X');
    updateBoard(1, 0, 'O');
    updateBoard(1, 1, 'X');
    updateBoard(2, 0, 'O');
    updateBoard(2, 2, 'X');
    updateBoard(3, 0, 'O');
    updateBoard(3, 3, 'X');
    updateBoard(0, 1, 'O');

    runIntTest("four X markers before fifth X", 4, countMarker('X'));
    runIntTest("four O markers before fifth O", 4, countMarker('O'));

    // Fifth X should remove oldest X, not oldest O
    updateBoard(0, 2, 'X');
    runCellTest("oldest X removed after fifth X", 3, 0, '-');
    runCellTest("newest X added after fifth X", 1, 0, 'X');
    runIntTest("still only four X markers", 4, countMarker('X'));
    runIntTest("O markers unaffected by fifth X", 4, countMarker('O'));

    // Fifth O should remove oldest O
    updateBoard(1, 2, 'O');
    runCellTest("oldest O removed after fifth O", 3, 1, '-');
    runCellTest("newest O added after fifth O", 1, 1, 'O');
    runIntTest("still only four O markers", 4, countMarker('O'));

    // Disappearing marker should be able to break a previous win
    resetGrid();
    updateBoard(0, 0, 'X');
    updateBoard(0, 1, 'O');
    updateBoard(1, 0, 'X');
    updateBoard(1, 1, 'O');
    updateBoard(2, 0, 'X');
    updateBoard(2, 1, 'O');
    updateBoard(3, 0, 'X');

    runTest("four X in bottom row is a win", true);

    resetGrid();

    updateBoard(0, 0, 'X');
    updateBoard(0, 1, 'O');
    updateBoard(1, 0, 'X');
    updateBoard(1, 2, 'O');
    updateBoard(2, 0, 'X');
    updateBoard(2, 1, 'O');
    updateBoard(3, 3, 'X');
    updateBoard(3, 1, 'O');

    updateBoard(0, 2, 'X');

    runCellTest("oldest X removed after fifth X", 3, 0, '-');
    runCellTest("newest X added after fifth X", 1, 0, 'X');
    runIntTest("still only four X markers", 4, countMarker('X'));
    runTest("no accidental win after X memory update", false);

    return 0;
}
