#include "Board.cpp"
#include "Player.cpp"
#include "SDL_Handler.cpp"
#include "MenuState.h"
#include <vector>
#include <set>
#include <iostream>
#include <windows.h>

class Game
{
private:
    Player *activePlayer;
    std::pair<Player *, Player *> players;

    Player one;
    Player two;

    std::pair<int, int> previousMove = {-1, -1};

    // Sets containing opening moves that should be swapped (sets have log time complexity to search vs vectors linear)
    // Hard coded based on diagrams from https://www.hexwiki.net/index.php/Swap_rule and https://www.hexwiki.net/index.php/Small_boards
    std::set<std::pair<int, int>> movesToSwapSize2 = {
        {0, 1},
        {1, 0}};
    std::set<std::pair<int, int>> movesToSwapSize3 = {
        {0, 2},
        {1, 0},
        {1, 1},
        {1, 2},
        {2, 0}};
    std::set<std::pair<int, int>> movesToSwapSize4 = {
        {0, 3},
        {1, 2},
        {2, 1},
        {3, 0}};
    std::set<std::pair<int, int>> movesToSwapSize5 = {
        {0, 4},
        {1, 1},
        {1, 2},
        {1, 3},
        {1, 4},
        {2, 1},
        {2, 2},
        {2, 3},
        {3, 0},
        {3, 1},
        {3, 2},
        {3, 3},
        {4, 0}};
    std::set<std::pair<int, int>> movesToSwapSize6 = {
        {0, 5},
        {1, 1},
        {1, 2},
        {1, 3},
        {1, 4},
        {1, 5},
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4},
        {2, 5},
        {3, 0},
        {3, 1},
        {3, 2},
        {3, 3},
        {3, 4},
        {3, 5},
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {5, 0}};
    std::set<std::pair<int, int>> movesToSwapSize7 = {
        {0, 6},
        {1, 2},
        {1, 4},
        {1, 5},
        {1, 6},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4},
        {2, 5},
        {3, 0},
        {3, 1},
        {3, 2},
        {3, 3},
        {3, 4},
        {3, 5},
        {3, 6},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5},
        {5, 0},
        {5, 1},
        {5, 2},
        {5, 4},
        {6, 0}};
    std::set<std::pair<int, int>> movesToSwapSize8 = {
        {0, 7},
        {1, 6},
        {1, 7},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4},
        {2, 5},
        {2, 6},
        {3, 1},
        {3, 2},
        {3, 3},
        {3, 4},
        {3, 5},
        {3, 6},
        {3, 7},
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5},
        {4, 6},
        {5, 1},
        {5, 2},
        {5, 3},
        {5, 4},
        {5, 5},
        {5, 6},
        {6, 0},
        {6, 1},
        {7, 0},
    };
    std::set<std::pair<int, int>> movesToSwapSize9 = {
        {0, 8},
        {1, 0},
        {1, 1},
        {1, 2},
        {1, 7},
        {1, 8},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4},
        {2, 5},
        {2, 6},
        {2, 7},
        {3, 0},
        {3, 1},
        {3, 2},
        {3, 3},
        {3, 4},
        {3, 5},
        {3, 6},
        {3, 7},
        {3, 8},
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5},
        {4, 6},
        {4, 7},
        {4, 8},
        {5, 0},
        {5, 1},
        {5, 2},
        {5, 3},
        {5, 4},
        {5, 5},
        {5, 6},
        {5, 7},
        {5, 8},
        {6, 1},
        {6, 2},
        {6, 3},
        {6, 4},
        {6, 5},
        {6, 6},
        {6, 7},
        {7, 0},
        {7, 1},
        {7, 6},
        {7, 7},
        {7, 8},
        {8, 0},
    };
    std::set<std::pair<int, int>> movesToSwapSize10 = {
        //  IMPORTANT, ASTERISKS HERE MARK PROVEN WINNING OR LOSING MOVES, NOT EVEN MOVES
        {0, 9},
        {1, 8}, //*
        {1, 9},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4},
        {2, 5},
        {2, 6},
        {2, 7},
        {2, 8},
        {3, 0},
        {3, 1},
        {3, 2},
        {3, 3},
        {3, 4},
        {3, 5},
        {3, 6},
        {3, 7},
        {3, 8},
        {3, 9},
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5}, //*
        {4, 6},
        {4, 7},
        {4, 8},
        {4, 9},
        {5, 0},
        {5, 1},
        {5, 2},
        {5, 3},
        {5, 4}, //*
        {5, 5},
        {5, 6},
        {5, 7},
        {5, 8},
        {5, 9},
        {6, 0},
        {6, 1},
        {6, 2},
        {6, 3},
        {6, 4},
        {6, 5},
        {6, 6},
        {6, 7},
        {6, 8},
        {6, 9},
        {7, 1},
        {7, 2},
        {7, 3},
        {7, 4},
        {7, 5},
        {7, 6},
        {7, 7},
        {7, 8},
        {8, 0},
        {8, 1}, //*
        {9, 0},
    };
    std::set<std::pair<int, int>> movesToSwapSize11 = {
        {0, 10}, // *
        {1, 9},
        {1, 10},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4},
        {2, 5}, //*
        {2, 6},
        {2, 7},
        {2, 8},
        {2, 9},
        {3, 0}, //*
        {3, 1},
        {3, 2},
        {3, 3},
        {3, 4},
        {3, 5},
        {3, 6},
        {3, 7},
        {3, 8},
        {3, 9},
        {3, 10}, //*
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4},
        {4, 5},
        {4, 6},
        {4, 7},
        {4, 8},
        {4, 9},
        {4, 10}, //*
        {5, 0},  //*
        {5, 1},
        {5, 2},
        {5, 3},
        {5, 4},
        {5, 5},
        {5, 6},
        {5, 7},
        {5, 8},
        {5, 9},
        {5, 10}, //*
        {6, 0},  //*
        {6, 1},
        {6, 2},
        {6, 3},
        {6, 4},
        {6, 5},
        {6, 6},
        {6, 7},
        {6, 8},
        {6, 9},
        {6, 10},
        {7, 0}, //*
        {7, 1},
        {7, 2},
        {7, 3},
        {7, 4},
        {7, 5},
        {7, 6},
        {7, 7},
        {7, 8},
        {7, 9},
        {7, 10}, //*
        {8, 1},
        {8, 2},
        {8, 3},
        {8, 4},
        {8, 5}, //*
        {8, 6},
        {8, 7},
        {8, 8},
        {8, 9},
        {9, 0},
        {9, 1},
        {10, 0}, //*
    };

public:
    Game(MenuState options, SDL_Handler *handler);
    ~Game();

    // Swap the players after they take their turns
    void swapActivePlayer();

    void processOptions(MenuState options, int &size, std::string &playerOneType, std::string &playerTwoType, int &minimaxDepth, int &mctsTimeLimit, int &mctsIterLimit);
};
