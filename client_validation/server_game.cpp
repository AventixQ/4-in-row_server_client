#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>

struct Player {
    int number;
    int clientSocket;
    bool isOnMove;
};

void handleClient(Player& player1, Player& player2) {
    char buffer[1024];

    std::cout<<"Sending player numbers to clients..."<<std::endl;
    //Inform player about number
    send(player1.clientSocket, std::to_string(player1.number).c_str(), sizeof(std::to_string(player1.number)), 0);
    send(player2.clientSocket, std::to_string(player2.number).c_str(), sizeof(std::to_string(player2.number)), 0);

    std::cout<<"Starting game..."<<std::endl;
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

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        if(player1.isOnMove){
            recv(player1.clientSocket, buffer, sizeof(buffer), 0);
            if(strncmp(buffer, "END", 3) == 0) break;
            send(player2.clientSocket, buffer, sizeof(buffer), 0);
            std::cout<<"Move of the player "<< player1.number << " " <<buffer<<std::endl;
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "WAIT");
            send(player1.clientSocket, buffer, sizeof(buffer), 0);

            player1.isOnMove = false;
            player2.isOnMove = true;
        } else if(player2.isOnMove){
            recv(player2.clientSocket, buffer, sizeof(buffer), 0);
            if(strncmp(buffer, "END", 3) == 0) break;
            send(player1.clientSocket, buffer, sizeof(buffer), 0);
            std::cout<<"Move of the player "<< player2.number << " " <<buffer<<std::endl;
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "WAIT");
            send(player2.clientSocket, buffer, sizeof(buffer), 0);

            player2.isOnMove = false;
            player1.isOnMove = true;
        }
    }
}

int main() {
    Player player[20];
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
        //Player player[playerNumber], player[playerNumber + 1];

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
        std::cout<<"Player "<<playerNumber<<" connected"<<std::endl;
        send(player[playerNumber].clientSocket, std::to_string(player[playerNumber].number).c_str(), sizeof(std::to_string(player[playerNumber].number)), 0);

        //Seperate thread for clients
        gameThreads.emplace_back(handleClient, std::ref(player[playerNumber-1]), std::ref(player[playerNumber]));
        
        ++playerNumber;
        playerNumbToSend = 0;
    }

    for (auto& thread : gameThreads) {
        thread.join();
    }

    close(serverSocket);
    return 0;
}
