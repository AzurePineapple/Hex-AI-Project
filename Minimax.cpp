#include "Minimax.h"
#include <chrono>
//  ResistanceDistance has WHITE as the maximising player

MiniMax::MiniMax()
{
}

MiniMax::~MiniMax()
{
}

std::vector<std::pair<int, int>> MiniMax::generateMoves(const std::vector<std::vector<int>> &boardMatrix)
{
    int boardSize = boardMatrix.size();
    std::vector<std::pair<int, int>> availableMoves;
    availableMoves.reserve(boardSize * boardSize); // Reserve maximum possible space

    for (int i = 0; i < boardSize; ++i)
    {
        for (int j = 0; j < boardSize; ++j)
        {
            if (boardMatrix[i][j] == 0)
            {
                availableMoves.emplace_back(i, j);
            }
        }
    }

    availableMoves.shrink_to_fit();

    return availableMoves;
}

float MiniMax::recurse(std::vector<std::vector<int>> &boardMatrix, int depth, bool blacksTurn, float alpha, float beta, int *noEvaluations, time_t start_time, double time_limit, double bridgeWeight)
{
    bool timeExceeded = difftime(time(NULL), start_time) >= time_limit;
    if (depth == 0 || isTerminal(boardMatrix) || timeExceeded)
    {
        *noEvaluations += 1;

        // Search the cache to see if the board state has been evaluated
        auto it = scoresMap.find(boardMatrix);
        if (it != scoresMap.end())
        {
            return it->second;
        }
        else
        {
            float score = Evaluator->evaluate(boardMatrix, bridgeWeight);
            scoresMap[boardMatrix] = score;
            return score;
        }
    }

    std::vector<std::pair<int, int>> availableMoves = generateMoves(boardMatrix);

    if (blacksTurn)
    {
        float minEval = std::numeric_limits<float>::infinity();

        // Iterate through board
        for (auto &&move : availableMoves)
        {
            // Play the move
            boardMatrix[move.first][move.second] = 1;

            // Recursively evaluate board state in new state
            float eval = recurse(boardMatrix, depth - 1, !blacksTurn, alpha, beta, noEvaluations, start_time, time_limit, bridgeWeight);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, minEval);

            // Undo the move
            boardMatrix[move.first][move.second] = 0;

            // Alpha beta pruning
            if (beta <= alpha)
            {
                break;
            }
        }

        return minEval;
    }

    else
    {

        float maxEval = -std::numeric_limits<float>::infinity();

        // Iterate through board
        for (auto &&move : availableMoves)
        {
            // Make a move
            boardMatrix[move.first][move.second] = 2;

            // Recursively evaluate board state in new state
            float eval = recurse(boardMatrix, depth - 1, !blacksTurn, alpha, beta, noEvaluations, start_time, time_limit, bridgeWeight);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, maxEval);

            // Undo the move
            boardMatrix[move.first][move.second] = 0;

            // Alpha beta pruning
            if (beta <= alpha)
            {
                break;
            }
        }

        return maxEval;
    }
}

// Makes an initial call to recurse, and uses the result to produce the i,j of the best move
std::pair<int, int> MiniMax::findBestMove(std::vector<std::vector<int>> boardMatrix, bool blacksTurn, int depth, double time_limit, double bridgeWeight)
{
    float alpha = -std::numeric_limits<float>::infinity();
    float beta = std::numeric_limits<float>::infinity();
    std::pair<int, int> bestMove;
    bestMove = std::make_pair(-1, -1);

    std::vector<std::pair<int, int>> availableMoves = generateMoves(boardMatrix);
    std::vector<std::pair<int, int>> bestMoves;

    time_t start_time = time(NULL);

    if (blacksTurn)
    {
        float bestVal = std::numeric_limits<float>::infinity();

        for (auto &&move : availableMoves)
        {
            // Play the move
            boardMatrix[move.first][move.second] = 1;
            int noEvaluations = 0;
            // Get the evaluation function of the move
            float moveVal = recurse(boardMatrix, depth - 1, !blacksTurn, alpha, beta, &noEvaluations, start_time, time_limit, bridgeWeight);
            // std::cout << "Finished evaluating: " << move.first << ", " << move.second << std::endl;
            // std::cout << "Performed " << noEvaluations << " evaluations" << std::endl;
            // undo the move
            boardMatrix[move.first][move.second] = 0;

            // If move value is better than previous, update best values
            if (moveVal < bestVal)
            {
                bestMoves.clear();
                bestMoves.push_back(move);
                bestVal = moveVal;
            }
            else if (moveVal == bestVal)
            {
                bestMoves.push_back(move);
            }
        }
    }

    else
    {
        float bestVal = -std::numeric_limits<float>::infinity();

        // Iterate over board and make a call to recursive evaluation function
        for (auto &&move : availableMoves)
        {
            // Play the move
            boardMatrix[move.first][move.second] = 2;

            int noEvaluations = 0;
            // Get the evaluation function of the move
            float moveVal = recurse(boardMatrix, depth - 1, !blacksTurn, alpha, beta, &noEvaluations, start_time, time_limit, bridgeWeight);
            // std::cout << "Finished evaluating: " << move.first << ", " << move.second << std::endl;
            // std::cout << "Performed " << noEvaluations << " evaluations" << std::endl;
            // undo the move
            boardMatrix[move.first][move.second] = 0;

            // If move value is better than previous, update best values
            if (moveVal > bestVal)
            {
                bestMoves.clear();
                bestMoves.push_back(move);
                bestVal = moveVal;
            }
            else if (moveVal == bestVal)
            {
                bestMoves.push_back(move);
            }
        }
    }

    // Select a random move from the bestMoves vector
    if (!bestMoves.empty())
    {
        srand(time(0));
        bestMove = bestMoves[rand() % bestMoves.size()];
    }

    return bestMove;
}

// Utility function to detect if board state is terminal (there is a winner). Not outright detecting a winner, detetcting if the board is full since a full board has a winner. This hopefully saves time checking for a winner after each move.
bool MiniMax::isTerminal(const std::vector<std::vector<int>> &boardMatrix)
{
    // Alternative method using inbuild efficient functions to accomplish the same tast
    return std::all_of(boardMatrix.begin(), boardMatrix.end(), [](const std::vector<int> &row)
                       { return std::all_of(row.begin(), row.end(), [](int cell)
                                            { return cell != 0; }); });
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Redundant function
void MiniMax::createChildren(std::vector<std::vector<int>> boardMatrix, int playerCode)
{
    int boardSize = boardMatrix.size();
    childStates.clear();
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            std::vector<std::vector<int>> testMatrix = boardMatrix;
            if (testMatrix[i][j] == 0)
            {
                testMatrix[i][j] = playerCode;
                childStates.push_back(testMatrix);
            }
        }
    }
}

// Redundant function
void MiniMax::printChildren()
{
    for (auto board : childStates)
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
}
