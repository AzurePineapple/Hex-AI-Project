#ifndef MENUSTATE_H
#define MENUSTATE_H

// Enumerate some values instead of numbers for readability

enum MenuIDS
{
    BOARD_SIZE_5x5 = 1001,
    BOARD_SIZE_6x6,
    BOARD_SIZE_7x7,
    BOARD_SIZE_8x8,
    BOARD_SIZE_9x9,
    BOARD_SIZE_10x10,
    BOARD_SIZE_11x11,
    PLAYER_ONE_HUMAN,
    PLAYER_ONE_MINIMAX,
    PLAYER_ONE_MCTS,
    PLAYER_TWO_HUMAN,
    PLAYER_TWO_MINIMAX,
    PLAYER_TWO_MCTS,
    MINIMAX_DEPTH_1,
    MINIMAX_DEPTH_2,
    MINIMAX_DEPTH_3,
    MCTS_TIME_3_SEC,
    MCTS_TIME_5_SEC,
    MCTS_TIME_10_SEC,
    MCTS_TIME_30_SEC,
    MCTS_ITER_1000,
    MCTS_ITER_10000,
    MCTS_ITER_100000,
    START_GAME
};

// Structure to hold menu state values
struct MenuState
{
    int selectedBoardSize = BOARD_SIZE_7x7;           // Default to 7x7
    int selectedPlayerOneOption = PLAYER_ONE_HUMAN;   // Default to Human
    int selectedPlayerTwoOption = PLAYER_TWO_MINIMAX; // Default to Human
    int selectedMinimaxDepth = MINIMAX_DEPTH_2;       // Default to depth 1
    int selectedMCTSTimeLimit = MCTS_TIME_10_SEC;     // Default to 3 seconds
    int selectedMCTSIterationLimit = MCTS_ITER_10000; // Default to 1000 iterations
};

#endif