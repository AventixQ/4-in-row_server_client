#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <iostream>
#include <string>

struct Player{
    std::string name;
    std::string pawn;
};

class ConnectFourGame {
public:
    ConnectFourGame();
    void displayBoard();
    bool makeMove(int column, Player& player);
    bool checkForWin(Player& player);
    bool isBoardFull();

private:
    //board 6x7
    static const int rows = 7;
    static const int cols = 7;
    std::string board[rows][cols];

    bool isValidMove(int column);
    bool checkForRow(int row, int col, Player& player);
    bool checkForColumn(int row, int col, Player& player);
    bool checkForDiagonal(int row, int col, Player& player);
};

#endif // GAME_LOGIC_H
