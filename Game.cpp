#include "Game.h"

// Default Constructor
Game::Game(MenuState options, SDL_Handler *handler, HMENU hMenu, HWND hwnd)
{
    // Process incoming game options

    int size;
    int boardColour;
    std::string playerOneType;
    std::string playerTwoType;
    int minimaxDepth;
    double mmTimeLimit;
    double mctsTimeLimit;
    int mctsIterLimit;
    bool rootParallelised;

    processOptions(options, size, boardColour, playerOneType, playerTwoType, minimaxDepth, mmTimeLimit, mctsTimeLimit, mctsIterLimit, rootParallelised);

    assert(size > 0 && size < 12);

    Board *gameBoard = new Board(size, boardColour);

    if (playerOneType == "human")
    {
        one.createPlayer("human", 1);
    }
    else if (playerOneType == "minimax")
    {
        one.createPlayer("computer", 1, "minimax", size, true, minimaxDepth, mmTimeLimit, mctsTimeLimit, mctsIterLimit, rootParallelised);
    }
    else if (playerOneType == "mcts")
    {
        one.createPlayer("computer", 1, "mcts", size, true, minimaxDepth, mmTimeLimit, mctsTimeLimit, mctsIterLimit, rootParallelised);
    }

    if (playerTwoType == "human")
    {
        two.createPlayer("human", 2);
    }
    else if (playerTwoType == "minimax")
    {
        two.createPlayer("computer", 2, "minimax", size, true, minimaxDepth, mmTimeLimit, mctsTimeLimit, mctsIterLimit, rootParallelised);
    }
    else if (playerTwoType == "mcts")
    {
        two.createPlayer("computer", 2, "mcts", size, true, minimaxDepth, mmTimeLimit, mctsTimeLimit, mctsIterLimit, rootParallelised);
    }

    // MCTS

    // Opponent (player or test) First
    // one.createPlayer("human", 1);
    // two.createPlayer("computer", 2, "mcts", size, true);
    // AI First
    // one.createPlayer("computer", 1, "mcts", size, true);
    // two.createPlayer("human", 2);

    // // Self play testing - noPP first
    // // one.createPlayer("computer", 1, "mcts", size, false);
    // // two.createPlayer("computer", 2, "mcts", size, true);
    // // Self play testing - PP first
    // one.createPlayer("computer", 1, "mcts", size, true);
    // two.createPlayer("computer", 2, "mcts", size, false);

    // Minimax
    // one.createPlayer("human", 1);
    // two.createPlayer("computer", 2, "minimax", size, false, 2);

    players.first = &one;
    players.second = &two;

    activePlayer = players.first;

    gameBoard->createBoard(handler->SCREEN_WIDTH);
    gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);

    bool quit = false;
    bool gameOver = false;
    bool swapEnabled = false;
    int turnCounter = 0;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
                break;
            }

            if (event.type == SDL_SYSWMEVENT && LOWORD(event.syswm.msg->msg.win.wParam) == RESET_GAME)
            {
                handler->showMainScreen();
                quit = true;
                break;
            }

            // check the previousMove is being updated correctly
            if (turnCounter != 0 && previousMove.first == -1 && previousMove.second == -1)
            {
                throw std::logic_error("Previous move logic has fucked up");
            }

            // Enable swap button on turn 1, disable after
            if (turnCounter == 1 && !swapEnabled)
            {
                setMenuState(hwnd, hMenu, 7, true);
                swapEnabled = true;
            }
            if (turnCounter == 2 && swapEnabled)
            {
                setMenuState(hwnd, hMenu, 7, false);
                swapEnabled = false;
            }

            if (!activePlayer->getIsComputer())
            {
                if (event.type == SDL_MOUSEMOTION)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    // Iterate through all Hexes
                    for (int i = 0; i < gameBoard->getSize(); i++)
                    {
                        for (int j = 0; j < gameBoard->getSize(); j++)
                        {

                            // Determine if cursor is within tile's bounding box

                            if (gameBoard->getTileObj(i, j)->BBcoords[0] < x && x < gameBoard->getTileObj(i, j)->BBcoords[2] && gameBoard->getTileObj(i, j)->BBcoords[1] < y && y < gameBoard->getTileObj(i, j)->BBcoords[3])
                            {
                                // Draw the ghost if cursor is within BB
                                gameBoard->getTileObj(i, j)->drawGhost(handler->Renderer, activePlayer->getColour());
                            }
                            else
                            {
                                // Delete the ghost if mouse is outside the given tiles BB
                                gameBoard->getTileObj(i, j)->deleteGhost(handler->Renderer);
                            }
                        }
                    }
                }

                if (event.type == SDL_MOUSEBUTTONDOWN)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    for (int i = 0; i < gameBoard->getSize(); i++)
                    {
                        for (int j = 0; j < gameBoard->getSize(); j++)
                        {

                            // Determine if cursor is within Hex's bounding box
                            if (gameBoard->getTileObj(i, j)->BBcoords[0] < x && x < gameBoard->getTileObj(i, j)->BBcoords[2] && gameBoard->getTileObj(i, j)->BBcoords[1] < y && y < gameBoard->getTileObj(i, j)->BBcoords[3] && gameBoard->getTileObj(i, j)->getContents() == "empty")
                            {
                                std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), i, j);
                                gameOver = result.first;
                                gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);
                                turnCounter += 1;
                                swapActivePlayer();

                                // Update the previous move after move is played
                                previousMove.first = i;
                                previousMove.second = j;
                            }
                        }
                    }
                }

                if (turnCounter == 1 && event.type == SDL_SYSWMEVENT)
                {
                    if (LOWORD(event.syswm.msg->msg.win.wParam) == SWAP_MOVE)
                    {
                        // Swap piece
                        gameBoard->boardSwap();
                        // gameBoard->showBoard();
                        gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);
                        // std::cout << "Swap performed" << std::endl;
                        turnCounter += 1;
                        swapActivePlayer();
                    }
                }
            }
            else
            {
                handler->displayLoading();

                // For the opening move, select from predetermined "fair" moves
                if (turnCounter == 0)
                {

                    std::set<std::pair<int, int>> fairMoves = {
                        {0, size - 1},
                        {1, 2},
                        {1, size - 3},
                        {2, 0},
                        {size - 3, size - 1},
                        {size - 2, 2},
                        {size - 2, size - 3},
                        {size - 1, 0}};

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, fairMoves.size() - 1);
                    int randomIndex = dis(gen);

                    auto it = fairMoves.begin();
                    std::advance(it, randomIndex);

                    auto openingMove = *it;

                    int x = openingMove.first;
                    int y = openingMove.second;

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;

                    gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);
                    turnCounter += 1;
                    swapActivePlayer();
                }
                // Swap rule implementation
                else if (turnCounter == 1)
                {
                    switch (size)
                    {
                    case 2:
                        // Check if leading move should be swapped
                        if (movesToSwapSize2.find(previousMove) != movesToSwapSize2.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 3:
                        // Check if leading move should be swapped
                        if (movesToSwapSize3.find(previousMove) != movesToSwapSize3.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 4:
                        // Check if leading move should be swapped
                        if (movesToSwapSize4.find(previousMove) != movesToSwapSize4.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 5:
                        // Check if leading move should be swapped
                        if (movesToSwapSize5.find(previousMove) != movesToSwapSize5.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 6:
                        // Check if leading move should be swapped
                        if (movesToSwapSize6.find(previousMove) != movesToSwapSize6.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 7:
                        // Check if leading move should be swapped
                        if (movesToSwapSize7.find(previousMove) != movesToSwapSize7.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 8:
                        // Check if leading move should be swapped
                        if (movesToSwapSize8.find(previousMove) != movesToSwapSize8.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 9:
                        // Check if leading move should be swapped
                        if (movesToSwapSize9.find(previousMove) != movesToSwapSize9.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 10:
                        // Check if leading move should be swapped
                        if (movesToSwapSize10.find(previousMove) != movesToSwapSize10.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;

                    case 11:
                        if (movesToSwapSize11.find(previousMove) != movesToSwapSize11.end())
                        {
                            gameBoard->boardSwap();
                            // std::cout << "Swap performed" << std::endl;
                        }
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            gameOver = result.first;
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;

                    default:
                        std::cout << "Invalid board size for swap, cannot perform swap" << std::endl;
                        break;
                    }

                    gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);
                    turnCounter += 1;
                    swapActivePlayer();
                }
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);

                    turnCounter += 1;
                    swapActivePlayer();

                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
            }

            // Update the GUI
            SDL_RenderPresent(handler->Renderer);

            // Close the window if the game is over
            if (gameOver)
            {
                // Reenable options menu's so new game can be started
                enableAllMenus(hwnd, hMenu);
                // Render the gameboard to the console
                gameBoard->showBoard();
                // Render the board to the screen
                SDL_RenderPresent(handler->Renderer);
                // std::cout << "Closing window in 2 seconds!" << std::endl;
                Sleep(2000);
                handler->showMainScreen();
                quit = true;
                break;
            }
        }
    }
}

Game::~Game()
{
}

// Overloaded constructor for testing
Game::Game(playerSettings playerOne, playerSettings playerTwo, int boardSize, SDL_Handler *handler, bool swapFirstPlayer)
{
    one.createPlayer(playerOne.playerType, playerOne.playerCode, playerOne.AIType, boardSize, playerOne.isParallelised, playerOne.mmDepth, playerOne.mmTimeLimit, playerOne.mctsTimeLimit, playerOne.mctsIterLimit, playerOne.rootParallelised, playerOne.explorationConstant, playerOne.RAVEBias, playerOne.bridgeWeight);

    two.createPlayer(playerTwo.playerType, playerTwo.playerCode, playerTwo.AIType, boardSize, playerTwo.isParallelised, playerTwo.mmDepth, playerTwo.mmTimeLimit, playerTwo.mctsTimeLimit, playerTwo.mctsIterLimit, playerTwo.rootParallelised, playerTwo.explorationConstant, playerTwo.RAVEBias, playerTwo.bridgeWeight);

    Board *gameBoard = new Board(boardSize, 1);

    players.first = &one;
    players.second = &two;

    int playedFirst = -1;
    // Alternate between which player goes first
    if (swapFirstPlayer)
    {
        activePlayer = players.second;
        playedFirst = 2;
    }
    else
    {
        activePlayer = players.first;
        playedFirst = 1;
    }

    gameBoard->createBoard(handler->SCREEN_WIDTH);

    bool quit = false;
    bool gameOver = false;
    int winner = -1;
    bool swapPerformed = false;
    int turnCounter = 0;

    while (!quit)
    {
        // check the previousMove is being updated correctly
        if (turnCounter != 0 && previousMove.first == -1 && previousMove.second == -1)
        {
            throw std::logic_error("Previous move logic has fucked up");
        }

        // For the opening move, select from predetermined "fair" moves
        if (turnCounter == 0)
        {
            // Create a set of fair moves. Derived from Seymour's book.
            std::set<std::pair<int, int>> fairMoves = {
                {0, boardSize - 1},
                {1, 2},
                {1, boardSize - 3},
                {2, 0},
                {boardSize - 3, boardSize - 1},
                {boardSize - 2, 2},
                {boardSize - 2, boardSize - 3},
                {boardSize - 1, 0}};

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, fairMoves.size() - 1);
            int randomIndex = dis(gen);

            auto it = fairMoves.begin();
            std::advance(it, randomIndex);

            auto openingMove = *it;

            int x = openingMove.first;
            int y = openingMove.second;

            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
            gameOver = result.first;
            winner = result.second;
            // UPdate the previous move after move is performed
            previousMove.first = x;
            previousMove.second = y;

            turnCounter += 1;
            swapActivePlayer();
        }
        // Swap rule implementation
        else if (turnCounter == 1)
        {
            switch (boardSize)
            {
            case 2:
                // Check if leading move should be swapped
                if (movesToSwapSize2.find(previousMove) != movesToSwapSize2.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 3:
                // Check if leading move should be swapped
                if (movesToSwapSize3.find(previousMove) != movesToSwapSize3.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 4:
                // Check if leading move should be swapped
                if (movesToSwapSize4.find(previousMove) != movesToSwapSize4.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 5:
                // Check if leading move should be swapped
                if (movesToSwapSize5.find(previousMove) != movesToSwapSize5.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 6:
                // Check if leading move should be swapped
                if (movesToSwapSize6.find(previousMove) != movesToSwapSize6.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 7:
                // Check if leading move should be swapped
                if (movesToSwapSize7.find(previousMove) != movesToSwapSize7.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 8:
                // Check if leading move should be swapped
                if (movesToSwapSize8.find(previousMove) != movesToSwapSize8.end())
                {
                    gameBoard->boardSwap();
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 9:
                // Check if leading move should be swapped
                if (movesToSwapSize9.find(previousMove) != movesToSwapSize9.end())
                {
                    gameBoard->boardSwap();
                    swapPerformed = true;
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;
            case 10:
                // Check if leading move should be swapped
                if (movesToSwapSize10.find(previousMove) != movesToSwapSize10.end())
                {
                    gameBoard->boardSwap();
                    swapPerformed = true;
                    // std::cout << "Swap performed" << std::endl;
                }
                // If no swap, play as normal
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;

            case 11:
                if (movesToSwapSize11.find(previousMove) != movesToSwapSize11.end())
                {
                    gameBoard->boardSwap();
                    swapPerformed = true;
                    // std::cout << "Swap performed" << std::endl;
                }
                else
                {
                    int x, y;
                    activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                    std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
                    gameOver = result.first;
                    winner = result.second;
                    // UPdate the previous move after move is performed
                    previousMove.first = x;
                    previousMove.second = y;
                }
                break;

            default:
                std::cout << "Invalid board size for swap, cannot perform swap" << std::endl;
                break;
            }

            turnCounter += 1;
            swapActivePlayer();
        }
        else
        {
            int x, y;
            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

            std::pair<bool, int> result = gameBoard->placePiece(activePlayer->getColour(), x, y);
            gameOver = result.first;
            winner = result.second;

            turnCounter += 1;
            swapActivePlayer();

            // UPdate the previous move after move is performed
            previousMove.first = x;
            previousMove.second = y;
        }

        // Close the window if the game is over
        if (gameOver)
        {

            if (winner == -1)
            {
                std::cout << "MEGA YIKES" << std::endl;
            }

            std::ofstream outputFile;
            bool fileExists = false;
            // Check if directory exists, create if not
            if (!std::filesystem::exists("parameterTuning"))
            {
                std::filesystem::create_directories("parameterTuning");
            }

            std::string filename = "parameterTuning/" + playerOne.experimentName + "_VS_" + playerTwo.experimentName + ".csv";

            // Check if the file already exists
            std::ifstream infile(filename);
            if (infile.good())
            {
                fileExists = true;
            }

            // Open file in append mode
            outputFile.open(filename, std::ios::app);

            // If file does not exist, write player settings at top

            if (!fileExists)
            {
                outputFile << "Player 1 settings:" << std::endl;
                outputFile << playerOne << std::endl;
                outputFile << "Player 2 settings:" << std::endl;
                outputFile << playerTwo << std::endl;
                outputFile << "Played first, Swapped, Winner, Player 1 avg Sims/move, Player 2 avg Sims/move," << std::endl;
            }

            // Write the winner row
            std::string swapPerformedString = swapPerformed ? "true" : "false";
            outputFile << playedFirst << ", " << swapPerformedString << ", " << winner << ", " << players.first->getSimulationsAvg() << ", " << players.second->getSimulationsAvg() << std::endl;

            outputFile.close();

            quit = true;
            break;
        }
    }
}

void Game::swapActivePlayer()
{
    if (activePlayer->getColour() == "black")
    {
        activePlayer = players.second;
    }
    else
    {
        activePlayer = players.first;
    }
}

void Game::processOptions(MenuState options, int &size, int &boardColour, std::string &playerOneType, std::string &playerTwoType, int &minimaxDepth, double &mmTimeLimit, double &mctsTimeLimit, int &mctsIterLimit, bool &rootParallelised)
{
    switch (options.selectedBoardSize)
    {
    case BOARD_SIZE_5x5:
        size = 5;
        break;
    case BOARD_SIZE_6x6:
        size = 6;
        break;
    case BOARD_SIZE_7x7:
        size = 7;
        break;
    case BOARD_SIZE_8x8:
        size = 8;
        break;
    case BOARD_SIZE_9x9:
        size = 9;
        break;
    case BOARD_SIZE_10x10:
        size = 10;
        break;
    case BOARD_SIZE_11x11:
        size = 11;
        break;
    default:
        throw new std::logic_error("Invalid board size enum");
        break;
    }
    switch (options.selectedPlayerOneOption)
    {
    case PLAYER_ONE_HUMAN:
        playerOneType = "human";
        break;
    case PLAYER_ONE_MINIMAX:
        playerOneType = "minimax";
        break;
    case PLAYER_ONE_MCTS:
        playerOneType = "mcts";
        break;
    default:
        throw new std::logic_error("Invalid player one enum");
        break;
    }
    switch (options.selectedPlayerTwoOption)
    {
    case PLAYER_TWO_HUMAN:
        playerTwoType = "human";
        break;
    case PLAYER_TWO_MINIMAX:
        playerTwoType = "minimax";
        break;
    case PLAYER_TWO_MCTS:
        playerTwoType = "mcts";
        break;
    default:
        throw new std::logic_error("Invalid player two enum");
        break;
    }
    switch (options.selectedMinimaxDepth)
    {
    case MINIMAX_DEPTH_1:
        minimaxDepth = 1;
        break;
    case MINIMAX_DEPTH_2:
        minimaxDepth = 2;
        break;
    case MINIMAX_DEPTH_3:
        minimaxDepth = 3;
        break;
    default:
        throw new std::logic_error("Invalid minimax depth enum");
        break;
    }
    switch (options.selectedMCTSTimeLimit)
    {
    case MCTS_TIME_3_SEC:
        mctsTimeLimit = 3.0;
        break;
    case MCTS_TIME_5_SEC:
        mctsTimeLimit = 5.0;
        break;
    case MCTS_TIME_10_SEC:
        mctsTimeLimit = 10.0;
        break;
    case MCTS_TIME_30_SEC:
        mctsTimeLimit = 30.0;
        break;
    default:
        throw new std::logic_error("Invalid mcts time enum");
        break;
    }
    switch (options.selectedMCTSIterationLimit)
    {
    case MCTS_ITER_1000:
        mctsIterLimit = 1000;
        break;
    case MCTS_ITER_10000:
        mctsIterLimit = 10000;
        break;
    case MCTS_ITER_25000:
        mctsIterLimit = 25000;
        break;
    default:
        throw new std::logic_error("Invalid mcts iter enum");
        break;
    }
    switch (options.selectedMinimaxTimeLimit)
    {
    case MM_TIME_3_SEC:
        mmTimeLimit = 3.0;
        break;
    case MM_TIME_5_SEC:
        mmTimeLimit = 5.0;
        break;
    case MM_TIME_10_SEC:
        mmTimeLimit = 10.0;
        break;
    case MM_TIME_30_SEC:
        mmTimeLimit = 30.0;
        break;
    default:
        throw new std::logic_error("Invalid mmTimeLimit enum");
        break;
    }
    switch (options.selectedBoardColour)
    {
    case BLACK_WHITE:
        boardColour = 1;
        break;
    case RED_BLUE:
        boardColour = 2;
        break;
    case GREEN_PURPLE:
        boardColour = 3;
        break;
    case BLUE_ORANGE:
        boardColour = 4;
        break;
    case BLUE_YELLOW:
        boardColour = 5;
        break;
    default:
        throw new std::logic_error("Invalid colour enum");
        break;
    }
    switch (options.selectedParallelStyle)
    {
    case ROOT:
        rootParallelised = true;
        break;
    case LEAF:
        rootParallelised = false;
        break;

    default:
        throw new std::logic_error("Invalid rootPara enum");
        break;
    }
}
