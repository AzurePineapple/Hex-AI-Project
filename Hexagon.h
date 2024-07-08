#include "src/include/SDL2/SDL.h"
#include <cmath>
#include <algorithm>

class Hexagon
{
private:
    const int numVertices = 6;
    double angleIncrement = 2 * M_PI / numVertices;
    int x_coord;
    int y_coord;
    std::string contents;

    SDL_Color creamColour = {255, 253, 208, 255};
    SDL_Color PieceColours[2];
    SDL_Color PieceColoursGhosts[2];

public:
    int radius;
    float BBcoords[4];

    bool ghostDrawn = false;

    Hexagon(int center_x, int center_y, SDL_Color activeColours[2], SDL_Color activeColoursGhosts[2], int size);
    ~Hexagon();

    // // Create hexagon with size(radius) = half the number of hexagons
    // //required for board size (16 for 11x11)
    // Hexagon testHex(handler->SCREEN_WIDTH / 32);

    // //https://www.redblobgames.com/grids/hexagons/ (how to work out intial positioning)

    void drawHexagon(SDL_Renderer *Renderer);

    // Render a ghost of a piece while the player hovers the tile
    void drawGhost(SDL_Renderer *Renderer, std::string playerColour);

    // Overwrite the render of the ghost
    void deleteGhost(SDL_Renderer *Renderer);
    // Set the contents of the tile if provided a colour code, elsewise it can be changed
    void setContents(int colourCode);
    // Get contents of tile
    std::string getContents();

    std::pair<int, int> getCoords();
};
