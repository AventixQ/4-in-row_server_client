#ifndef server_game_H
#define server_game_H

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>

struct Player{
    std::string name;
    std::string pawn;
    int clientSocket;
    bool isOnMove;
    int number;
};

class ConnectFourGame {
public:
    ConnectFourGame();
    std::string displayBoard();
    bool makeMove(int column, Player& player);

private:
    //board 6x7
    static const int rows = 7;
    static const int cols = 7;
    std::string board[rows][cols];

    
    void waitForGameStart(int clientSocket);
    bool isValidMove(int column);
};

#endif // GAME_LOGIC_H
