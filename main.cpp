#include "Game.cpp"
#include <limits>
#include <iostream>
// #include "ResistanceDistance.cpp"
// #include "MiniMax.cpp"
#include <limits.h>
#include <cstdlib>
#include <ctime>
#include "src/include/Eigen/Dense"

int main(int argv, char **args)
{
    srand(static_cast<unsigned int>(time(0)));

    // // For testing Evaluation function
    // std::vector<std::vector<int>> boardMatrix;
    // boardMatrix.resize(3, std::vector<int>(3, 0));

    // int boardSize = boardMatrix.size();
    // // boardMatrix[1][1] = 1;
    // ResistanceDistance *test = new ResistanceDistance();

    // boardMatrix[0][1] = 1;
    // boardMatrix[1][2] = 1;
    // boardMatrix[2][0] = 2;

    // auto score = test->evaluate(boardMatrix);
    // std::cout << score << std::endl;

    // For testing MiniMax

    // std::vector<std::vector<int>> board;
    // board.resize(3, std::vector<int>(3, 0));
    // // board[0][0] = 0;
    // // board[0][1] = 0;
    // // board[0][2] = 0;
    // // board[1][0] = 0;
    // // board[1][1] = 0;
    // // board[1][2] = 0;
    // // board[2][0] = 0;
    // // board[2][1] = 0;
    // // board[2][2] = 0;
    // MiniMax *test = new MiniMax();
    // test->go(board, true);

    // For testing entire game loop

    Game *newGame = new Game(7);

    return 0;
}
