// #include <Eigen/Dense>
#include "src/include/Eigen/Dense"
// #include <Eigen/QR>
#include "src/include/Eigen/QR"
#include <iostream>
#include <limits>
#include <math.h>
#include <omp.h>
using Eigen::MatrixXd;

class ResistanceDistance
{
private:
    std::vector<std::vector<int>> exampleBoard;

    // Adjacency Matrices for each player
    MatrixXd AdjMat;
    MatrixXd AdjMat_two;

    // Empty indeces of AdjMat, used to reduce it
    std::vector<int> emptyColIndeces;

    // Redundant
    // Reduced AdjMat;
    MatrixXd RedAdjMat;
    MatrixXd RedAdjMat_two;

    // Laplacian Matrix
    MatrixXd LaplacianMatrix;
    MatrixXd LaplacianMatrix_two;
    // Laplacian Matrix
    MatrixXd tempLaplacianMatrix;
    MatrixXd tempLaplacianMatrix_two;

    // Vector to contain neighbouring cells for a given i,j
    std::vector<std::pair<int, int>> neighbours;

    // Vector to contain bridge neighbours for a given i,j
    std::vector<std::pair<int, int>> bridgeNeighbours;

    // Vector for storing 2D indeces
    std::vector<std::pair<int, int>> indexMap;

    // Vector for storing 2D indeces after AdjMat has been reduced
    std::vector<std::pair<int, int>> IndexMap_one;
    std::vector<std::pair<int, int>> IndexMap_two;

    // Flag to say if the Adjacency matrix has been initialised, to check if it needs to be created
    bool AdjMatInitialised = false;

public:
    ResistanceDistance(/* args */);
    ~ResistanceDistance();

    float evaluate(const std::vector<std::vector<int>> &boardMatrix);
    float evaluateBoard(std::vector<std::vector<int>> boardMatrix);
    float evaluateMove(std::vector<std::vector<int>> boardMatrix, int i, int j);
    int getOneDIndex(int i, int j);
    int getOneDIndex_Reduced(int i, int j);
    void createAdjMatOld(std::vector<std::vector<int>> boardMatrix);
    void createAdjMatNew(std::vector<std::vector<int>> boardMatrix);
    void reduceAdjMat();
    void createLaplacianOld();
    void createLaplacianNew();
    void initialiseLaplacian();
    void updateAdjMat(std::vector<std::vector<int>> boardMatrix, int x, int y);
    void updateAdjMatNew(std::vector<std::vector<int>> boardMatrix, int x, int y);
    void updateLaplacian(std::vector<std::vector<int>> boardMatrix, int x, int y);
    void updateLaplacianNew(std::vector<std::vector<int>> boardMatrix, int x, int y);

    void printChild(std::vector<std::vector<int>> board);

    int getResistanceDistance_tiles(std::pair<int, int> tileA, std::pair<int, int> tileB);
    float getResistanceDistance_board();
    float alternateResistanceDistance();
    MatrixXd pseudoInverse(const MatrixXd &M);
    bool validateTile(std::pair<int, int> cell, int boardSize);
};
