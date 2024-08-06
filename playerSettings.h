#ifndef PLAYERSETTINGS_H
#define PLAYERSETTINGS_H

#include <iostream>
#include <string>

struct playerSettings
{
    std::string experimentName;
    std::string playerType;
    int playerCode;
    std::string AIType;
    bool isParallelised;
    int mmDepth;
    double mmTimeLimit;
    double mctsTimeLimit;
    int mctsIterLimit;
    bool rootParallelised;
    double explorationConstant;
    double RAVEBias;

    // Declare the friend function so that I can overload the << operator
    friend std::ostream &operator<<(std::ostream &os, const playerSettings &settings);
};

// Overload the << operator so I can output the contents of the struct to a file
inline std::ostream &operator<<(std::ostream &os, const playerSettings &settings)
{
    os << "Experiment Name, " << settings.experimentName << "\n"
       << "Player Type, " << settings.playerType << "\n"
       << "Player Code, " << settings.playerCode << "\n"
       << "AI Type, " << settings.AIType << "\n"
       << "Is Parallelised, " << (settings.isParallelised ? "true" : "false") << "\n"
       << "Minimax Depth, " << settings.mmDepth << "\n"
       << "Minimax Time Limit, " << settings.mmTimeLimit << "\n"
       << "MCTS Time Limit, " << settings.mctsTimeLimit << "\n"
       << "MCTS Iteration Limit, " << settings.mctsIterLimit << "\n"
       << "Root Parallelised, " << (settings.rootParallelised ? "true" : "false") << "\n"
       << "Exploration Constant, " << settings.explorationConstant << "\n"
       << "RAVE Bias, " << settings.RAVEBias << "\n";
    return os;
}
#endif