#include <iostream>
#include <vector>
#include "Hexagon.cpp"
#include "src/include/SDL2/SDL.h"
#include <map>
#include <array>
#include "DisjointSet.cpp"

class Board
{
private:
    // Size of board to be played on, 11x11, 13x13 etc.
    int boardSize;

    // Number of hexes required horizontally due to offset
    int requiredWidth;

    // Matrix for storing board data
    std::vector<std::vector<int>> boardMatrix;
    

    // Matrix for storing Hexagon objects
    std::vector<Hexagon> tiles;

    // Vector for storing 2D indeces
    std::vector<std::pair<int, int>> indexMap;

    // Map containing offsets for board position for various sizes
    std::map<int, std::array<int, 2>> offsets;

    // Virtual nodes at each side of the board to detect winner
    std::pair<int, int> topNode;
    std::pair<int, int> bottomNode;
    std::pair<int, int> leftNode;
    std::pair<int, int> rightNode;

    // Vector to contain neighbouring cells for a given i,j (proof in notebook, derived from correlating hex grid to square grid)
    std::vector<std::pair<int, int>> neighbours;

    // Colours for drawings

    SDL_Color blackColour = {0, 0, 0, 255};
    SDL_Color blackColourGhost = {0, 0, 0, 100};
    SDL_Color whiteColour = {255, 255, 255, 255};
    SDL_Color whiteColourGhost = {255, 255, 255, 100};
    SDL_Color redColour = {255, 0, 0, 255};
    SDL_Color redColourGhost = {255, 0, 0, 100};
    SDL_Color blueColour = {0, 0, 255, 255};
    SDL_Color blueColourGhost = {0, 0, 255, 100};
    SDL_Color greenColour = {182, 220, 52, 255};
    SDL_Color greenColourGhost = {182, 220, 52, 100};
    SDL_Color purpleColour = {59, 21, 119, 255};
    SDL_Color purpleColourGhost = {59, 21, 119, 100};
    SDL_Color orangeColour = {255, 165, 0, 255};
    SDL_Color orangeColourGhost = {255, 165, 0, 100};
    SDL_Color lightBlueColour = {40, 92, 208, 255};
    SDL_Color lightBlueColourGhost = {40, 92, 208, 100};
    SDL_Color yellowColour = {249, 180, 42, 255};
    SDL_Color yellowColourGhost = {249, 180, 42, 100};
    SDL_Color regalBlueColour = {14, 64, 87, 255};
    SDL_Color regalBlueColourGhost = {14, 64, 87, 100};

    // Current active colours
    SDL_Color activeColours[2];
    SDL_Color activeColoursGhosts[2];

public:
    DisjointSet dsBlack;
    DisjointSet dsWhite;

    Board(int size, int ColourPairChoice);

    void createDisjointSets();

    // Show board in command line
    void showBoard();

    // Board size Getter
    int getSize();

    // Placing piece on the board
    bool placePiece(std::string player, int i, int j);

    // Populates neighbours vector with correct pairs
    void createNeighbours(int i, int j);
    

    // Return value of cell
    int getContents(int i, int j);

    // Get Hexagon object at position i,j
    Hexagon *getTileObj(int i, int j);

    // Convert i and j indeces of board matrix to index of object vector
    int getOneDIndex(int i, int j);

    // Create tiles comprising the board
    void createBoard(int screenwidth);
    // Draw the board using tiles
    void drawBoard(SDL_Renderer *Renderer, int screenwidth);
    // Draw the board borders
    void drawBoardEdges(SDL_Renderer *Renderer, int boardSize, int radius);
    // Print index map to console
    void showIndexMap();
    // Getter for index map
    std::vector<std::pair<int, int>> getIndexMap();
    // Connects placed tiles to its neighbours
    void connectToNeighbours(int i, int j, int colourCode);

    bool detectWinner();

    std::vector<std::vector<int>> getBoardMatrix();

    std::vector<std::pair<int, int>> getEmptyTiles();

    // Swap rule function
    void boardSwap(); 
};
