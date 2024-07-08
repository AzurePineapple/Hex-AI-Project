#include <vector>
#include <map>

class DisjointSet
{
private:
    // Vector containing all coordinates in board
    std::vector<std::pair<int, int>> indexMap;

    // Map of coordinate pairs (tiles/nodes)
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    // Map of size of trees containing the key
    std::map<std::pair<int, int>, int> size;

public:
    DisjointSet();
    ~DisjointSet();

    // Creates a set for each pair of coords passed into it
    void populateSet(std::vector<std::pair<int, int>> elems);

    // Add node parents and size to relevant maps
    void makeSet(std::pair<int, int> x);
    // Find the direct parent node of given node
    std::pair<int, int> find(std::pair<int, int> x);
    // Combine two sets, joining smaller set to larger set
    void unionBySize(std::pair<int, int> x, std::pair<int, int> y);

    void wipeDS();
};
