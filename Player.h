#include "Minimax.cpp"
#include "MCTS.cpp"

class Player
{
private:
    bool isComputer;
    std::string playerColour;
    int playerColourCode;
    int move[2];
    std::string AIModel;
    // A_star_approach AI;
    MiniMax minimaxEngine;
    std::unique_ptr<TreeNode> root;
    bool isParallelised;
    int minimaxSearchDepth;
    int mctsTimeLimit;
    int mctsIterLimit;

public:
    Player();
    void createPlayer(std::string playerType, int colourCode, std::string AIType, int size, bool isParallelisedParam, int searchDepthParam, int mctsTimeLimitParam, int mctsIterLimitParam);
    int *playMove(int x, int y);

    void playMoveAI(std::vector<std::pair<int, int>> availableTiles, int boardSize, int *x, int *y, std::vector<std::vector<int>> boardMatrix);

    // Player colour getter
    std::string getColour();

    int getColourCode();

    void updateColourCode(int colourCode);

    bool getIsComputer();
};
