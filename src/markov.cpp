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

MarkovAlgorithm::Matrix MarkovAlgorithm::GenerateStochasticMatrix()
{
    Matrix res;
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


std::ostream &operator<<(std::ostream &os, const MarkovAlgorithm::Matrix &matrix)
{
    os << "[";
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        if (i != 0)
        {
            os << " ";
        }
        for (int j = 0; j < NS3Config::numNodes; j++)
        {
            if (j != NS3Config::numNodes - 1)
            {
                os << matrix[i][j] << ", ";
            }
            else
            {
                os << matrix[i][j];
            }
        }
        if (i != NS3Config::numNodes - 1)
        {
            os << std::endl;
        }
        else
        {
            os << "]";
        }
    }
    return os;
}
