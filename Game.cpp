#include "Game.h"

Game::Game(int size, SDL_Handler* handler)
{
    assert(size > 0);

    

    Board *gameBoard = new Board(size, 2);

    // MCTS

    // Opponent (player or test) First
    // one.createPlayer("human", 1);
    // two.createPlayer("computer", 2, "MCTS", size, true);
    // AI First
    // one.createPlayer("computer", 1, "MCTS", size, true);
    // two.createPlayer("human", 2);

    // // Self play testing - noPP first
    // // one.createPlayer("computer", 1, "MCTS", size, false);
    // // two.createPlayer("computer", 2, "MCTS", size, true);
    // // Self play testing - PP first
    // one.createPlayer("computer", 1, "MCTS", size, true);
    // two.createPlayer("computer", 2, "MCTS", size, false);

    // Minimax
    one.createPlayer("human", 1);
    two.createPlayer("computer", 2, "minimax", size, false, 2);

    players.first = &one;
    players.second = &two;

    activePlayer = players.first;

    gameBoard->createBoard(handler->SCREEN_WIDTH);
    gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);

    bool quit = false;
    bool gameOver = false;
    int turnCounter = 0;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }

            // check the previousMove is being updated correctly
            if (turnCounter != 0 && previousMove.first == -1 && previousMove.second == -1)
            {
                throw std::logic_error("Previous move logic has fucked up");
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
                                gameOver = gameBoard->placePiece(activePlayer->getColour(), i, j);
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

                if (turnCounter == 1 && event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_p)
                    {
                        // Swap piece
                        gameBoard->boardSwap();
                        // gameBoard->showBoard();
                        gameBoard->drawBoard(handler->Renderer, handler->SCREEN_WIDTH);
                        std::cout << "Swap performed" << std::endl;
                        turnCounter += 1;
                        swapActivePlayer();
                    }
                }
            }
            else
            {
                // Swap rule implementation
                if (turnCounter == 1)
                {
                    switch (size)
                    {
                    case 2:
                        // Check if leading move should be swapped
                        if (movesToSwapSize2.find(previousMove) != movesToSwapSize2.end())
                        {
                            gameBoard->boardSwap();
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;
                    case 9:
                        // Check if leading move should be swapped
                        if (movesToSwapSize9.find(previousMove) != movesToSwapSize9.end())
                        {
                            std::vector<std::string> letters = {"a", "b", "c", "d", "e", "f", "g", "h", "i"};
                            std::cout << "Opening Move: " << letters[previousMove.first] << "" << previousMove.second << std::endl;
                            gameBoard->boardSwap();
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                            std::cout << "Swap performed" << std::endl;
                        }
                        // If no swap, play as normal
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
                            // UPdate the previous move after move is performed
                            previousMove.first = x;
                            previousMove.second = y;
                        }
                        break;

                    case 11:
                        if (movesToSwapSize11.find(previousMove) != movesToSwapSize11.end())
                        {
                            gameBoard->boardSwap();
                            std::cout << "Swap performed" << std::endl;
                        }
                        else
                        {
                            int x, y;
                            activePlayer->playMoveAI(gameBoard->getEmptyTiles(), gameBoard->getSize(), &x, &y, gameBoard->getBoardMatrix());

                            gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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

                    gameOver = gameBoard->placePiece(activePlayer->getColour(), x, y);
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
                gameBoard->showBoard();
                SDL_RenderPresent(handler->Renderer);
                std::cout << "Closing window in 2 seconds!" << std::endl;
                Sleep(2000);
                quit = true;
                break;
            }
        }
    }
}

Game::~Game()
{
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
