#include "DisjointSet.h"

DisjointSet::DisjointSet()
{
}

DisjointSet::~DisjointSet()
{
}

void DisjointSet::populateSet(std::vector<std::pair<int, int>> elems)
{
    indexMap = elems;
    for (auto &&coordPair : indexMap)
    {
        makeSet(coordPair);
    }
}

void DisjointSet::makeSet(std::pair<int, int> x)
{
    parent.insert({x, x});
    size.insert({x, 1});
}

std::pair<int, int> DisjointSet::find(std::pair<int, int> x)
{
    if (parent[x] == x)
    {
        return x;
    }
    else
    {
        // Separating the result before returning implements path compression https://www.youtube.com/watch?v=QYivmB3B7S0&t=118s
        parent[x] = find(parent[x]);
        return parent[x];
    }
}

void DisjointSet::unionBySize(std::pair<int, int> x, std::pair<int, int> y)
{
    std::pair<int, int> root_x = find(x);
    std::pair<int, int> root_y = find(y);

    if (root_x == root_y)
    {
        return;
    }
    else if (size[root_x] < size[root_y])
    {
        parent[root_x] = root_y;
        size[root_y] += size[root_x];
    }
    else
    {
        parent[root_y] = root_x;
        size[root_x] += size[root_y];
    }
}

void DisjointSet::wipeDS()
{
    parent.clear();
    size.clear();
}
