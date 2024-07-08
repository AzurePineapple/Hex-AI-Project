#include "Hexagon.h"

Hexagon::Hexagon(int center_x, int center_y, SDL_Color activeColours[2], SDL_Color activeColoursGhosts[2], int size = 0)
{
    radius = size;
    x_coord = center_x;
    y_coord = center_y;
    PieceColours[0] = activeColours[0];
    PieceColours[1] = activeColours[1];
    PieceColoursGhosts[0] = activeColoursGhosts[0];
    PieceColoursGhosts[1] = activeColoursGhosts[1];
}

Hexagon::~Hexagon()
{
}

void Hexagon::drawHexagon(SDL_Renderer *Renderer)
{

    // Create array of coordinates for bounding box
    float BBx = float(x_coord - 0.5 * radius);
    float BBy = float(y_coord - 0.5 * radius);
    BBcoords[0] = BBx;
    BBcoords[1] = BBy;
    BBcoords[2] = BBx + float(radius);
    BBcoords[3] = BBy + float(radius);

    // Vertex objects for RenderGeometry, Point Objects for borders
    SDL_Vertex vertices[numVertices];
    SDL_FPoint edgePoints[numVertices + 1];

    // Determine vertex coordinates
    for (int i = 0; i < numVertices; i++)
    {
        float x = x_coord + radius * cos(M_PI_2 + (i * angleIncrement));
        float y = y_coord + radius * sin(M_PI_2 + (i * angleIncrement));
        SDL_Vertex vertex = {{x, y}, creamColour, {1, 1}};
        vertices[i] = vertex;

        edgePoints[i] = {x, y};
    }

    // Dupe intial point to connect final edge
    edgePoints[6] = edgePoints[0];

    // Index list for which elements of the vertices are for which triangle. 0,1,2 draws a triangle between
    // these elements of vertices[]. Second triangle is drawn using vertices 2,3,0.
    int indexlist[12] = {0, 1, 2, 2, 3, 0, 3, 4, 0, 4, 5, 0};
    // Render background of hexes
    SDL_RenderGeometry(Renderer, NULL, vertices, 6, indexlist, 12);

    // Render Bounding box
    // SDL_FRect rect = {float(x_coord - 0.5 * radius), float(y_coord - 0.5 * radius), float(radius), float(radius)};
    // SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
    // SDL_RenderDrawRectF(Renderer, &rect);

    // Render borders
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderDrawLinesF(Renderer, edgePoints, numVertices + 1);

    // Draw pieces if cell contains piece
    if (contents != "empty")
    {
        SDL_Vertex pieceVertices[numVertices];
        SDL_FPoint pieceEdgePoints[numVertices + 1];

        // Determine coordinates of smaller hexagons inside tiles
        if (contents == "black")
        {
            for (int i = 0; i < numVertices; i++)
            {
                float x = x_coord + 0.8 * radius * cos(M_PI_2 + (i * angleIncrement));
                float y = y_coord + 0.8 * radius * sin(M_PI_2 + (i * angleIncrement));
                SDL_Vertex vertex = {{x, y}, PieceColours[0], {1, 1}};
                pieceVertices[i] = vertex;
                pieceEdgePoints[i] = {x, y};
            }
            pieceEdgePoints[6] = pieceEdgePoints[0];
        }
        if (contents == "white")
        {
            for (int i = 0; i < numVertices; i++)
            {
                float x = x_coord + 0.8 * radius * cos(M_PI_2 + (i * angleIncrement));
                float y = y_coord + 0.8 * radius * sin(M_PI_2 + (i * angleIncrement));
                SDL_Vertex vertex = {{x, y}, PieceColours[1], {1, 1}};
                pieceVertices[i] = vertex;
                pieceEdgePoints[i] = {x, y};
            }
            pieceEdgePoints[6] = pieceEdgePoints[0];
        }

        // Index list for which elements of the vertices are for which triangle. 0,1,2 draws a triangle between
        // these elements of vertices[]. Second triangle is drawn using vertices 2,3,0.
        int indexlist[12] = {0, 1, 2, 2, 3, 0, 3, 4, 0, 4, 5, 0};
        // Render background of hexes
        SDL_RenderGeometry(Renderer, NULL, pieceVertices, 6, indexlist, 12);

        // Render borders
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderDrawLinesF(Renderer, pieceEdgePoints, numVertices + 1);
    }
}

void Hexagon::drawGhost(SDL_Renderer *Renderer, std::string playerColour)
{
    if (contents == "empty" && ghostDrawn == false)
    {
        // Update the tile so that ghost can't be drawn
        ghostDrawn = true;

        // Draw ghost of piece to be played, same logic as above
        SDL_Vertex pieceVertices[numVertices];
        SDL_FPoint pieceEdgePoints[numVertices + 1];
        if (playerColour == "black")
        {
            for (int i = 0; i < numVertices; i++)
            {
                float x = x_coord + 0.8 * radius * cos(M_PI_2 + (i * angleIncrement));
                float y = y_coord + 0.8 * radius * sin(M_PI_2 + (i * angleIncrement));
                SDL_Vertex vertex = {{x, y}, PieceColoursGhosts[0], {1, 1}};
                pieceVertices[i] = vertex;
                pieceEdgePoints[i] = {x, y};
            }
            pieceEdgePoints[6] = pieceEdgePoints[0];
        }

        else if (playerColour == "white")
        {

            for (int i = 0; i < numVertices; i++)
            {
                float x = x_coord + 0.8 * radius * cos(M_PI_2 + (i * angleIncrement));
                float y = y_coord + 0.8 * radius * sin(M_PI_2 + (i * angleIncrement));
                SDL_Vertex vertex = {{x, y}, PieceColoursGhosts[1], {1, 1}};
                pieceVertices[i] = vertex;
                pieceEdgePoints[i] = {x, y};
            }
            pieceEdgePoints[6] = pieceEdgePoints[0];
        }

        int indexlist[12] = {0, 1, 2, 2, 3, 0, 3, 4, 0, 4, 5, 0};
        // Render background of hexes
        SDL_RenderGeometry(Renderer, NULL, pieceVertices, 6, indexlist, 12);

        // Render borders

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 100);
        SDL_RenderDrawLinesF(Renderer, pieceEdgePoints, numVertices + 1);
    }
}

void Hexagon::deleteGhost(SDL_Renderer *Renderer)
{
    if (ghostDrawn && contents == "empty")
    {
        ghostDrawn = false;
        // Vertex objects for RenderGeometry, Point Objects for borders
        SDL_Vertex vertices[numVertices];
        SDL_FPoint edgePoints[numVertices + 1];

        // Determine vertex coordinates
        for (int i = 0; i < numVertices; i++)
        {
            float x = x_coord + radius * cos(M_PI_2 + (i * angleIncrement));
            float y = y_coord + radius * sin(M_PI_2 + (i * angleIncrement));
            SDL_Vertex vertex = {{x, y}, creamColour, {1, 1}};
            vertices[i] = vertex;

            edgePoints[i] = {x, y};
        }

        // Dupe intial point to connect final edge
        edgePoints[6] = edgePoints[0];

        // Index list for which elements of the vertices are for which triangle. 0,1,2 draws a triangle between
        // these elements of vertices[]. Second triangle is drawn using vertices 2,3,0.
        int indexlist[12] = {0, 1, 2, 2, 3, 0, 3, 4, 0, 4, 5, 0};
        // Render background of hexes
        SDL_RenderGeometry(Renderer, NULL, vertices, 6, indexlist, 12);

        // Render borders
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderDrawLinesF(Renderer, edgePoints, numVertices + 1);
    }
}

void Hexagon::setContents(int colourCode)
{
    switch (colourCode)
    {
    case 1:
        contents = "black";
        break;

    case 2:
        contents = "white";
        break;

    default:
        contents = "empty";
        break;
    }
}

std::string Hexagon::getContents()
{
    return contents;
}

std::pair<int, int> Hexagon::getCoords()
{
    return std::make_pair(x_coord, y_coord);
}
