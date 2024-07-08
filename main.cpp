#include "Game.cpp"
#include <limits>
#include <iostream>
// #include "ResistanceDistance.cpp"
// #include "MiniMax.cpp"
#include <limits.h>
#include <cstdlib>
#include <ctime>
#include "src/include/Eigen/Dense"

// int getOneDIndex(int i, int j)
// {
//     std::vector<std::pair<int, int>> indexMap = {{0, 0},
//                                                  {0, 1},
//                                                  {1, 0},
//                                                  {1, 1}};
//     std::pair<int, int> searchPair = {i, j};
//     auto iterator = std::find(indexMap.begin(), indexMap.end(), searchPair);
//     if (iterator != indexMap.end())
//     {
//         int index = iterator - indexMap.begin();
//         return index;
//     }
//     else
//     {
//         return -1;
//     }
// }

int main(int argv, char **args)
{
    srand(static_cast<unsigned int>(time(0)));

    // For testing Evaluation function
    // std::vector<std::vector<int>> boardMatrix;
    // boardMatrix.resize(3, std::vector<int>(3, 0));

    // int boardSize = boardMatrix.size();
    // // boardMatrix[1][1] = 1;
    // ResistanceDistance *test = new ResistanceDistance();

    // boardMatrix[0][0] = 1;
    // float boardScore = test->evaluate(boardMatrix);
    // std::cout << boardScore << std::endl;

    // boardMatrix[0][1] = 2;
    // boardScore = test->evaluate(boardMatrix, 0, 1);
    // std::cout << boardScore << std::endl;

    // boardMatrix[1][0] = 2;
    // boardScore = test->evaluate(boardMatrix, 1, 0);
    // std::cout << boardScore << std::endl;

    // boardMatrix[1][1] = 2;
    // boardScore = test->evaluate(boardMatrix, 1, 1);
    // std::cout << boardScore << std::endl;

    // boardMatrix[1][2] = 2;
    // boardScore = test->evaluate(boardMatrix, 1, 2);
    // std::cout << boardScore << std::endl;

    // For testing MiniMax

    std::vector<std::vector<int>> board;
    board.resize(3, std::vector<int>(3, 0));
    // board[0][0] = 0;
    // board[0][1] = 0;
    // board[0][2] = 0;
    // board[1][0] = 0;
    // board[1][1] = 0;
    // board[1][2] = 0;
    // board[2][0] = 0;
    // board[2][1] = 0;
    // board[2][2] = 0;
    MiniMax *test = new MiniMax();
    test->go(board, true);

    // For testing entire game loop

    // Game *newGame = new Game(9);

    return 0;
}
