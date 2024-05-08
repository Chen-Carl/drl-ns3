#include "markov.h"

#include <random>

double MarkovAlgorithm::GenerateRandomNumber()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0, 1);
    return dis(gen);
}

double MarkovAlgorithm::GenerateRandomNumber(double min, double max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> MarkovAlgorithm::GenerateStochasticMatrix()
{
    std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> res;
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        double sum = 0;
        for (int j = 0; j < NS3Config::numNodes; j++)
        {
            res[i][j] = MarkovAlgorithm::GenerateRandomNumber();
            sum += res[i][j];
        }
        for (int j = 0; j < NS3Config::numNodes; j++)
        {
            res[i][j] /= sum;
        }
    }
    return res;
}

std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> MarkovAlgorithm::GenerateCycleStochasticMatrix()
{
    std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> res;
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        res[i][i] = GenerateRandomNumber();
        res[i][(i + 1) % NS3Config::numNodes] = GenerateRandomNumber(0, 1 - res[i][i]);
        res[i][(i - 1 + NS3Config::numNodes) % NS3Config::numNodes] = 1 - res[i][i] - res[i][(i + 1) % NS3Config::numNodes];
    }
    return res;
}


std::array<double, NS3Config::numNodes> MarkovAlgorithm::GetMatrixCol(const std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> &matrix, int col)
{
    std::array<double, NS3Config::numNodes> res;
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        res[i] = matrix[i][col];
    }
    return res;
}