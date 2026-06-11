#include <stdio.h>
#include <iostream> 
#include <vector>
using namespace std;

struct MoveResult {
    bool validMove;
    bool win;
    bool draw;
    int reward;
};

class Connect4 {
public:
    void reset();
    MoveResult step(int x, int y);
    char checkWinner();
    bool isTerminal() const;
    int currentPlayer() const;
    vector<float> encodeState() const;
    vector<int> legalMoves() const;
    
private:
    char grid[4][4];
    int played_history_x[4][2];
    int played_history_o[4][2];
    int played_count_x = 0;
    int played_count_o = 0;
};

vector<float> Connect4::encodeState() {
    vector<float> state;
    
    // X = 1, O = -1, empty = 0

    // newest = 1.0, oldest = 0.25, empty = 0

    return state;
}

MoveResult Connect4::step(int x, int y) {

    MoveResult moveResult;
    char marker;

    int row = 3 - y;
    int col = x;
  
    if (played_count_x % 2) {
        grid[row][col] = 'X';
        
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
        
        grid[row][col] = 'O';

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

    return moveResult;

}

void printBoard() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
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

void reset() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            grid[i][j] = '-';
        }

        for (int j = 0; j < 2; j++) {
            played_history_x[i][j] = 0;
            played_history_o[i][j] = 0;
        }
    }
    
    played_count_x = 0;
    played_count_o = 0;

}

void runTest(const char testName[], bool expected) {
    bool result = checkWinner();

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

    Connect4 game;

    game.reset();

    return 0;
}
