#include "Game.cpp"
#include <limits>
#include <iostream>
// #include "ResistanceDistance.cpp"
// #include "MiniMax.cpp"
#include <limits.h>
#include <cstdlib>
#include <ctime>
#include "src/include/Eigen/Dense"

HMENU CreateMainMenu()
{
    HMENU hMenu = CreateMenu();

    // Create pop up menu for board size options
    HMENU boardSizeMenu = CreatePopupMenu();

    // Create options for board size menu
    AppendMenu(boardSizeMenu, MF_STRING, 1001, "5x5");
    AppendMenu(boardSizeMenu, MF_STRING, 1002, "6x6");
    AppendMenu(boardSizeMenu, MF_STRING, 1003, "7x7");
    AppendMenu(boardSizeMenu, MF_STRING, 1004, "8x8");
    AppendMenu(boardSizeMenu, MF_STRING, 1005, "9x9");
    AppendMenu(boardSizeMenu, MF_STRING, 1006, "10x10");
    AppendMenu(boardSizeMenu, MF_STRING, 1007, "11x11");

    // Append the board size menu to the main menu bar
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)boardSizeMenu, "Board Size");

    // Create pop up menu for player one's settings
    HMENU playerOneOptions = CreatePopupMenu();

    // Create the options
    AppendMenu(playerOneOptions, MF_STRING, 1008, "Human");
    AppendMenu(playerOneOptions, MF_STRING, 1009, "MiniMax");
    AppendMenu(playerOneOptions, MF_STRING, 1010, "MCTS");

    // Add the the menu bar
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)playerOneOptions, "Player 1 Settings");

    // Create pop up menu for player one's settings
    HMENU playerTwoOptions = CreatePopupMenu();

    // Create the options
    AppendMenu(playerTwoOptions, MF_STRING, 1011, "Human");
    AppendMenu(playerTwoOptions, MF_STRING, 1012, "MiniMax");
    AppendMenu(playerTwoOptions, MF_STRING, 1013, "MCTS");

    // Add the the menu bar
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)playerTwoOptions, "Player 2 Settings");

    // Create pop up menu for minimax options
    HMENU minimaxOptions = CreatePopupMenu();
    HMENU depthMenu = CreatePopupMenu();
    // Create the options
    AppendMenu(depthMenu, MF_STRING, 1014, "1");
    AppendMenu(depthMenu, MF_STRING, 1015, "2");
    AppendMenu(depthMenu, MF_STRING, 1016, "3");

    AppendMenu(minimaxOptions, MF_STRING | MF_POPUP, (UINT_PTR)depthMenu, "Search Depth");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)minimaxOptions, "MiniMax Options");

    return hMenu;
}

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
    SDL_Handler *handler = new SDL_Handler();
    handler->init();

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (SDL_GetWindowWMInfo(handler->Window, &wmInfo))
    {
        HWND hwnd = wmInfo.info.win.window;

        HMENU hmenu = CreateMainMenu();
        SetMenu(hwnd, hmenu);
    }

    Game *newGame = new Game(7, handler);

    return 0;
}
