#include "config.h"
#include "markov.h"

std::array<double, NS3Config::numNodes> NS3Config::onoffDataRates = NS3Config::InitOnoffDataRates();

std::array<double, NS3Config::numNodes> NS3Config::limitRates = NS3Config::InitLimitRates();

std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> NS3Config::matrix = MarkovAlgorithm::GenerateStochasticMatrix();

std::unordered_map<ns3::Ptr<ns3::Node>, int> NS3Config::node2idx;
std::unordered_map<ns3::Ipv4Address, int, Ipv4AddressHash> NS3Config::ip2idx;

std::array<double, NS3Config::numNodes> NS3Config::InitOnoffDataRates()
{
    std::array<double, NS3Config::numNodes> onoffDataRates;
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        onoffDataRates[i] = 1000 * (i + 1);
    }
    return onoffDataRates;
}

std::array<double, NS3Config::numNodes> NS3Config::InitLimitRates()
{
    std::array<double, NS3Config::numNodes> limitRates;
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        limitRates[i] = 1.0 / NS3Config::numNodes;
    }
    return limitRates;
}