#pragma once

#include <array>

#include "config.h"

class MarkovAlgorithm
{
public:
    using Matrix = std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes>;

    static double GenerateRandomNumber();
    static double GenerateRandomNumber(double min, double max);

    static Matrix GenerateStochasticMatrix();
};

std::ostream &operator<<(std::ostream &os, const MarkovAlgorithm::Matrix &matrix);