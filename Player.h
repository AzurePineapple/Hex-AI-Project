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
    double mmTimeLimit;
    double mctsTimeLimit;
    int mctsIterLimit;
    bool rootParallelised;
    double explorationConstant;
    double RAVEBias;

public:
    Player();
    void createPlayer(std::string playerType, int colourCode, std::string AIType, int size, bool isParallelisedParam, int searchDepthParam, double mmTimeLimitParam, double mctsTimeLimitParam, int mctsIterLimitParam, bool rootParallelisedParam, double explorationConstantParam, double RAVEBiasParam);
    int *playMove(int x, int y);

    void playMoveAI(std::vector<std::pair<int, int>> availableTiles, int boardSize, int *x, int *y, std::vector<std::vector<int>> boardMatrix);

    // Player colour getter
    std::string getColour();

    int getColourCode();

    void updateColourCode(int colourCode);

    bool getIsComputer();

    ~Player();
};

// Create global variables for root parallelisation, this is hacky and probably bad practice but I'm short on time
std::vector<std::vector<int>> valuesVector;
std::mutex mtx;

// Thread function
void mctsThread(std::vector<std::vector<int>> boardMatrix, int row, int col, TreeNode *parent, int playerOwner, int playerColourCode, int mctsIterLimit, double mctsTimeLimit, double explorationConstant, double RAVEBias);