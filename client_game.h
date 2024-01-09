#ifndef client_game_H
#define client_game_H

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct Player{
    std::string name;
    std::string pawn;
};

class ConnectFourGame {
public:
    ConnectFourGame();
    std::string displayBoard();
    bool makeMove(int column, Player& player);
    bool checkForWin(Player& player);
    bool isBoardFull();

private:
    //board 6x7
    static const int rows = 7;
    static const int cols = 7;
    std::string board[rows][cols];

    
    void waitForGameStart(int clientSocket);
    bool isValidMove(int column);
    bool checkForRow(int row, int col, Player& player);
    bool checkForColumn(int row, int col, Player& player);
    bool checkForDiagonal(int row, int col, Player& player);
};

#endif // GAME_LOGIC_H
