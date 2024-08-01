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
    std::vector<std::vector<std::vector<int>>> childStates;
    std::vector<std::vector<int>> bestState;
    std::vector<std::vector<int>> state;

    std::vector<std::vector<std::vector<int>>> bestStatePath;

    ResistanceDistance *Evaluator = new ResistanceDistance();

    std::map<std::vector<std::vector<int>>, float> scoresMap;

public:
    MiniMax();
    ~MiniMax();

    void createChildren(std::vector<std::vector<int>> boardMatrix, int playerCode);

    void printChildren();

    // std::pair<float, std::vector<std::vector<int>>> doTheThing(std::vector<std::vector<int>> boardMatrix, int depth, float alpha, float beta, bool maximisingPlayer);

    float recurse(std::vector<std::vector<int>> &boardMatrix, int depth, bool blacksTurn, float alpha, float beta, int *noRecursions, time_t start_time, double time_limit);

    std::pair<int, int> findBestMove(std::vector<std::vector<int>> boardMatrix, bool blacksTurn, int depth, double time_limit);

    std::pair<int, int> go(std::vector<std::vector<int>> boardMatrix, bool blacksTurn, int depth, double time_limit);

    std::vector<std::pair<int, int>> generateMoves(const std::vector<std::vector<int>> &boardMatrix);

    bool isTerminal(const std::vector<std::vector<int>> &boardMatrix);
};
