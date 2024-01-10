#include "client_game.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
//#include <cstdlib> //clear console


//start of the game
ConnectFourGame::ConnectFourGame() {
    for(int i = 0; i < cols; ++i){
        board[0][i] = std::to_string(i+1);  //char('0' + i + 1);
        //std::cout<<char('0' + i + 1);
    }
    for (int i = 1; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            board[i][j] = " ";
        }
    }
}

//for better visualisation of board
std::string printColumnEnd(int cols){
    std::string columnEnd = "";
    for(int i = 0; i < cols + 1; i++) columnEnd += "-----";
    columnEnd += "---";
    return columnEnd;
}

std::string ConnectFourGame::displayBoard() {
    std::string boardView = "";
    boardView += printColumnEnd(cols);
    for (int i = 0; i < rows; ++i) {
        boardView += "\n";
        boardView += "|";
        for (int j = 0; j < cols; ++j) {
            boardView += "  " + board[i][j] + "  |";
        }
        boardView += "\n";
        boardView += printColumnEnd(cols);
    }
    boardView += "\n";
    return boardView;
}

bool ConnectFourGame::isValidMove(int column) {
    // Check if column number is valid
    if (column < 0 || column >= cols) {
        std::cout << "Invalid column number\n";
        return false;
    }

    //Check if column is full
    if (board[1][column] != " ") {
        std::cout << "Column is full\n";
        return false;
    }

    //Valid move
    return true;
}

bool ConnectFourGame::makeMove(int column, Player& player) {
    if (!isValidMove(column)) {
        return false;
    }

    //Find first free space in column
    int row = rows - 1;
    while (row > 0 && board[row][column] != " ") {
        --row;
    }

    //Put player pawn
    board[row][column] = player.pawn;

    return true;
}

bool ConnectFourGame::checkForRow(int row, int col, Player& player) {
    int count = 0;

    //Check left
    for (int i = col; i >= 0; --i) {
        if (board[row][i] == player.pawn) {
            ++count;
        } else {
            break;
        }
    }

    //Check right
    for (int i = col + 1; i < cols; ++i) {
        if (board[row][i] == player.pawn) {
            ++count;
        } else {
            break;
        }
    }

    return count >= 4; // If 4 in row -> true
}

bool ConnectFourGame::checkForColumn(int row, int col, Player& player) {
    int count = 0;

    //Check down
    for (int i = row; i < rows; ++i) {
        if (board[i][col] == player.pawn) {
            ++count;
        } else {
            break;
        }
    }

    return count >= 4; // If 4 in column -> true
}

bool ConnectFourGame::checkForDiagonal(int row, int col, Player& player) {
    int countLeft = 0; // For left diagonal
    int countRight = 0; // For right diagonal

    //Check left and up
    for (int i = row, j = col; i >= 0 && j >= 0; --i, --j) {
        if (board[i][j] == player.pawn) {
            ++countLeft;
        } else {
            break;
        }
    }

    //Check right and down
    for (int i = row + 1, j = col + 1; i < rows && j < cols; ++i, ++j) {
        if (board[i][j] == player.pawn) {
            ++countRight;
        } else {
            break;
        }
    }

    return countLeft + countRight >= 4; // If 4 in column -> true
}

bool ConnectFourGame::checkForWin(Player& player) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (board[i][j] == player.pawn) {
                if (checkForRow(i, j, player) || checkForColumn(i, j, player) || checkForDiagonal(i, j, player)) {
                    return true;
                }
            }
        }
    }

    return false; //No one wins
}

bool ConnectFourGame::isBoardFull() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if(board[i][j] == " ") return false;
        }
    }
    return true;
}

void displayConnectionMessage(Player &player) {
    std::cout<<"You are " + player.name + ". Your pawn is: " + player.pawn + "\n"
        + "Waiting for second player...\n";
}

bool yourTurn(ConnectFourGame& game, Player &player, int col, bool who) {
    if (game.makeMove(col - 1, player))
    {
        std::cout << game.displayBoard();
        if (game.checkForWin(player))
        {
            
            std::cout << std::endl;
            if(who) std::cout <<"Congratulations! You won!\n";
            else std::cout <<"Your opponent won this game. Better luck next time!\n";
            return false; //end of game
        }

        if (game.isBoardFull())
        {
            //std::cout << game.displayBoard();
            std::cout << std::endl;
            std::cout << "It's a draw!\n";
            return false; //end of game
        }
    }
    return true;
}

void connectToServer(ConnectFourGame& game) {
    Player player;
    Player oponent;
    
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Error in connection with socket\n";
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345); // To change
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error in connecting with server \n";
        close(clientSocket);
        return;
    } else{
        std::cout<<"Welcome to the game!"<<std::endl;
    
    //connect and get from server number and pawn
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    //std::cout<<buffer<<std::endl;
    if (buffer[0] == '0' || buffer[0] == '1') {
        int playerNumber = buffer[0] - '0' + 1;
        player.name = "Player " + std::to_string(playerNumber);
        player.pawn = (playerNumber == 1) ? "X" : "O";
        oponent.name = "Player " + (playerNumber == 1) ? "0" : "1";
        oponent.pawn = (playerNumber == 1) ? "O" : "X";

        displayConnectionMessage(player);

        //Play a game
        std::string message;
        bool RUNNING = true;
        while (RUNNING) {
            memset(buffer, 0, sizeof(buffer));
            recv(clientSocket, buffer, sizeof(buffer), 0);
            //std::cout<<buffer<<std::endl; //OK
            if (strncmp(buffer, "WAIT", 4) == 0) {
                std::cout << "Waiting for second player's move...\n";
            } else if (strncmp(buffer, "BOARD", 5) == 0) {
                std::string boardData(buffer, sizeof(buffer));
                int opponentCol = boardData[5] - '0'; //opponent column
                if(opponentCol != 9){ //Opponent made his move
                    std::cout<<"Opponent move: "<<opponentCol<<std::endl;
                    RUNNING = yourTurn(game, oponent, opponentCol, false);
                    if(!RUNNING){
                        strcpy(buffer, "END");
                        send(clientSocket, buffer, sizeof(buffer), 0);
                        continue;
                    }
                } else std::cout << game.displayBoard();

                int column;
                std::cout << "Your turn. Enter column number (1-7): ";
                std::cin >> column;

                std::string message = "BOARD" + std::to_string(column);
                send(clientSocket, message.c_str(), message.size(), 0);

                RUNNING = yourTurn(game, player, column, true);
                if(!RUNNING){
                    strcpy(buffer, "END");
                    send(clientSocket, buffer, sizeof(buffer), 0);
                }
            }
        }
    }
    close(clientSocket);}
}

int main() {
    ConnectFourGame game;
    connectToServer(game);

    return 0;
}