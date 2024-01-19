#include "server_game.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>

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

std::string ConnectFourGame::displayBoard() { //display current state of the board
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

//SERVER VALIDATION
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

void handleClient(Player& player1, Player& player2, ConnectFourGame& game) {
    char buffer[1024];

    std::cout<<"Sending player numbers to clients..."<<std::endl;
    //Inform player about number
    send(player1.clientSocket, std::to_string(player1.number).c_str(), sizeof(std::to_string(player1.number)), 0);
    send(player2.clientSocket, std::to_string(player2.number).c_str(), sizeof(std::to_string(player2.number)), 0);

    std::cout<<"Starting game.."<<std::endl;
    player1.isOnMove = false;
    player2.isOnMove = true;

    //Player 1 waiting for first move from player 2
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "WAIT");
    send(player1.clientSocket, buffer, sizeof(buffer), 0);

    //Player 2 first move
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "BOARD9");
    send(player2.clientSocket, buffer, sizeof(buffer), 0);

    Player player[2];
    while (true) {
        std::string message;
        memset(buffer, 0, sizeof(buffer));

        if(player1.isOnMove){ //check which player has his turn and put into player[0]
            player[0] = player1;
            player[1] = player2;
        }
        else{
            player[1] = player1;
            player[0] = player2;
        }

        recv(player[0].clientSocket, buffer, sizeof(buffer), 0);
        if (strncmp(buffer, "END", 3) == 0)
            break; // if second player ended game (won or had a tie), break loop
        int move;
        try{move = buffer[5] - '0';}
        catch(const std::exception& e){move = 9;}
        while (!game.makeMove(move - 1, player[0]))
        { // if not able to move, send error to player
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "ERROR");
            send(player[0].clientSocket, buffer, sizeof(buffer), 0);

            memset(buffer, 0, sizeof(buffer));
            recv(player[0].clientSocket, buffer, sizeof(buffer), 0);
            move = buffer[5] - '0';
        }
        message = "BOARD" + std::to_string(move);
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "PLAYED");
        send(player[0].clientSocket, buffer, sizeof(buffer), 0);          // send info to player, that his move was correct
        send(player[1].clientSocket, message.c_str(), message.size(), 0); // send move to opponent
        std::cout << "Move of the player " << player[0].server_number << " " << message << std::endl;
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "WAIT");
        send(player[0].clientSocket, buffer, sizeof(buffer), 0); // send to player waiting info

        player1.isOnMove = !player1.isOnMove; // change player
        player2.isOnMove = !player2.isOnMove;
    }
}

int main() {
    Player player[200];
    ConnectFourGame game[100];
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error in connection with socket\n";
        return -1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error in binding\n";
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 2) == -1) {
        std::cerr << "Error in listening\n";
        close(serverSocket);
        return -1;
    }

    std::vector<std::thread> gameThreads;

    int playerNumber = 0;
    int playerNumbToSend = 0;

    while (true) {
        //Accept first player and send number
        player[playerNumber].clientSocket = accept(serverSocket, nullptr, nullptr);
        std::cout<<"Player "<<playerNumber<<" connected"<<std::endl;
        player[playerNumber].number = playerNumbToSend;
        send(player[playerNumber].clientSocket, std::to_string(player[playerNumber].number).c_str(), sizeof(std::to_string(player[playerNumber].number)), 0);

        //Accept 2nd player and send number
        ++playerNumber;
        ++playerNumbToSend;
        player[playerNumber].clientSocket = accept(serverSocket, nullptr, nullptr);
        player[playerNumber].number = playerNumbToSend;
        player[playerNumber].server_number = playerNumber;
        std::cout<<"Player "<<playerNumber<<" connected"<<std::endl;
        send(player[playerNumber].clientSocket, std::to_string(player[playerNumber].number).c_str(), sizeof(std::to_string(player[playerNumber].number)), 0);

        //Seperate thread for clients
        gameThreads.emplace_back(handleClient, std::ref(player[playerNumber-1]), std::ref(player[playerNumber]), std::ref(game[(playerNumber-1)/2]));
        
        ++playerNumber;
        playerNumbToSend = 0;
    }

    for (auto& thread : gameThreads) {
        thread.join();
    }

    close(serverSocket);
    return 0;
}
