#ifndef PLAYERSETTINGS_H
#define PLAYERSETTINGS_H

#include <iostream>
#include <string>

struct playerSettings
{
    std::string experimentName = "Default";
    std::string playerType = "computer";
    int playerCode = 1;
    std::string AIType = "mcts";
    bool isParallelised = true;
    int mmDepth = 3;
    double mmTimeLimit = 10.0;
    double mctsTimeLimit = 5.0;
    int mctsIterLimit = 10000;
    bool rootParallelised = true;
    double explorationConstant = 0.3; // Default from https://cgi.cse.unsw.edu.au/~abdallahs/Papers/2010/Monte-Carlo%20Hex.pdf section 3
    double RAVEBias = 0.001;          // Default from https://cgi.cse.unsw.edu.au/~abdallahs/Papers/2010/Monte-Carlo%20Hex.pdf section 3
    double bridgeWeight = 0;          // Sets the bridge weight for resistance distance calculation. 0 means bridges are not connected. MUST be between 2.0 and 3.0, non inclusive.

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
       << "RAVE Bias, " << settings.RAVEBias << "\n"
       << "Bridge Weight, " << settings.bridgeWeight << "\n";
    return os;
}
#endif