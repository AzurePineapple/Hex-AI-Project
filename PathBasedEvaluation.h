#include <limits>
#include <vector>
#include <queue>

class PathBasedEvaluation
{
private:
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {1, -1}};

public:
    PathBasedEvaluation(std::vector<std::vector<int>> boardMatrix);

    getMinDistance(std::vector<std::vector<int>> boardMatrix, int playerCode);

    ~PathBasedEvaluation();
};
