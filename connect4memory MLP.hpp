#pragma once
#include <vector>
using std::vector;


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
    vector<float> encodeState();
    char currentPlayer();
    vector<int> legalMoves();
    int isGameOver();
    MoveResult stepAction(int action);
    
private:
    char grid[4][4];
    int played_history_x[4][2];
    int played_history_o[4][2];
    int turnNumber = 0;
    int played_count_x = 0;
    int played_count_o = 0;
    char marker;
    int isTerminal = 0;
    const int maxTurns = 100;
};