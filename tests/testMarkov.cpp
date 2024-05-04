#include "markov.h"

void testStochasticMatrix();

int main()
{
    testStochasticMatrix();
    return 0;
}

void testStochasticMatrix()
{
    MarkovAlgorithm::Matrix matrix = MarkovAlgorithm::GenerateStochasticMatrix();
    std::cout << matrix << std::endl;
}
