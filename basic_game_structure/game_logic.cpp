#include "game_logic.h"
#include <iostream>
#include <string>
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
void printColumnEnd(int cols){
    for(int i = 0; i < cols+1; i++) std::cout<<"-----";
    std::cout<<"---";
}

void ConnectFourGame::displayBoard() {
    printColumnEnd(cols);
    for (int i = 0; i < rows; ++i) {
        std::cout<<std::endl;
        std::cout<<"|";
        for (int j = 0; j < cols; ++j) {
            std::cout<<"  "<<board[i][j]<<"  |";
        }
        std::cout<<std::endl;
        printColumnEnd(cols);
    }
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

    // Find first free space in column
    int row = rows - 1;
    while (row > 0 && board[row][column] != " ") {
        --row;
    }

    // Put player pawn
    board[row][column] = player.pawn;

    return true;
}

bool ConnectFourGame::checkForRow(int row, int col, Player& player) {
    int count = 0;

    // Check left
    for (int i = col; i >= 0; --i) {
        if (board[row][i] == player.pawn) {
            ++count;
        } else {
            break;
        }
    }

    // Check right
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

    // Check down
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

    // Check left
    for (int i = row, j = col; i >= 0 && j >= 0; --i, --j) {
        if (board[i][j] == player.pawn) {
            ++countLeft;
        } else {
            break;
        }
    }

    // Check right
    for (int i = row, j = col; i < rows && j < cols; --i, ++j) {
        if (board[i][j] == player.pawn) {
            ++countRight;
        } else {
            break;
        }
    }

    return countLeft >= 4 || countRight >= 4; // If 4 in column -> true
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

    return false; // No one wins
}

bool ConnectFourGame::isBoardFull() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if(board[i][j] == " ") return false;
        }
    }
    return true;
}

int main() {
    ConnectFourGame game;

    Player player1;
    player1.name = "Player 1";
    player1.pawn = "X";

    Player player2;
    player2.name = "Player 2";
    player2.pawn = "O";

    std::cout << "Welcome to the game!\n";
    std::cout << player1.name << ": " << player1.pawn << "\n";
    std::cout << player2.name << ": " << player2.pawn << "\n";
    std::cout << "Let's start the game!\n";

    //game.displayBoard();

    int whoIsPlaying = 1; //Player 1 starts
    Player currentPlayer[2] = {player1, player2};

    while (true) {
        game.displayBoard();
        std::cout<<std::endl;
        Player player = currentPlayer[whoIsPlaying - 1];
        // Player is choosing column
        int column;
        std::cout << player.name << ", it's your turn. Enter column number (1-7): "<<std::endl;;
        std::cin >> column;

        // Player move checking -> if he is able to move, check for wins/board is full
        if (game.makeMove(column - 1, player)) {
            if (game.checkForWin(player)) {
                game.displayBoard();
                std::cout<<std::endl;
                std::cout << player.name << " wins!\n";
                break;
            }

            if (game.isBoardFull()) {
                game.displayBoard();
                std::cout<<std::endl;
                std::cout << "It's a draw!\n";
                break;
            }

            // Change player
            if(whoIsPlaying == 1) whoIsPlaying = 2;
            else whoIsPlaying = 1;
        }
    }

    return 0;
}