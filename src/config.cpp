#include "config.h"

std::array<int, NS3Config::numNodes> NS3Config::onoffDataRates = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};

std::array<double, NS3Config::numNodes> NS3Config::limitRates = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

std::unordered_map<ns3::Ptr<ns3::Node>, int> NS3Config::node2idx;
