#include "markov.h"

void testStochasticMatrix();

int main()
{
    testStochasticMatrix();
    return 0;
}

void testStochasticMatrix()
{
    std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> matrix = MarkovAlgorithm::GenerateStochasticMatrix();
    std::cout << matrix << std::endl;
}
