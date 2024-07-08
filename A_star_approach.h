#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <cmath>
#include <limits> //std::numeric_limits<double>::infinity()
#include <chrono>

class A_star_approach
{
private:
    std::vector<std::vector<double>> blackCircuit;
    std::vector<std::vector<double>> whiteCircuit;


    int playerCode;
    std::vector<std::pair<int, int>> neighbours;

public:
    A_star_approach(/* args */);
    ~A_star_approach();

    struct cell
    {
        int parent_i, parent_j;
        double f, g, h;
    };

    void updateCircuits(std::vector<std::vector<int>> boardMatrix, int colourCode);

    void showCircuits();

    // Sanity check coords
    bool isCellValid(std::vector<std::vector<double>> playerCircuit, int i, int j);
    // Check if cell is free
    bool isCellOccupied(std::vector<std::vector<double>> playerCircuit, int i, int j);
    // Check if cell is at the destination
    bool isDestination(int i, int j, std::pair<int, int> destination);
    // Calculate the Heuristic value for a given cell (hexagonal manhattan distance)
    double calculateHValue(int i, int j, std::pair<int, int> isDestination);
    // Prints the path from the source to the destination
    int tracePath(std::vector<std::vector<cell>> cellDetails, std::vector<std::vector<double>> playerCircuit, std::pair<int, int> isDestination, int *toPlayX, int *toPlayY);

    int A_Star(std::vector<std::vector<double>> playerCircuit, std::pair<int, int> source, std::pair<int, int> destination, int *x, int *y);

    void playMove(std::vector<std::vector<int>> boardMatrix, int colourCode, int *x, int *y);

    float EvaluateBoard();
};
