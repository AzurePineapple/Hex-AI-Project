#include "Game.cpp"
#include <limits>
#include <iostream>
// #include "ResistanceDistance.cpp"
// #include "MiniMax.cpp"
#include <limits.h>
#include <cstdlib>
#include <ctime>
#include "src/include/Eigen/Dense"

// Function to print the progress bar
void printProgressBar(int current, int total, int barWidth = 50)
{
    float progress = (float)current / total;
    int pos = barWidth * progress;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

void ParameterTuning()
{
    SDL_Handler *handler = new SDL_Handler();
    // Set the size of the test board
    const int boardSize = 9;
    // Set the number of test matches to do
    int numMatches = 30;

    // std::vector<double> explorationConstants = {0.0, 0.1, 0.2, 0.4, 0.5, 0.6, 0.7};

    // for (double C : explorationConstants)
    // {
    //     playerSettings playerOne;
    //     playerOne.experimentName = "C=" + std::to_string(C);
    //     playerOne.explorationConstant = C;

    //     // Default control player, using default values. Default value details are in playerSettings.h
    //     playerSettings playerTwo;
    //     playerTwo.playerCode = 2;

    //     // Bool to alternate which player goes first
    //     bool swapFirstPlayer = false;
    //     for (int i = 0; i < numMatches; i++)
    //     {
    //         Game *testGame = new Game(playerOne, playerTwo, boardSize, handler, swapFirstPlayer);
    //         delete testGame;
    //         swapFirstPlayer = !swapFirstPlayer;

    //         // Update the progress bar
    //         printProgressBar(i + 1, numMatches);
    //     }
    //     std::cout << "Completed " << numMatches << " games. Find results in parameterTuning/" << playerOne.experimentName << "_VS_" << playerTwo.experimentName << ".csv" << std::endl;
    // }
    // std::cout << "Exploration constants finished" << std::endl;

    // std::vector<double> raveBiases = {0.0005, 0.00025, 0.000125};
    // for (double b : raveBiases)
    // {
    //     playerSettings playerOne;
    //     playerOne.experimentName = "b=" + std::to_string(b);
    //     playerOne.RAVEBias = b;

    //     // Default control player, using default values. Default value details are in playerSettings.h
    //     playerSettings playerTwo;
    //     playerTwo.playerCode = 2;

    //     // Bool to alternate which player goes first
    //     bool swapFirstPlayer = false;
    //     for (int i = 0; i < numMatches; i++)
    //     {
    //         Game *testGame = new Game(playerOne, playerTwo, boardSize, handler, swapFirstPlayer);
    //         delete testGame;
    //         swapFirstPlayer = !swapFirstPlayer;

    //         // Update the progress bar
    //         printProgressBar(i + 1, numMatches);
    //     }
    //     std::cout << "Completed " << numMatches << " games. Find results in parameterTuning/" << playerOne.experimentName << "_VS_" << playerTwo.experimentName << ".csv" << std::endl;
    // }

    // std::cout << "RaveBiases finished" << std::endl;

    // std::vector<double> bridgeWeights = {
    //     //                                      2.1,
    //     //                                      2.2,
    //     //                                      2.3,
    //     //                                      2.4,
    //     //                                      2.5,
    //     //                                      2.6,
    //     //                                      2.7,
    //     //                                      2.8,
    //     2.9};
    // for (auto BW : bridgeWeights)
    // {
    //     playerSettings playerOne;
    //     playerOne.AIType = "minimax";
    //     playerOne.experimentName = "BW=" + std::to_string(BW);
    //     playerOne.bridgeWeight = BW;

    //     // Default control player, using default values. Default value details are in playerSettings.h
    //     playerSettings playerTwo;
    //     playerTwo.playerCode = 2;
    //     playerTwo.AIType = "minimax";
    //     playerTwo.experimentName = "DefaultMinimax";

    //     // Bool to alternate which player goes first
    //     bool swapFirstPlayer = false;
    //     printProgressBar(0, numMatches);
    //     for (int i = 0; i < numMatches; i++)
    //     {
    //         Game *testGame = new Game(playerOne, playerTwo, boardSize, handler, swapFirstPlayer);
    //         delete testGame;
    //         swapFirstPlayer = !swapFirstPlayer;

    //         // Update the progress bar
    //         printProgressBar(i + 1, numMatches);
    //     }
    //     std::cout << "Completed " << numMatches << " games. Find results in parameterTuning/" << playerOne.experimentName << "_VS_" << playerTwo.experimentName << ".csv" << std::endl;
    // }
    // std::cout << "Bridge Weights (10s) finished" << std::endl;

    // TODO: This test was kind of a dud because I fucked up the second player's time limit

    // bridgeWeights = {2.1,
    //                  2.2,
    //                  2.3,
    //                  2.4,
    //                  2.5,
    //                  2.6,
    //                  2.7,
    //                  2.8,
    //                  2.9};
    // for (auto BW : bridgeWeights)
    // {
    //     playerSettings playerOne;
    //     playerOne.AIType = "minimax";
    //     playerOne.experimentName = "BW=" + std::to_string(BW) + "_TimeLim=60";
    //     playerOne.bridgeWeight = BW;
    //     playerOne.mmTimeLimit = 60.0;

    //     // Default control player, using default values. Default value details are in playerSettings.h
    //     playerSettings playerTwo;
    //     playerTwo.playerCode = 2;
    //     playerTwo.AIType = "minimax";
    //     playerTwo.experimentName = "DefaultMinimax_TimeLim=60";
    //     playerOne.mmTimeLimit = 60.0; // FIXME: DUH

    //     // Bool to alternate which player goes first
    //     bool swapFirstPlayer = false;
    //     printProgressBar(0, numMatches);
    //     for (int i = 0; i < numMatches; i++)
    //     {
    //         Game *testGame = new Game(playerOne, playerTwo, boardSize, handler, swapFirstPlayer);
    //         delete testGame;
    //         swapFirstPlayer = !swapFirstPlayer;

    //         // Update the progress bar
    //         printProgressBar(i + 1, numMatches);
    //     }
    //     std::cout << "Completed " << numMatches << " games. Find results in parameterTuning/" << playerOne.experimentName << "_VS_" << playerTwo.experimentName << ".csv" << std::endl;
    // }
    // std::cout << "Bridge Weights finished" << std::endl;

    double bestC = 0.1;
    double bestB = 0.005;
    double bestBridgeWeight = 2.3;
    numMatches = 100;

    playerSettings playerOne;
    playerOne.experimentName = "BestMCTS";
    playerOne.AIType = "mcts";
    playerOne.explorationConstant = bestC;
    playerOne.RAVEBias = bestB;
    playerOne.mctsTimeLimit = 10.0;

    playerSettings playerTwo;
    playerTwo.playerCode = 2;
    playerTwo.experimentName = "BestMinimax";
    playerTwo.AIType = "minimax";
    playerTwo.bridgeWeight = bestBridgeWeight;
    playerTwo.mmTimeLimit = 10.0;

    // Bool to alternate which player goes first
    bool swapFirstPlayer = false;
    printProgressBar(0, numMatches);
    for (int i = 0; i < numMatches; i++)
    {
        Game *testGame = new Game(playerOne, playerTwo, boardSize, handler, swapFirstPlayer);
        delete testGame;
        swapFirstPlayer = !swapFirstPlayer;

        // Update the progress bar
        printProgressBar(i + 1, numMatches);
    }
    std::cout << "Completed " << numMatches << " games. Find results in parameterTuning/" << playerOne.experimentName << "_VS_" << playerTwo.experimentName << ".csv" << std::endl;

    std::cout << "MCTS vs. MiniMax finished" << std::endl;
}

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
    HMENU boardSettings = CreatePopupMenu();
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_5x5, "5x5");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_6x6, "6x6");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_7x7, "7x7");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_8x8, "8x8");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_9x9, "9x9");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_10x10, "10x10");
    AppendMenu(boardSizeMenu, MF_STRING, BOARD_SIZE_11x11, "11x11");

    // Select the default value
    CheckMenuItem(boardSizeMenu, options.selectedBoardSize, MF_CHECKED);

    HMENU boardColour = CreatePopupMenu();

    AppendMenu(boardColour, MF_STRING, BLACK_WHITE, "Black/White");
    AppendMenu(boardColour, MF_STRING, RED_BLUE, "Red/Blue");
    AppendMenu(boardColour, MF_STRING, GREEN_PURPLE, "Green/Purple");
    AppendMenu(boardColour, MF_STRING, BLUE_ORANGE, "Blue/Orange");
    AppendMenu(boardColour, MF_STRING, BLUE_YELLOW, "Blue/Yellow");

    CheckMenuItem(boardColour, options.selectedBoardColour, MF_CHECKED);

    // Add the sub menus to the main menu
    AppendMenu(boardSettings, MF_STRING | MF_POPUP, (UINT_PTR)boardSizeMenu, "Board Size");
    AppendMenu(boardSettings, MF_STRING | MF_POPUP, (UINT_PTR)boardColour, "Board Colour");

    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)boardSettings, "Board Settings");

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
    HMENU mmTimeLimitMenu = CreatePopupMenu();
    AppendMenu(depthMenu, MF_STRING, MINIMAX_DEPTH_1, "1");
    AppendMenu(depthMenu, MF_STRING, MINIMAX_DEPTH_2, "2");
    AppendMenu(depthMenu, MF_STRING, MINIMAX_DEPTH_3, "3");

    // Select the default value
    CheckMenuItem(depthMenu, options.selectedMinimaxDepth, MF_CHECKED);

    AppendMenu(mmTimeLimitMenu, MF_STRING, MM_TIME_3_SEC, "3 Sec");
    AppendMenu(mmTimeLimitMenu, MF_STRING, MM_TIME_5_SEC, "5 Sec");
    AppendMenu(mmTimeLimitMenu, MF_STRING, MM_TIME_10_SEC, "10 Sec");
    AppendMenu(mmTimeLimitMenu, MF_STRING, MM_TIME_30_SEC, "30 Sec");

    CheckMenuItem(mmTimeLimitMenu, options.selectedMinimaxTimeLimit, MF_CHECKED);

    // Add the sub sub menu to the sub menu
    AppendMenu(minimaxOptions, MF_STRING | MF_POPUP, (UINT_PTR)depthMenu, "Search Depth");
    AppendMenu(minimaxOptions, MF_STRING | MF_POPUP, (UINT_PTR)mmTimeLimitMenu, "Time Limit");
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)minimaxOptions, "MiniMax Options");

    // Create MCTS options
    HMENU mctsOptions = CreatePopupMenu();
    HMENU timeLimitMenu = CreatePopupMenu();
    HMENU iterationLimitMenu = CreatePopupMenu();
    HMENU parallelStyleMenu = CreatePopupMenu();
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_3_SEC, "3 Sec");
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_5_SEC, "5 Sec");
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_10_SEC, "10 Sec");
    AppendMenu(timeLimitMenu, MF_STRING, MCTS_TIME_30_SEC, "30 Sec");

    // Select the default value
    CheckMenuItem(timeLimitMenu, options.selectedMCTSTimeLimit, MF_CHECKED);

    AppendMenu(iterationLimitMenu, MF_STRING, MCTS_ITER_1000, "1000");
    AppendMenu(iterationLimitMenu, MF_STRING, MCTS_ITER_10000, "10000");
    AppendMenu(iterationLimitMenu, MF_STRING, MCTS_ITER_25000, "25000");

    // Select the default value
    CheckMenuItem(iterationLimitMenu, options.selectedMCTSIterationLimit, MF_CHECKED);

    AppendMenu(parallelStyleMenu, MF_STRING, ROOT, "Root Parallelisation");
    AppendMenu(parallelStyleMenu, MF_STRING, LEAF, "Leaf Parallelisation");

    // Select the default value
    CheckMenuItem(parallelStyleMenu, options.selectedParallelStyle, MF_CHECKED);

    // Add the sub sub menu to the sub menu
    AppendMenu(mctsOptions, MF_STRING | MF_POPUP, (UINT_PTR)timeLimitMenu, "Time Limit");
    // Add the sub sub menu to the sub menu
    AppendMenu(mctsOptions, MF_STRING | MF_POPUP, (UINT_PTR)iterationLimitMenu, "Iteration Limit");
    // Add the sub sub menu to the sub menu
    AppendMenu(mctsOptions, MF_STRING | MF_POPUP, (UINT_PTR)parallelStyleMenu, "Parallelisation Type");
    // Add the sub menu to the main menu
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)mctsOptions, "MCTS Options");

    // Horizontal separator
    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hMenu, MF_STRING | MF_DISABLED, 0, "");

    // Append a start button
    AppendMenu(hMenu, MF_STRING, START_GAME, "Start Game");
    // // Append a reset button
    AppendMenu(hMenu, MF_STRING, RESET_GAME, "Reset Game");

    // Append the swap button
    AppendMenu(hMenu, MF_STRING, SWAP_MOVE, "Swap Move");

    return hMenu;
}

void resetGame(Game *game, HWND hwnd, HMENU hMenu)
{
    if (game != nullptr)
    {
        delete game;
        game = nullptr;
    }

    enableAllMenus(hwnd, hMenu);
}

void startGame(SDL_Handler *handler, MenuState options, HMENU hMenu, HWND hwnd, Game *game)
{
    if (game != nullptr)
    {
        delete game;
    }

    // Turn settings menus off when game is started
    setMenuState(hwnd, hMenu, 0, false); // Pass the menu to be set by position, i.e boardSize is 0, playerOne is 1 etc.
    setMenuState(hwnd, hMenu, 1, false); // playerOneOptions
    setMenuState(hwnd, hMenu, 2, false); // playerTwoOptions
    setMenuState(hwnd, hMenu, 3, false); // minimaxOptions
    setMenuState(hwnd, hMenu, 4, false); // mctsOptions
    setMenuState(hwnd, hMenu, 7, false); // swapbutton (reenabled at beginning of game cycle)

    // Create the game object
    game = new Game(options, handler, hMenu, hwnd);
}

void ProcessMenuSelection(HWND hwnd, WPARAM wParam, SDL_Handler *handler, MenuState &options, Game *game)
{
    int wmId = LOWORD(wParam);

    HMENU hMenu = GetMenu(hwnd);
    HMENU boardSettingsMenu = GetSubMenu(hMenu, 0);
    HMENU boardSizeMenu = GetSubMenu(boardSettingsMenu, 0);
    HMENU boardColourMenu = GetSubMenu(boardSettingsMenu, 1);
    HMENU playerOneOptions = GetSubMenu(hMenu, 1);
    HMENU playerTwoOptions = GetSubMenu(hMenu, 2);
    HMENU minimaxOptions = GetSubMenu(hMenu, 3);
    HMENU depthMenu = GetSubMenu(minimaxOptions, 0);
    HMENU mmTimeLimitMenu = GetSubMenu(minimaxOptions, 1);
    HMENU mctsOptions = GetSubMenu(hMenu, 4);
    HMENU timeLimitMenu = GetSubMenu(mctsOptions, 0);
    HMENU iterationLimitMenu = GetSubMenu(mctsOptions, 1);
    HMENU parallelStyleMenu = GetSubMenu(mctsOptions, 2);

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
    case MCTS_ITER_25000:
        // mcts menu changes toggle mcts menu
        ToggleMenuItem(iterationLimitMenu, wmId, options.selectedMCTSIterationLimit);
        break;
    case START_GAME:
        startGame(handler, options, hMenu, hwnd, game);
        break;
    case MM_TIME_3_SEC:
    case MM_TIME_5_SEC:
    case MM_TIME_10_SEC:
    case MM_TIME_30_SEC:
        ToggleMenuItem(mmTimeLimitMenu, wmId, options.selectedMinimaxTimeLimit);
        break;
    case RESET_GAME:
        resetGame(game, hwnd, hMenu);
        break;
    case BLACK_WHITE:
    case RED_BLUE:
    case GREEN_PURPLE:
    case BLUE_ORANGE:
    case BLUE_YELLOW:
        ToggleMenuItem(boardColourMenu, wmId, options.selectedBoardColour);
        break;
    case ROOT:
    case LEAF:
        ToggleMenuItem(parallelStyleMenu, wmId, options.selectedParallelStyle);
        break;
    }
}

int main(int argv, char **args)
{

    bool TestingMode = true;

    if (TestingMode)
    {
        ParameterTuning();
    }
    else
    {
        srand(static_cast<unsigned int>(time(0)));

        static MenuState options;

        SDL_Handler *handler = new SDL_Handler();
        handler->init();
        handler->wipeScreen();
        handler->showMainScreen();
        // Tells SDL events to listen to windows api events
        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        Game *game = nullptr;

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
        bool clickDetected = false;
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

                if (event.type == SDL_SYSWMEVENT)
                {
                    auto wParam = event.syswm.msg->msg.win.wParam;
                    ProcessMenuSelection(hwnd, wParam, handler, options, game);
                }
            }
        }
    }

    return 0;
}
