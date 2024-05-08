#pragma once

#include <array>

#include "config.h"

class MarkovAlgorithm
{
public:
    static double GenerateRandomNumber();
    static double GenerateRandomNumber(double min, double max);

    static std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> GenerateStochasticMatrix();

    static std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> GenerateCycleStochasticMatrix();

    static std::array<double, NS3Config::numNodes> GetMatrixCol(const std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> &matrix, int col);
};

template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<std::array<T, N>, N> &matrix)
{
    os << "[";
    for (int i = 0; i < N; i++)
    {
        os << "[";
        for (int j = 0; j < N; j++)
        {
            if (j != N - 1)
            {
                os << matrix[i][j] << ", ";
            }
            else
            {
                os << matrix[i][j] << "]";
            }
        }
        if (i != N - 1)
        {
            os << ", " << std::endl;
        }
    }
    os << "]";
    return os;
}

template <typename T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const std::array<T, N> &arr)
{
    os << "[";
    for (int i = 0; i < N; i++)
    {
        if (i != N - 1)
        {
            os << arr[i] << ", ";
        }
        else
        {
            os << arr[i] << "]";
        }
    }
    return os;
}