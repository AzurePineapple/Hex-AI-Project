#include "ResistanceDistance.h"
using Eigen::MatrixXd;
#include <chrono>

ResistanceDistance::ResistanceDistance()
{
}

ResistanceDistance::~ResistanceDistance()
{
}

float ResistanceDistance::evaluate(const std::vector<std::vector<int>>& boardMatrix, double bridgeWeight)
{
    for (size_t i = 0; i < boardMatrix.size(); i++)
    {
        for (size_t j = 0; j < boardMatrix.size(); j++)
        {
            indexMap.push_back({i, j});
        }
    }

    createAdjMat(boardMatrix, bridgeWeight);

    createLaplacian();

    return getResistanceDistance();
}

// Returns the 1D index of a search pair
int ResistanceDistance::getOneDIndex(int i, int j)
{
    std::pair<int, int> searchPair = {i, j};
    auto iterator = std::find(indexMap.begin(), indexMap.end(), searchPair);
    if (iterator != indexMap.end())
    {
        int index = iterator - indexMap.begin();
        return index;
    }
    else
    {
        return -1;
    }
}

void ResistanceDistance::createAdjMat(std::vector<std::vector<int>> boardMatrix, double bridgeWeight)
{
    int boardSize = boardMatrix.size();

    // Player one
    AdjMat.resize(int(pow(boardSize, 2)) + 2, int(pow(boardSize, 2)) + 2);
    AdjMat.setZero();

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (boardMatrix[i][j] != 2)
            {
                // Create vector of neighbour coordinates
                neighbours.clear();
                neighbours.push_back(std::make_pair(i - 1, j));
                neighbours.push_back(std::make_pair(i - 1, j + 1));
                neighbours.push_back(std::make_pair(i, j - 1));
                neighbours.push_back(std::make_pair(i, j + 1));
                neighbours.push_back(std::make_pair(i + 1, j - 1));
                neighbours.push_back(std::make_pair(i + 1, j));

                // Iterate through neighbours of the cell
                for (auto &&neiPair : neighbours)
                {
                    // Only perform actions on valid cells
                    if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
                    {
                        // Edge weight holder
                        int AdjValue = 0;

                        // Check the value at the first node to the edge weight
                        switch (boardMatrix[i][j])
                        {
                        case 0:
                            AdjValue += 2;
                            break;
                        case 1:
                            AdjValue += 1;
                            break;
                        case 2:
                            AdjValue += 10000;
                            break;
                        }
                        // Check the value at the second node to the edge weight
                        switch (boardMatrix[neiPair.first][neiPair.second])
                        {
                        case 0:
                            AdjValue += 2;
                            break;
                        case 1:
                            AdjValue += 1;
                            break;
                        case 2:
                            AdjValue += 10000;
                            break;
                        }

                        AdjMat(getOneDIndex(neiPair.first, neiPair.second) + 1, getOneDIndex(i, j) + 1) = AdjValue;
                        AdjMat(getOneDIndex(i, j) + 1, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
                    }
                }

                // Add to the bridge neighbours vector if required neighbours that make up the bridge are empty
                bridgeNeighbours.clear();
                // Checks if tiles are valid first, && is a short circuit operator, the second term isn't evaluated if the first is false
                if (validateTile({i + 1, j - 1}, boardSize) && validateTile({i + 1, j}, boardSize) && boardMatrix[i + 1][j - 1] == 0 && boardMatrix[i + 1][j] == 0)
                {
                    bridgeNeighbours.push_back({i + 2, j - 1});
                }
                if (validateTile({i + 1, j}, boardSize) && validateTile({i, j + 1}, boardSize) && boardMatrix[i + 1][j] == 0 && boardMatrix[i][j + 1] == 0)
                {
                    bridgeNeighbours.push_back({i + 1, j + 1});
                }
                if (validateTile({i, j + 1}, boardSize) && validateTile({i - 1, j + 1}, boardSize) && boardMatrix[i][j + 1] == 0 && boardMatrix[i - 1][j + 1] == 0)
                {
                    bridgeNeighbours.push_back({i - 1, j + 2});
                }
                if (validateTile({i - 1, j + 1}, boardSize) && validateTile({i - 1, j}, boardSize) && boardMatrix[i - 1][j + 1] == 0 && boardMatrix[i - 1][j] == 0)
                {
                    bridgeNeighbours.push_back({i - 2, j + 1});
                }
                if (validateTile({i - 1, j}, boardSize) && validateTile({i, j - 1}, boardSize) && boardMatrix[i - 1][j] == 0 && boardMatrix[i][j - 1] == 0)
                {
                    bridgeNeighbours.push_back({i - 1, j - 1});
                }
                if (validateTile({i, j - 1}, boardSize) && validateTile({i + 1, j - 1}, boardSize) && boardMatrix[i][j - 1] == 0 && boardMatrix[i + 1][j - 1] == 0)
                {
                    bridgeNeighbours.push_back({i + 1, j - 2});
                }

                for (auto &&bridge : bridgeNeighbours)
                {
                    // Only perform actions on valid cells
                    if (0 <= bridge.first && bridge.first < boardSize && 0 <= bridge.second && bridge.second < boardSize)
                    {
                        // Check if both the cell and the bridge cell have a friendly tile in them
                        if (boardMatrix[i][j] == 1 && boardMatrix[bridge.first][bridge.second] == 1)
                        {
                            // Value set higher than 2 (resistance between two adjacent pieces), but lower than 3 (resistance between a friendly piece and an empty cell). This makes a two bridge still a good connection, but not as good as an immediate neighbour, as they can still be broken
                            AdjMat(getOneDIndex(bridge.first, bridge.second) + 1, getOneDIndex(i, j) + 1) = bridgeWeight;
                            AdjMat(getOneDIndex(i, j) + 1, getOneDIndex(bridge.first, bridge.second) + 1) = bridgeWeight;
                        }
                    }
                }
            }
        }
    }

    // Iterate over column space of board matrix, setting adjacency for permanent edge nodes
    // Player one
    for (int j = 0; j < boardSize; j++)
    {
        // Top Row node now has value = 1
        int AdjValue = 1;

        switch (boardMatrix[0][j])
        {
        case 0:
            AdjValue += 2;
            break;

        case 1:
            AdjValue += 1;
            break;
        case 2:
            AdjValue += 10000;
            break;
        }
        AdjMat(0, getOneDIndex(0, j) + 1) = AdjValue;
        AdjMat(getOneDIndex(0, j) + 1, 0) = AdjValue;

        // Bottom Row
        AdjValue = 1;

        switch (boardMatrix[boardSize - 1][j])
        {
        case 0:
            AdjValue += 2;
            break;

        case 1:
            AdjValue += 1;
            break;
        case 2:
            AdjValue += 10000;
            break;
        }
        AdjMat(AdjMat.rows() - 1, getOneDIndex(boardSize - 1, j) + 1) = AdjValue;
        AdjMat(getOneDIndex(boardSize - 1, j) + 1, AdjMat.rows() - 1) = AdjValue;
    }

    // Player two

    AdjMat_two.resize(int(pow(boardSize, 2)) + 2, int(pow(boardSize, 2)) + 2);
    AdjMat_two.setZero();

    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (boardMatrix[i][j] != 1)
            {
                // Create vector of neighbour coordinates
                neighbours.clear();
                neighbours.push_back(std::make_pair(i - 1, j));
                neighbours.push_back(std::make_pair(i - 1, j + 1));
                neighbours.push_back(std::make_pair(i, j - 1));
                neighbours.push_back(std::make_pair(i, j + 1));
                neighbours.push_back(std::make_pair(i + 1, j - 1));
                neighbours.push_back(std::make_pair(i + 1, j));

                // Iterate through neighbours of the cell
                for (auto &&neiPair : neighbours)
                {
                    // Only perform actions on valid cells
                    if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
                    {
                        // Edge weight holder
                        int AdjValue = 0;

                        // Check the value at the first node to the edge weight
                        switch (boardMatrix[i][j])
                        {
                        case 0:
                            AdjValue += 2;
                            break;
                        case 1:
                            AdjValue += 10000;
                            break;
                        case 2:
                            AdjValue += 1;
                            break;
                        }
                        // Check the value at the second node to the edge weight
                        switch (boardMatrix[neiPair.first][neiPair.second])
                        {
                        case 0:
                            AdjValue += 2;
                            break;
                        case 1:
                            AdjValue += 10000;
                            break;
                        case 2:
                            AdjValue += 1;
                            break;
                        }

                        AdjMat_two(getOneDIndex(neiPair.first, neiPair.second) + 1, getOneDIndex(i, j) + 1) = AdjValue;
                        AdjMat_two(getOneDIndex(i, j) + 1, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
                    }
                }

                // Add to the bridge neighbours vector if required neighbours that make up the bridge are empty
                bridgeNeighbours.clear();
                if (validateTile({i + 1, j - 1}, boardSize) && validateTile({i + 1, j}, boardSize) && boardMatrix[i + 1][j - 1] == 0 && boardMatrix[i + 1][j] == 0)
                {
                    bridgeNeighbours.push_back({i + 2, j - 1});
                }
                if (validateTile({i + 1, j}, boardSize) && validateTile({i, j + 1}, boardSize) && boardMatrix[i + 1][j] == 0 && boardMatrix[i][j + 1] == 0)
                {
                    bridgeNeighbours.push_back({i + 1, j + 1});
                }
                if (validateTile({i, j + 1}, boardSize) && validateTile({i - 1, j + 1}, boardSize) && boardMatrix[i][j + 1] == 0 && boardMatrix[i - 1][j + 1] == 0)
                {
                    bridgeNeighbours.push_back({i - 1, j + 2});
                }
                if (validateTile({i - 1, j + 1}, boardSize) && validateTile({i - 1, j}, boardSize) && boardMatrix[i - 1][j + 1] == 0 && boardMatrix[i - 1][j] == 0)
                {
                    bridgeNeighbours.push_back({i - 2, j + 1});
                }
                if (validateTile({i - 1, j}, boardSize) && validateTile({i, j - 1}, boardSize) && boardMatrix[i - 1][j] == 0 && boardMatrix[i][j - 1] == 0)
                {
                    bridgeNeighbours.push_back({i - 1, j - 1});
                }
                if (validateTile({i, j - 1}, boardSize) && validateTile({i + 1, j - 1}, boardSize) && boardMatrix[i][j - 1] == 0 && boardMatrix[i + 1][j - 1] == 0)
                {
                    bridgeNeighbours.push_back({i + 1, j - 2});
                }

                for (auto &&bridge : bridgeNeighbours)
                {
                    // Only perform actions on valid cells
                    if (0 <= bridge.first && bridge.first < boardSize && 0 <= bridge.second && bridge.second < boardSize)
                    {
                        // Check if both the cell and the bridge cell have a friendly tile in them
                        if (boardMatrix[i][j] == 2 && boardMatrix[bridge.first][bridge.second] == 2)
                        {
                            // Value set higher than 2 (resistance between two adjacent pieces), but lower than 3 (resistance between a friendly piece and an empty cell). This makes a two bridge still a good connection, but not as good as an immediate neighbour, as they can still be broken
                            AdjMat_two(getOneDIndex(bridge.first, bridge.second) + 1, getOneDIndex(i, j) + 1) = bridgeWeight;
                            AdjMat_two(getOneDIndex(i, j) + 1, getOneDIndex(bridge.first, bridge.second) + 1) = bridgeWeight;
                        }
                    }
                }
            }
        }
    }

    // Iterate over row space of board matrix, setting adjacency for permanent edge nodes
    // Player two
    for (int i = 0; i < boardSize; i++)
    {
        // Top Row node now has value = 1
        int AdjValue = 1;

        switch (boardMatrix[i][0])
        {
        case 0:
            AdjValue += 2;
            break;
        case 1:
            AdjValue += 10000;
            break;
        case 2:
            AdjValue += 1;
            break;
        }
        AdjMat_two(0, getOneDIndex(i, 0) + 1) = AdjValue;
        AdjMat_two(getOneDIndex(i, 0) + 1, 0) = AdjValue;

        // Bottom Row
        AdjValue = 1;

        switch (boardMatrix[i][boardSize - 1])
        {
        case 0:
            AdjValue += 2;
            break;

        case 1:
            AdjValue += 10000;
            break;
        case 2:
            AdjValue += 1;
            break;
        }
        AdjMat_two(AdjMat_two.rows() - 1, getOneDIndex(i, boardSize - 1) + 1) = AdjValue;
        AdjMat_two(getOneDIndex(i, boardSize - 1) + 1, AdjMat_two.rows() - 1) = AdjValue;
    }

    // Set flag so update can be called in the future, saving computation time
    AdjMatInitialised = true;
}

void ResistanceDistance::createLaplacian()
{
    // Black's Laplacian
    LaplacianMatrix.resize(AdjMat.rows(), AdjMat.cols());

    for (int row = 0; row < AdjMat.rows(); row++)
    {
        for (int col = 0; col < AdjMat.cols(); col++)
        {
            // Skip diagonal entries
            if (row != col)
            {
                int AdjMatEntry = AdjMat(row, col);
                // If no connection in AdjMat, set Laplacian entry to 0
                if (AdjMatEntry == 0)
                {
                    LaplacianMatrix(row, col) = 0;
                }
                // Otherwise set to -1/w(i,j) according to Bapat and Weiss
                else
                {
                    LaplacianMatrix(row, col) = -1.0 / AdjMatEntry;
                }
            }
        }
    }

    for (int i = 0; i < LaplacianMatrix.rows(); i++)
    {
        double rowSum = LaplacianMatrix.row(i).sum() - LaplacianMatrix(i, i);
        LaplacianMatrix(i, i) = -rowSum;
    }

    // Whites's Laplacian
    MatrixXd Laplacian2;
    LaplacianMatrix_two.resize(AdjMat_two.rows(), AdjMat_two.cols());

    for (int row = 0; row < AdjMat_two.rows(); row++)
    {
        for (int col = 0; col < AdjMat_two.cols(); col++)
        {
            // Skip diagonal entries
            if (row != col)
            {
                int AdjMatEntry = AdjMat_two(row, col);
                // If no connection in AdjMat, set Laplacian entry to 0
                if (AdjMatEntry == 0)
                {
                    LaplacianMatrix_two(row, col) = 0;
                }
                // Otherwise set to -1/w(i,j) according to Bapat and Weiss
                else
                {
                    LaplacianMatrix_two(row, col) = -1.0 / AdjMatEntry;
                }
            }
        }
    }

    for (int i = 0; i < LaplacianMatrix_two.rows(); i++)
    {
        double rowSum = LaplacianMatrix_two.row(i).sum() - LaplacianMatrix_two(i, i);
        LaplacianMatrix_two(i, i) = -rowSum;
    }
}

// Alternative method for calculating the resistance distance given in "A Simple Method for Computing Resistance Distance by Ravindra B. Bapat, Ivan Gutmana,b, and Wenjun Xiaob"
float ResistanceDistance::getResistanceDistance()
{
    // To use the formula from the literature we need L_i and L_ij, which is the Laplacian with the ith columns/rows and the
    // Laplacian with the ith and jth columns/rows removed respectively

    // Create vector of column integers to keep in the submatrix
    std::vector<int> columnsToKeep;
    for (int i = 0; i < LaplacianMatrix.cols(); i++)
    {
        columnsToKeep.push_back(i);
    }

    // Remove the first integer to remove the first column, creating L_i for both players
    columnsToKeep.erase(columnsToKeep.begin());
    MatrixXd L_i = LaplacianMatrix(columnsToKeep, columnsToKeep);
    MatrixXd L2_i = LaplacianMatrix_two(columnsToKeep, columnsToKeep);
    // Next remove the last, to create L_ij for both players
    columnsToKeep.erase(columnsToKeep.end() - 1);
    MatrixXd L_ij = LaplacianMatrix(columnsToKeep, columnsToKeep);
    MatrixXd L2_ij = LaplacianMatrix_two(columnsToKeep, columnsToKeep);

    // Old sequential method, very slow ~30,000microseconds on 11x11 board
    // float det_L_i = L_i.determinant();
    // float det_L_ij = L_ij.determinant();
    // float det_L2_i = L2_i.determinant();
    // float det_L2_ij = L2_ij.determinant();

    // New parallel processing method, performs the four calculations simultaneously.
    float det_L_i, det_L_ij, det_L2_i, det_L2_ij;
#pragma omp parallel sections
    {
        // Player one
        // Compute determinants
#pragma omp section
        {
            det_L_i = L_i.determinant();
        }
#pragma omp section
        {
            det_L_ij = L_ij.determinant();
        }
        // Player two
        // Compute determinants
#pragma omp section
        {
            det_L2_i = L2_i.determinant();
        }
#pragma omp section
        {
            det_L2_ij = L2_ij.determinant();
        }
    }

    // If det_L_i is 0, it means there are no spanning trees for the graph, which means that the graph has been cut in two by the opponent. So we need to return the corresponding board score remembering that white is MAX and black is MIN
    if (det_L_i == 0)
    {
        // This is blacks graph, so if it has 0 spanning trees then white has won, so return +inf
        return std::numeric_limits<float>::infinity();
    }
    if (det_L2_i == 0)
    {
        // Vice Versa
        return -std::numeric_limits<float>::infinity();
    }

    // Compute resistance distance
    float r_ij = det_L_ij / det_L_i;
    // Compute resistance distance
    float r2_ij = det_L2_ij / det_L2_i;

    float boardScore = log(r_ij / r2_ij);
    return boardScore;
}

bool ResistanceDistance::validateTile(std::pair<int, int> cell, int boardSize)
{
    return (0 <= cell.first && cell.first < boardSize && 0 <= cell.second && cell.second < boardSize);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Redundant function
float ResistanceDistance::evaluateMove(std::vector<std::vector<int>> boardMatrix, int i, int j)
{
    int boardSize = boardMatrix.size();
    if (!((0 <= i && i < boardSize) && (0 <= j && j < boardSize)))
    {
        throw std::invalid_argument("Received false coordinates");
    }

    updateAdjMatNew(boardMatrix, i, j);

    updateLaplacianNew(boardMatrix, i, j);

    float a = getResistanceDistance_board();

    return a;
}

// Returns the 1D index of a search pair in the reduced index map
int ResistanceDistance::getOneDIndex_Reduced(int i, int j)
{
    std::pair<int, int> searchPair = {i, j};
    auto iterator = std::find(IndexMap_one.begin(), IndexMap_one.end(), searchPair);
    if (iterator != IndexMap_one.end())
    {
        int index = iterator - IndexMap_one.begin();
        return index;
    }
    else
    {
        return -1;
    }
}

// Redundant function
// Creates two adjacency matrices for a given board matrix, one per player
void ResistanceDistance::createAdjMatOld(std::vector<std::vector<int>> boardMatrix)
{
    int boardSize = boardMatrix.size();
    // Create adjacency matrices for each player

    // Player one
    AdjMat.resize(int(pow(boardSize, 2)) + 2, int(pow(boardSize, 2)) + 2);
    // Create adjacency matrix of correct size (plus two in each direction for permanent edge nodes) and populate it with -1 values

    for (int i = 0; i < AdjMat.rows(); i++)
    {
        for (int j = 0; j < AdjMat.cols(); j++)
        {
            AdjMat(i, j) = -1;
        }
    }

    // Player two
    AdjMat_two.resize(int(pow(boardSize, 2)) + 2, int(pow(boardSize, 2)) + 2);
    // Create adjacency matrix of correct size (plus two in each direction for permanent edge nodes) and populate it with -1 values

    for (int i = 0; i < AdjMat_two.rows(); i++)
    {
        for (int j = 0; j < AdjMat_two.cols(); j++)
        {
            AdjMat_two(i, j) = -1;
        }
    }

    // Iterate through board and set AdjMat values to correct weights between edges - only between board cells, permanent edge nodes come later (this worked on the first try and I don't want to touch it)

    // Player one
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (boardMatrix[i][j] != 2)
            {
                // Create vector of neighbour coordinates
                neighbours.clear();
                neighbours.push_back(std::make_pair(i - 1, j));
                neighbours.push_back(std::make_pair(i - 1, j + 1));
                neighbours.push_back(std::make_pair(i, j - 1));
                neighbours.push_back(std::make_pair(i, j + 1));
                neighbours.push_back(std::make_pair(i + 1, j - 1));
                neighbours.push_back(std::make_pair(i + 1, j));

                // Iterate through neighbours of the cell
                for (auto &&neiPair : neighbours)
                {
                    // Only perform actions on valid cells
                    if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
                    {
                        // Check if neighbour is not occupied by a foe, if so, leave value as -1
                        if (boardMatrix[neiPair.first][neiPair.second] != 2)
                        {
                            // Edge weight holder
                            int AdjValue = 0;

                            // Check the value at the first node to the edge weight
                            switch (boardMatrix[i][j])
                            {
                            case 0:
                                AdjValue += 1;
                                break;

                            case 1:
                                AdjValue += 0;
                                break;
                            }
                            // Check the value at the second node to the edge weight
                            switch (boardMatrix[neiPair.first][neiPair.second])
                            {
                            case 0:
                                AdjValue += 1;
                                break;

                            case 1:
                                AdjValue += 0;
                                break;
                            }

                            AdjMat(getOneDIndex(neiPair.first, neiPair.second) + 1, getOneDIndex(i, j) + 1) = AdjValue;
                            AdjMat(getOneDIndex(i, j) + 1, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
                        }
                    }
                }
            }
        }
    }

    // Player two
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            if (boardMatrix[i][j] != 1)
            {
                // Create vector of neighbour coordinates
                neighbours.clear();
                neighbours.push_back(std::make_pair(i - 1, j));
                neighbours.push_back(std::make_pair(i - 1, j + 1));
                neighbours.push_back(std::make_pair(i, j - 1));
                neighbours.push_back(std::make_pair(i, j + 1));
                neighbours.push_back(std::make_pair(i + 1, j - 1));
                neighbours.push_back(std::make_pair(i + 1, j));

                // Iterate through neighbours of the cell
                for (auto &&neiPair : neighbours)
                {
                    // Only perform actions on valid cells
                    if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
                    {
                        // Check if neighbour is not occupied by a foe
                        if (boardMatrix[neiPair.first][neiPair.second] != 1)
                        {
                            // Edge weight holder
                            int AdjValue = 0;

                            // Check the value at the first node to the edge weight
                            switch (boardMatrix[i][j])
                            {
                            case 0:
                                AdjValue += 1;
                                break;

                            case 2:
                                AdjValue += 0;
                                break;
                            }
                            // Check the value at the second node to the edge weight
                            switch (boardMatrix[neiPair.first][neiPair.second])
                            {
                            case 0:
                                AdjValue += 1;
                                break;

                            case 2:
                                AdjValue += 0;
                                break;
                            }

                            AdjMat_two(getOneDIndex(neiPair.first, neiPair.second) + 1, getOneDIndex(i, j) + 1) = AdjValue;
                            AdjMat_two(getOneDIndex(i, j) + 1, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
                        }
                    }
                }
            }
        }
    }

    // Iterate over column space of board matrix, setting adjacency for permanent edge nodes

    // Player one
    for (int j = 0; j < boardSize; j++)
    {
        int AdjValue = 0;
        if (boardMatrix[0][j] != 2)
        {
            switch (boardMatrix[0][j])
            {
            case 0:
                AdjValue += 1;
                break;

            case 1:
                AdjValue += 0;
                break;
            }
            AdjMat(0, getOneDIndex(0, j) + 1) = AdjValue;
            AdjMat(getOneDIndex(0, j) + 1, 0) = AdjValue;
            AdjValue = 0;
        }

        if (boardMatrix[boardSize - 1][j] != 2)
        {
            switch (boardMatrix[boardSize - 1][j])
            {
            case 0:
                AdjValue += 1;
                break;

            case 1:
                AdjValue += 0;
                break;
            }
            AdjMat(AdjMat.rows() - 1, getOneDIndex(boardSize - 1, j) + 1) = AdjValue;
            AdjMat(getOneDIndex(boardSize - 1, j) + 1, AdjMat.rows() - 1) = AdjValue;
        }
    }

    // Player two, using row space this time
    for (int i = 0; i < boardSize; i++)
    {
        int AdjValue = 0;
        if (boardMatrix[i][0] != 1)
        {
            switch (boardMatrix[i][0])
            {
            case 0:
                AdjValue += 1;
                break;

            case 2:
                AdjValue += 0;
                break;
            }
            AdjMat_two(0, getOneDIndex(i, 0) + 1) = AdjValue;
            AdjMat_two(getOneDIndex(i, 0) + 1, 0) = AdjValue;
            AdjValue = 0;
        }

        if (boardMatrix[i][boardSize - 1] != 1)
        {
            switch (boardMatrix[i][boardSize - 1])
            {
            case 0:
                AdjValue += 1;
                break;

            case 2:
                AdjValue += 0;
                break;
            }
            AdjMat_two(AdjMat_two.rows() - 1, getOneDIndex(i, boardSize - 1) + 1) = AdjValue;
            AdjMat_two(getOneDIndex(i, boardSize - 1) + 1, AdjMat_two.rows() - 1) = AdjValue;
        }
    }

    AdjMatInitialised = true;
}

// Redundant
// Removes columns and rows from the adjacency matrix so that it represents only the connected portion of the graph
void ResistanceDistance::reduceAdjMat()
{
    // Player one
    emptyColIndeces.clear();

    // Iterate over the columns of AdjMat
    for (int j = 0; j < AdjMat.cols(); j++)
    {
        bool emptyCol = true;

        // Iterate through column entries
        for (int i = 0; i < AdjMat.rows(); i++)
        {
            // If any entry is non-empty, set the bool to false
            if (AdjMat.col(j)[i] != -1)
            {
                emptyCol = false;
            }
        }

        // Add empty column indeces to vector
        if (emptyCol)
        {
            emptyColIndeces.push_back(j);
        }
    }

    std::vector<int> colsToKeep;
    for (int i = 0; i < AdjMat.cols(); i++)
    {
        if (!(std::count(emptyColIndeces.begin(), emptyColIndeces.end(), i)))
        {
            colsToKeep.push_back(i);
        }
    }

    for (auto &&i : colsToKeep)
    {
        if (i != colsToKeep[0] && i != colsToKeep[colsToKeep.size() - 1])
        {
            IndexMap_one.push_back(indexMap[i - 1]);
        }
    }

    RedAdjMat = AdjMat(colsToKeep, colsToKeep);

    // Player two
    emptyColIndeces.clear();

    // Iterate over the columns of AdjMat
    for (int j = 0; j < AdjMat_two.cols(); j++)
    {
        bool emptyCol = true;

        // Iterate through column entries
        for (int i = 0; i < AdjMat_two.rows(); i++)
        {
            // If any entry is non-empty, set the bool to false
            if (AdjMat_two.col(j)[i] != -1)
            {
                emptyCol = false;
            }
        }

        // Add empty column indeces to vector
        if (emptyCol)
        {
            emptyColIndeces.push_back(j);
        }
    }

    colsToKeep.clear();
    for (int i = 0; i < AdjMat_two.cols(); i++)
    {
        if (!(std::count(emptyColIndeces.begin(), emptyColIndeces.end(), i)))
        {
            colsToKeep.push_back(i);
        }
    }

    for (auto &&i : colsToKeep)
    {
        if (i != colsToKeep[0] && i != colsToKeep[colsToKeep.size() - 1])
        {
            IndexMap_two.push_back(indexMap[i - 1]);
        }
    }

    RedAdjMat_two = AdjMat_two(colsToKeep, colsToKeep);
}

// Redundant function, was used for testing
int ResistanceDistance::getResistanceDistance_tiles(std::pair<int, int> tileA, std::pair<int, int> tileB)
{
    MatrixXd Linv = LaplacianMatrix.completeOrthogonalDecomposition().pseudoInverse();

    // int A = 0;
    // int B = LaplacianMatrix.cols() - 1;

    if (getOneDIndex_Reduced(tileA.first, tileA.second) == -1 || getOneDIndex_Reduced(tileB.first, tileB.second) == -1)
    {
        std::cout << "Tiles cannot be connected, one or both of them is occupied by a foe" << std::endl;
        return 0;
    }
    else
    {
        int A = getOneDIndex_Reduced(tileA.first, tileA.second) + 1;
        int B = getOneDIndex_Reduced(tileB.first, tileB.second) + 1;

        std::cout << "Resistance distance between (" << tileA.first << ", " << tileA.second << ") and (" << tileB.first << ", " << tileB.second << ") is:" << std::endl;
        std::cout << Linv(A, A) + Linv(B, B) - Linv(A, B) - Linv(B, A) << std::endl;

        return 0;
    }
}

// Redundant
void ResistanceDistance::createLaplacianOld()
{

    // Player one
    LaplacianMatrix.resize(AdjMat.rows(), AdjMat.cols());

    double zero = 1e-10;

    for (int row = 0; row < AdjMat.rows(); row++)
    {
        for (int col = 0; col < AdjMat.cols(); col++)
        {
            switch (int(AdjMat(row, col)))
            {
            case -1:
                LaplacianMatrix(row, col) = 0;
                break;
            case 0:
                LaplacianMatrix(row, col) = -1 / zero;
                break;
            case 1:
                LaplacianMatrix(row, col) = -1 / 1;
                break;
            case 2:
                LaplacianMatrix(row, col) = -1 / 2;
                break;
            default:
                LaplacianMatrix(row, col) = 0;
                break;
            }
        }
    }

    for (int row = 0; row < LaplacianMatrix.rows(); row++)
    {
        double rowSum = 0.0;
        for (int col = 0; col < LaplacianMatrix.cols(); col++)
        {
            if (col != row)
            {
                rowSum += LaplacianMatrix(row, col);
            }
        }
        LaplacianMatrix(row, row) = -rowSum;
    }

    // Player two
    LaplacianMatrix_two.resize(AdjMat_two.rows(), AdjMat_two.cols());

    // double zero = 1e-10;

    for (int row = 0; row < AdjMat_two.rows(); row++)
    {
        for (int col = 0; col < AdjMat_two.cols(); col++)
        {
            switch (int(AdjMat_two(row, col)))
            {
            case -1:
                LaplacianMatrix_two(row, col) = 0;
                break;
            case 0:
                LaplacianMatrix_two(row, col) = -1 / zero;
                break;
            case 1:
                LaplacianMatrix_two(row, col) = -1 / 1;
                break;
            case 2:
                LaplacianMatrix_two(row, col) = -1 / 2;
                break;
            default:
                LaplacianMatrix_two(row, col) = 0;
                break;
            }
        }
    }

    for (int row = 0; row < LaplacianMatrix_two.rows(); row++)
    {
        double rowSum = 0.0;
        for (int col = 0; col < LaplacianMatrix_two.cols(); col++)
        {
            if (col != row)
            {
                rowSum += LaplacianMatrix_two(row, col);
            }
        }
        LaplacianMatrix_two(row, row) = -rowSum;
    }
}

// Initalise the Laplacian with 0 values - Redundant
void ResistanceDistance::initialiseLaplacian()
{
    LaplacianMatrix = Eigen::MatrixXd::Zero(AdjMat.rows(), AdjMat.cols());
    LaplacianMatrix_two = Eigen::MatrixXd::Zero(AdjMat_two.rows(), AdjMat_two.cols());

    return;
}

void ResistanceDistance::updateAdjMat(std::vector<std::vector<int>> boardMatrix, int x, int y)
{
    int boardSize = boardMatrix.size();
    // Get the correct column to update from move coordinates
    int columnToUpdate = getOneDIndex(x, y) + 1;

    // Create vector of neighbour coordinates
    neighbours.clear();
    neighbours.push_back(std::make_pair(x - 1, y));
    neighbours.push_back(std::make_pair(x - 1, y + 1));
    neighbours.push_back(std::make_pair(x, y - 1));
    neighbours.push_back(std::make_pair(x, y + 1));
    neighbours.push_back(std::make_pair(x + 1, y - 1));
    neighbours.push_back(std::make_pair(x + 1, y));

    // See who just played
    switch (boardMatrix[x][y])
    {
    // Player 1 just played
    case 1:
    {
        // Set AdjMat row/columns to -1 for opponent of whomever just played
        for (int i = 0; i < AdjMat_two.cols(); i++)
        {
            AdjMat_two(columnToUpdate, i) = -1;
            AdjMat_two(i, columnToUpdate) = -1;
        }

        // Update player 1's Adj Mat
        // Iterate through neighbours of the cell
        for (auto &&neiPair : neighbours)
        {
            // Only perform actions on valid cells
            if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
            {
                // Check if neighbour is not occupied by a foe, if so, leave value as -1
                if (boardMatrix[neiPair.first][neiPair.second] != 2)
                {
                    // Edge weight holder, set to 0 because tile definitely has a value as per switch statement above
                    int AdjValue = 0;

                    // Check the value at the second node to the edge weight
                    switch (boardMatrix[neiPair.first][neiPair.second])
                    {
                    case 0:
                        AdjValue += 1;
                        break;

                    case 1:
                        AdjValue += 0;
                        break;
                    }

                    AdjMat(getOneDIndex(neiPair.first, neiPair.second) + 1, columnToUpdate) = AdjValue;
                    AdjMat(columnToUpdate, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
                }
            }
        }
    }
    break;

    case 2:
        // Set AdjMat row/columns to -1 for opponent of whomever just played
        for (int i = 0; i < AdjMat.cols(); i++)
        {
            AdjMat(columnToUpdate, i) = -1;
            AdjMat(i, columnToUpdate) = -1;
        }
        // Iterate through neighbours of the cell
        for (auto &&neiPair : neighbours)
        {
            // Only perform actions on valid cells
            if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
            {
                // Check if neighbour is not occupied by a foe
                if (boardMatrix[neiPair.first][neiPair.second] != 1)
                {
                    // Edge weight holder
                    int AdjValue = 0;

                    // Check the value at the second node to the edge weight
                    switch (boardMatrix[neiPair.first][neiPair.second])
                    {
                    case 0:
                        AdjValue += 1;
                        break;

                    case 2:
                        AdjValue += 0;
                        break;
                    }

                    AdjMat_two(getOneDIndex(neiPair.first, neiPair.second) + 1, columnToUpdate) = AdjValue;
                    AdjMat_two(columnToUpdate, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
                }
            }
        }
        break;
    default:
        break;
    }
}

void ResistanceDistance::updateAdjMatNew(std::vector<std::vector<int>> boardMatrix, int x, int y)
{
    int boardSize = boardMatrix.size();
    // Get the correct column/row to update from move coordinates
    int updateRow = getOneDIndex(x, y) + 1;

    // Create vector of neighbour coordinates
    neighbours.clear();
    neighbours.push_back(std::make_pair(x - 1, y));
    neighbours.push_back(std::make_pair(x - 1, y + 1));
    neighbours.push_back(std::make_pair(x, y - 1));
    neighbours.push_back(std::make_pair(x, y + 1));
    neighbours.push_back(std::make_pair(x + 1, y - 1));
    neighbours.push_back(std::make_pair(x + 1, y));

    // See who just played
    switch (boardMatrix[x][y])
    {
    case 1:
        for (auto &&neiPair : neighbours)
        {
            // Check cell validity
            if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
            {
                // Update player's AdjMat
                // Set to 1, as boardMatrix[x][y] definitely has a friendly colour in it
                int AdjValue = 1;

                switch (boardMatrix[neiPair.first][neiPair.second])
                {
                case 0:
                    AdjValue += 2;
                    break;
                case 1:
                    AdjValue += 1;
                    break;
                case 2:
                    AdjValue += 10000;
                    break;
                }

                AdjMat(getOneDIndex(neiPair.first, neiPair.second) + 1, updateRow) = AdjValue;
                AdjMat(updateRow, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;

                // Update opponent's AdjMat
                AdjValue = 10000;
                switch (boardMatrix[neiPair.first][neiPair.second])
                {
                case 0:
                    AdjValue += 2;
                    break;
                case 1:
                    AdjValue += 10000;
                    break;
                case 2:
                    AdjValue += 1;
                    break;
                }
                AdjMat_two(getOneDIndex(neiPair.first, neiPair.second) + 1, updateRow) = AdjValue;
                AdjMat_two(updateRow, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
            }
        }

        break;
    case 2:
        for (auto &&neiPair : neighbours)
        {
            // Check cell validity
            if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
            {
                // Update player's AdjMat
                // Set to 1, as boardMatrix[x][y] definitely has a friendly colour in it
                int AdjValue = 1;

                switch (boardMatrix[neiPair.first][neiPair.second])
                {
                case 0:
                    AdjValue += 2;
                    break;
                case 1:
                    AdjValue += 10000;
                    break;
                case 2:
                    AdjValue += 1;
                    break;
                }

                AdjMat_two(getOneDIndex(neiPair.first, neiPair.second) + 1, updateRow) = AdjValue;
                AdjMat_two(updateRow, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;

                // Update opponent's AdjMat
                AdjValue = 10000;
                switch (boardMatrix[neiPair.first][neiPair.second])
                {
                case 0:
                    AdjValue += 2;
                    break;
                case 1:
                    AdjValue += 1;
                    break;
                case 2:
                    AdjValue += 10000;
                    break;
                }

                AdjMat(getOneDIndex(neiPair.first, neiPair.second) + 1, updateRow) = AdjValue;
                AdjMat(updateRow, getOneDIndex(neiPair.first, neiPair.second) + 1) = AdjValue;
            }
        }
        break;
    }

    // Update edge node entries in both AdjMats

    // Player one
    for (int j = 0; j < boardSize; j++)
    {
        // Top Row node now has value = 1
        int AdjValue = 1;

        switch (boardMatrix[0][j])
        {
        case 0:
            AdjValue += 2;
            break;
        case 1:
            AdjValue += 1;
            break;
        case 2:
            AdjValue += 10000;
            break;
        }
        AdjMat(0, getOneDIndex(0, j) + 1) = AdjValue;
        AdjMat(getOneDIndex(0, j) + 1, 0) = AdjValue;

        // Bottom Row
        AdjValue = 1;

        switch (boardMatrix[boardSize - 1][j])
        {
        case 0:
            AdjValue += 2;
            break;
        case 1:
            AdjValue += 1;
            break;
        case 2:
            AdjValue += 10000;
            break;
        }
        AdjMat(AdjMat.rows() - 1, getOneDIndex(boardSize - 1, j) + 1) = AdjValue;
        AdjMat(getOneDIndex(boardSize - 1, j) + 1, AdjMat.rows() - 1) = AdjValue;
    }

    // Player two
    for (int i = 0; i < boardSize; i++)
    {
        // Top Row node now has value = 1
        int AdjValue = 1;

        switch (boardMatrix[i][0])
        {
        case 0:
            AdjValue += 2;
            break;
        case 1:
            AdjValue += 10000;
            break;
        case 2:
            AdjValue += 1;
            break;
        }
        AdjMat_two(0, getOneDIndex(i, 0) + 1) = AdjValue;
        AdjMat_two(getOneDIndex(i, 0) + 1, 0) = AdjValue;

        // Bottom Row
        AdjValue = 1;

        switch (boardMatrix[i][boardSize - 1])
        {
        case 0:
            AdjValue += 2;
            break;

        case 1:
            AdjValue += 10000;
            break;
        case 2:
            AdjValue += 1;
            break;
        }
        AdjMat_two(AdjMat_two.rows() - 1, getOneDIndex(i, boardSize - 1) + 1) = AdjValue;
        AdjMat_two(getOneDIndex(i, boardSize - 1) + 1, AdjMat_two.rows() - 1) = AdjValue;
    }
}

void ResistanceDistance::updateLaplacian(std::vector<std::vector<int>> boardMatrix, int x, int y)
{
    int boardSize = boardMatrix.size();

    // Get the correct column to update in laplacian from move coordinates
    int moveColumn = getOneDIndex(x, y) + 1;
    double zero = 1e-10;

    // See who just played
    switch (boardMatrix[x][y])
    {
    // Player 1 just played
    case 1:
        // Set opponents rows/columns in their laplacian to be 0
        LaplacianMatrix_two.row(moveColumn).setZero();
        LaplacianMatrix_two.col(moveColumn).setZero();

        // neighbours doesn't have to be reset, since this function called immediately after update AdjMat
        for (auto &&neiPair : neighbours)
        {
            // Only perform actions on valid cells
            if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
            {

                int neighbourColumn = getOneDIndex(neiPair.first, neiPair.second) + 1;

                // Check the value in AdjMat for that row/column and set values in laplacian accordingly
                // Set along both sides of the diagonal for symmetrical matrix
                switch (int(AdjMat(moveColumn, neighbourColumn)))
                {
                case -1:
                    LaplacianMatrix(moveColumn, neighbourColumn) = 0;
                    LaplacianMatrix(neighbourColumn, moveColumn) = 0;
                    break;
                case 0:
                    LaplacianMatrix(moveColumn, neighbourColumn) = -1 / zero;
                    LaplacianMatrix(neighbourColumn, moveColumn) = -1 / zero;
                    break;
                case 1:
                    LaplacianMatrix(moveColumn, neighbourColumn) = -1 / 1;
                    LaplacianMatrix(neighbourColumn, moveColumn) = -1 / 1;
                    break;
                case 2:
                    LaplacianMatrix(moveColumn, neighbourColumn) = -1 / 2;
                    LaplacianMatrix(neighbourColumn, moveColumn) = -1 / 2;
                    break;
                default:
                    LaplacianMatrix(moveColumn, neighbourColumn) = 0;
                    LaplacianMatrix(neighbourColumn, moveColumn) = 0;
                    break;
                }
            }
        }

        // Set main diagonal to be negative of row sum

        for (int i = 0; i < LaplacianMatrix.rows(); i++)
        {
            double rowSum = LaplacianMatrix.row(i).sum() - LaplacianMatrix(i, i);
            LaplacianMatrix(i, i) = -rowSum;
        }

        break;

    // Player 2 just played
    case 2:
        // Set opponents rows/columns in their laplacian to be 0
        LaplacianMatrix.row(moveColumn).setZero();
        LaplacianMatrix.col(moveColumn).setZero();

        for (auto &&neiPair : neighbours)
        {
            // Only perform actions on valid cells
            if (0 <= neiPair.first && neiPair.first < boardSize && 0 <= neiPair.second && neiPair.second < boardSize)
            {

                int neighbourColumn = getOneDIndex(neiPair.first, neiPair.second) + 1;

                // Check the value in AdjMat for that row/column and set values in laplacian accordingly
                // Set along both sides of the diagonal for symmetrical matrix
                switch (int(AdjMat_two(moveColumn, neighbourColumn)))
                {
                case -1:
                    LaplacianMatrix_two(moveColumn, neighbourColumn) = 0;
                    LaplacianMatrix_two(neighbourColumn, moveColumn) = 0;
                    break;
                case 0:
                    LaplacianMatrix_two(moveColumn, neighbourColumn) = -1 / zero;
                    LaplacianMatrix_two(neighbourColumn, moveColumn) = -1 / zero;

                    break;
                case 1:
                    LaplacianMatrix_two(moveColumn, neighbourColumn) = -1 / 1;
                    LaplacianMatrix_two(neighbourColumn, moveColumn) = -1 / 1;
                    break;
                case 2:
                    LaplacianMatrix_two(moveColumn, neighbourColumn) = -1 / 2;
                    LaplacianMatrix_two(neighbourColumn, moveColumn) = -1 / 2;
                    break;
                default:
                    LaplacianMatrix_two(moveColumn, neighbourColumn) = 0;
                    LaplacianMatrix_two(neighbourColumn, moveColumn) = 0;
                    break;
                }
            }
        }
        // Set main diagonal to be negative of row sum
        for (int i = 0; i < LaplacianMatrix_two.rows(); i++)
        {
            double rowSum = LaplacianMatrix_two.row(i).sum() - LaplacianMatrix_two(i, i);
            LaplacianMatrix_two(i, i) = -rowSum;
        }
        break;
    default:
        break;
    }
}

void ResistanceDistance::updateLaplacianNew(std::vector<std::vector<int>> boardMatrix, int x, int y)
{
    int boardSize = boardMatrix.size();
    // Get the row/column in the AdjMat that corresponds to the move
    int updateCol = getOneDIndex(x, y) + 1;

    // Update black's Laplacian
    for (int row = 0; row < AdjMat.rows(); row++)
    {
        if (AdjMat(row, updateCol) != 0)
        {
            double lapValue = -1.0 / AdjMat(row, updateCol);
            LaplacianMatrix(row, updateCol) = lapValue;
            LaplacianMatrix(updateCol, row) = lapValue;
        }
    }

    // Update white's Laplacian
    for (int row = 0; row < AdjMat_two.rows(); row++)
    {
        if (AdjMat_two(row, updateCol) != 0)
        {
            double lapValue = -1.0 / AdjMat_two(row, updateCol);
            LaplacianMatrix_two(row, updateCol) = lapValue;
            LaplacianMatrix_two(updateCol, row) = lapValue;
        }
    }

    // Set diagonal entries in the updated Laplacian
    for (int i = 0; i < LaplacianMatrix.rows(); i++)
    {
        double rowSum = LaplacianMatrix.row(i).sum() - LaplacianMatrix(i, i);
        LaplacianMatrix(i, i) = -rowSum;
    }

    for (int i = 0; i < LaplacianMatrix_two.rows(); i++)
    {
        double rowSum = LaplacianMatrix_two.row(i).sum() - LaplacianMatrix_two(i, i);
        LaplacianMatrix_two(i, i) = -rowSum;
    }
}

// Redundant function, old pinv method of resistance distance
float ResistanceDistance::getResistanceDistance_board()
{
    // Difference between old method and new method of calculating Linv has elements of order of magnitude 10^-15 to -17, close enough that they are to be treated as identical
    // MatrixXd Linv = LaplacianMatrix.completeOrthogonalDecomposition().pseudoInverse();
    MatrixXd Linv = pseudoInverse(LaplacianMatrix);

    int A = 0;
    int B = LaplacianMatrix.cols() - 1;

    // This value decreases as the board gets easier to connect, i.e better for black
    // First equation holds for any generalise inverse, second for moore-penrose. Both we implemented to confirm matrix is actually a M-P
    // float ResistanceDistanceValue = Linv(A, A) + Linv(B, B) - Linv(A, B) - Linv(B, A);
    float ResistanceDistanceValue = Linv(A, A) + Linv(B, B) - 2 * Linv(A, B);

    // // Hacky way of detecting black win, should return highest possible score
    // if (ResistanceDistanceValue < 1e-9)
    // {
    //     return std::numeric_limits<float>::infinity();
    // }

    // Player two
    // MatrixXd Linv_two = LaplacianMatrix_two.completeOrthogonalDecomposition().pseudoInverse();
    MatrixXd Linv_two = pseudoInverse(LaplacianMatrix_two);

    // This value decreases as white's position get's better
    // float ResistanceDistanceValue_two = Linv_two(A, A) + Linv_two(B, B) - Linv_two(A, B) - Linv_two(B, A);
    float ResistanceDistanceValue_two = Linv_two(A, A) + Linv_two(B, B) - 2 * Linv_two(A, B);

    // // Hacky way of detecting white win, should return lowest possible score
    // if (ResistanceDistanceValue_two < 1e-9)
    // {
    //     return -std::numeric_limits<float>::infinity();
    // }

    // White wants to maximise, Black wants to minimise
    float boardScore = log(ResistanceDistanceValue / ResistanceDistanceValue_two);

    return boardScore;
}

MatrixXd ResistanceDistance::pseudoInverse(const MatrixXd &M)
{
    Eigen::JacobiSVD<MatrixXd> svd(M, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::VectorXd singularValues = svd.singularValues();
    Eigen::VectorXd singularValues_inv = singularValues;

    // Calculate the reciprocal of singular values, setting low values to zero
    for (int i = 0; i < singularValues.size(); i++)
    {
        if (singularValues(i) > 1e-10)
        {
            singularValues_inv(i) = 1.0 / singularValues(i);
        }
        else
        {
            singularValues_inv(i) = 0;
        }
    }

    // Construct the vector as a diagonal matrix
    MatrixXd S_inv = singularValues_inv.asDiagonal();

    return svd.matrixV() * S_inv * svd.matrixU().transpose();
}

void ResistanceDistance::printChild(std::vector<std::vector<int>> board)
{
    int boardSize = board.size();
    std::cout << std::string(2 * board.size() + 2, '-') << std::endl;
    for (int i = 0; i < boardSize; i++)
    {
        std::cout << std::string(i, ' ');
        std::cout << "\\ ";
        for (int j = 0; j < boardSize; j++)
        {
            std::cout << board[i][j] << " ";
        }
        std::cout << "\\" << std::endl;
    }
    std::cout << std::string(board.size(), ' ');
    std::cout << std::string(2 * board.size() + 2, '-') << std::endl;
}