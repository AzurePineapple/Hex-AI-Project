#include "Game.cpp"
#include <limits>
#include <iostream>
// #include "ResistanceDistance.cpp"
// #include "MiniMax.cpp"
#include <limits.h>
#include <cstdlib>
#include <ctime>
#include "src/include/Eigen/Dense"

// Function to deslect all menu items in a given menu
void UncheckAllMenuItems(HMENU hMenu, std::vector<int> itemIDs)
{
    for (int itemID : itemIDs)
    {
        CheckMenuItem(hMenu, itemID, MF_UNCHECKED);
    }
}

// Swaps the selected menu item if the passed one isn't the same as the currently selected one
void ToggleMenuItem(HMENU hMenu, int itemID, int &currentSelectedID)
{
    if (itemID != currentSelectedID)
    {
        CheckMenuItem(hMenu, currentSelectedID, MF_UNCHECKED);
        CheckMenuItem(hMenu, itemID, MF_CHECKED);
        currentSelectedID = itemID;
    }
}

// Create the menu objects
HMENU CreateMainMenu(MenuState options)
{
    HMENU hMenu = CreateMenu();

    // Create pop-up menu for board size options
    HMENU boardSizeMenu = CreatePopupMenu();
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_5x5, "5x5");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_6x6, "6x6");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_7x7, "7x7");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_8x8, "8x8");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_9x9, "9x9");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_10x10, "10x10");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_11x11, "11x11");

    // Select the default value
    CheckMenuItem(boardSizeMenu, options.selectedBoardSize, MF_CHECKED);
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)boardSizeMenu, "Board Size");

    // Create pop-up menu for player one's settings
    HMENU playerOneOptions = CreatePopupMenu();
    AppendMenu(playerOneOptions, MF_STRING, PLAYER_ONE_HUMAN, "Human");
    AppendMenu(playerOneOptions, MF_STRING, PLAYER_ONE_MINIMAX, "MiniMax");
    AppendMenu(playerOneOptions, MF_STRING, PLAYER_ONE_MCTS, "MCTS");

    // Select the default value
    CheckMenuItem(playerOneOptions, options.selectedPlayerOneOption, MF_CHECKED);
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)playerOneOptions, "Player 1 Settings");

    // Create pop-up menu for player two's settings
    HMENU playerTwoOptions = CreatePopupMenu();
    AppendMenu(playerTwoOptions, MF_STRING, PLAYER_TWO_HUMAN, "Human");
    AppendMenu(playerTwoOptions, MF_STRING, PLAYER_TWO_MINIMAX, "MiniMax");
    AppendMenu(playerTwoOptions, MF_STRING, PLAYER_TWO_MCTS, "MCTS");

    // Select the default value
    CheckMenuItem(playerTwoOptions, options.selectedPlayerTwoOption, MF_CHECKED);
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)playerTwoOptions, "Player 2 Settings");

    // Create pop-up menu for Minimax options and the sub-menu for depth selection
    HMENU minimaxOptions = CreatePopupMenu();
    HMENU depthMenu = CreatePopupMenu();
    AppendMenu(depthMenu, MF_STRING, MINIMAX_DEPTH_1, "1");
    AppendMenu(depthMenu, MF_STRING, MINIMAX_DEPTH_2, "2");
    AppendMenu(depthMenu, MF_STRING, MINIMAX_DEPTH_3, "3");

    // Select the default value
    CheckMenuItem(depthMenu, options.selectedMinimaxDepth, MF_CHECKED);
    // Add the sub sub menu to the sub menu
    AppendMenu(minimaxOptions, MF_STRING | MF_POPUP, (UINT_PTR)depthMenu, "Search Depth");
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)minimaxOptions, "MiniMax Options");

    // Create MCTS options
    HMENU mctsOptions = CreatePopupMenu();
    HMENU timeLimitMenu = CreatePopupMenu();
    HMENU iterationLimitMenu = CreatePopupMenu();
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_3_SEC, "3 Sec");
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_5_SEC, "5 Sec");
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_10_SEC, "10 Sec");
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_30_SEC, "30 Sec");

    // Select the default value
    CheckMenuItem(timeLimitMenu, options.selectedMCTSTimeLimit, MF_CHECKED);

    AppendMenu(iterationLimitMenu, MF_STRING, MCTS_ITER_1000, "1000");
    AppendMenu(iterationLimitMenu, MF_STRING, MCTS_ITER_10000, "10000");
    AppendMenu(iterationLimitMenu, MF_STRING, MCTS_ITER_100000, "100000");

    // Select the default value
    CheckMenuItem(iterationLimitMenu, options.selectedMCTSIterationLimit, MF_CHECKED);
    // Add the sub sub menu to the sub menu
    AppendMenu(mctsOptions, MF_STRING | MF_POPUP, (UINT_PTR)timeLimitMenu, "Time Limit");
    // Add the sub sub menu to the sub menu
    AppendMenu(mctsOptions, MF_STRING | MF_POPUP, (UINT_PTR)iterationLimitMenu, "Iteration Limit");
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)mctsOptions, "MCTS Options");

    // Append the swap button
    AppendMenu(hMenu, MF_STRING, SWAP_MOVE, "Swap Move");
    // Append a start button
    AppendMenu(hMenu, MF_STRING, START_GAME, "Start Game");

    return hMenu;
}

void StartGame(SDL_Handler *handler, MenuState options, HMENU hMenu, HWND hwnd)
{
    // Turn settings menu's off when game is started
    setMenuState(hwnd, hMenu, 0, false); // Pass the menu to be set by position, i.e boardSize is 0, playerOne is 1 etc.
    setMenuState(hwnd, hMenu, 1, false); // playerOneOptions
    setMenuState(hwnd, hMenu, 2, false); // playerTwoOptions
    setMenuState(hwnd, hMenu, 3, false); // minimaxOptions
    setMenuState(hwnd, hMenu, 4, false); // mctsOptions
    setMenuState(hwnd, hMenu, 5, false); // swapbutton (reenabled at beginning of game cycle)

    // Create the game object
    Game *newGame = new Game(options, handler, hMenu, hwnd);
}

void ProcessMenuSelection(HWND hwnd, WPARAM wParam, SDL_Handler *handler, MenuState &options)
{
    int wmId = LOWORD(wParam);

    HMENU hMenu = GetMenu(hwnd);
    HMENU boardSizeMenu = GetSubMenu(hMenu, 0);
    HMENU playerOneOptions = GetSubMenu(hMenu, 1);
    HMENU playerTwoOptions = GetSubMenu(hMenu, 2);
    HMENU minimaxOptions = GetSubMenu(hMenu, 3);
    HMENU depthMenu = GetSubMenu(minimaxOptions, 0);
    HMENU mctsOptions = GetSubMenu(hMenu, 4);
    HMENU timeLimitMenu = GetSubMenu(mctsOptions, 0);
    HMENU iterationLimitMenu = GetSubMenu(mctsOptions, 1);

    switch (wmId)
    {
    // Skip the break statement in all but the last case so that all above cases go to the same code block, useful trick
    case BOARD_SIZE_5x5:
    case BOARD_SIZE_6x6:
    case BOARD_SIZE_7x7:
    case BOARD_SIZE_8x8:
    case BOARD_SIZE_9x9:
    case BOARD_SIZE_10x10:
    case BOARD_SIZE_11x11:
        // Board size menu changes toggle board size menu

        // Pass options value by reference, so that it is changed within
        ToggleMenuItem(boardSizeMenu, wmId, options.selectedBoardSize);
        break;
    case PLAYER_ONE_HUMAN:
    case PLAYER_ONE_MINIMAX:
    case PLAYER_ONE_MCTS:
        // Player one menu changes toggle player menu
        ToggleMenuItem(playerOneOptions, wmId, options.selectedPlayerOneOption);
        break;
    case PLAYER_TWO_HUMAN:
    case PLAYER_TWO_MINIMAX:
    case PLAYER_TWO_MCTS:
        // Player two menu changes toggle player menu
        ToggleMenuItem(playerTwoOptions, wmId, options.selectedPlayerTwoOption);
        break;
    case MINIMAX_DEPTH_1:
    case MINIMAX_DEPTH_2:
    case MINIMAX_DEPTH_3:
        // Minimax menu changes toggle minimax menu
        ToggleMenuItem(depthMenu, wmId, options.selectedMinimaxDepth);
        break;
    case MCTS_TIME_3_SEC:
    case MCTS_TIME_5_SEC:
    case MCTS_TIME_10_SEC:
    case MCTS_TIME_30_SEC:
        // mcts menu changes toggle mcts menu
        ToggleMenuItem(timeLimitMenu, wmId, options.selectedMCTSTimeLimit);
        break;
    case MCTS_ITER_1000:
    case MCTS_ITER_10000:
    case MCTS_ITER_100000:
        // mcts menu changes toggle mcts menu
        ToggleMenuItem(iterationLimitMenu, wmId, options.selectedMCTSIterationLimit);
        break;
    case START_GAME:
        StartGame(handler, options, hMenu, hwnd);
        break;
    }
}

// LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     static MenuState options;
//     switch (message)
//     {
//     case WM_COMMAND:
//     {
//         SDL_Handler *handler = reinterpret_cast<SDL_Handler *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
//         ProcessMenuSelection(hwnd, wParam, handler, options);
//     }
//     break;
//     case WM_DESTROY:
//         PostQuitMessage(0);
//         break;
//     default:
//         return DefWindowProc(hwnd, message, wParam, lParam);
//     }
//     return 0;
// }

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

    static MenuState options;

    SDL_Handler *handler = new SDL_Handler();
    handler->init();
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(handler->Window, &wmInfo);

    HWND hwnd = wmInfo.info.win.window;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(handler));

    HMENU hMenu = CreateMainMenu(options);
    SetMenu(hwnd, hMenu);

    // Main message loop
    MSG msg;
    bool running = true;
    while (running)
    {
        // Check for SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                PostQuitMessage(0);
                running = false;
            }
            // Handle other SDL events as needed

            if (event.type == SDL_SYSWMEVENT)
            {
                auto wParam = event.syswm.msg->msg.win.wParam;
                ProcessMenuSelection(hwnd, wParam, handler, options);
            }
        }
    }

    return 0;
}
