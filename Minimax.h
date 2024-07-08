#include <iostream>
#include <vector>
#include <limits>
#include <time.h>
#include "ResistanceDistance.cpp"

class MiniMax
{
private:
    std::vector<std::vector<std::vector<int>>> childStates;
    std::vector<std::vector<int>> bestState;
    std::vector<std::vector<int>> state;

    std::vector<std::vector<std::vector<int>>> bestStatePath;

    ResistanceDistance *Evaluator = new ResistanceDistance();

public:
    MiniMax();
    ~MiniMax();

    void createChildren(std::vector<std::vector<int>> boardMatrix, int playerCode);
    float evaluateBoard(std::vector<std::vector<int>> boardMatrix, int i, int j, bool blacksTurn);

    void printChildren();
    void printChild(std::vector<std::vector<int>> board);

    // std::pair<float, std::vector<std::vector<int>>> doTheThing(std::vector<std::vector<int>> boardMatrix, int depth, float alpha, float beta, bool maximisingPlayer);

    float recurse(std::vector<std::vector<int>> boardMatrix, int depth, bool blacksTurn, float alpha, float beta, int x, int y);

    std::pair<int, int> findBestMove(std::vector<std::vector<int>> boardMatrix, bool blacksTurn, int depth);

    std::pair<int, int> go(std::vector<std::vector<int>> boardMatrix, bool blacksTurn);

    std::vector<std::pair<int, int>> generateMoves(std::vector<std::vector<int>> boardMatrix);

    bool isTerminal(const std::vector<std::vector<int>> &boardMatrix);
};