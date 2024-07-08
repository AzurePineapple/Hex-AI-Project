#include "PathBasedEvaluation.h"

PathBasedEvaluation::PathBasedEvaluation(std::vector<std::vector<int>> boardMatrix)
{
    int blackDistance = getMinDistance(boardMatrix, 1);
    int whiteDistance = getMinDistance(boardMatrix, 2);

    // Higher scores are better for black (white's route is longer, blacks route is shorter), and vice versa
    return whiteDistance - blackDistance;
}

PathBasedEvaluation::getMinDistance(std::vector<std::vector<int>> boardMatrix, int playerCode)
{
    // Create variables for tracking the distances from players first edge
    int size = boardMatrix.size();
    std::vector<std::vector<int>> distances(size, std::vector<int>(size, __INT_MAX__)); // Populate the distances vector with max ints
    std::queue<std::pair<int, int>> queue;


    // Iterate through first row/column, if 

    // If black
    if (playerCode = 1)
    {
        for (int row = 0; row < size; row++)
        {
            if (boardMatrix[row][0] == playerCode)
            {
                queue.push({row, 0});
                distances[row][0] = 0;
            }
        }
    }
    // if white
    else if (playerCode == 2)
    {
        for (int col = 0; col < size; col++)
        {
            if (boardMatrix[0][col] == playerCode)
            {
                queue.push({0, col});
                distance[0][col] = 0;
            }
        }
    }


}

PathBasedEvaluation::~PathBasedEvaluation()
{
}
