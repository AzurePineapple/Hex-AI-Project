#include <iostream>
#include <vector>
#include <limits>
#include <time.h>
#include "ResistanceDistance.cpp"
#include <ctime>
#include <random>
#include <map>

class MiniMax
{
private:
    ResistanceDistance *Evaluator = new ResistanceDistance();

    std::map<std::vector<std::vector<int>>, float> scoresMap;

    // Redundant %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    std::vector<std::vector<std::vector<int>>> childStates;
    std::vector<std::vector<int>> bestState;
    std::vector<std::vector<std::vector<int>>> bestStatePath;

public:
    MiniMax();
    ~MiniMax();



    std::vector<std::pair<int, int>> generateMoves(const std::vector<std::vector<int>> &boardMatrix);
    
    float recurse(std::vector<std::vector<int>> &boardMatrix, int depth, bool blacksTurn, float alpha, float beta, int *noRecursions, time_t start_time, double time_limit);

    std::pair<int, int> findBestMove(std::vector<std::vector<int>> boardMatrix, bool blacksTurn, int depth, double time_limit);

    bool isTerminal(const std::vector<std::vector<int>> &boardMatrix);

    // Redundant %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    void createChildren(std::vector<std::vector<int>> boardMatrix, int playerCode);
    void printChildren();
};
