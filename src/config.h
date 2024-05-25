#pragma once

#include <string>
#include <array>
#include <unordered_map>
#include <cstdint>

#include <ns3/node.h>

struct Ipv4AddressHash
{
    std::size_t operator()(const ns3::Ipv4Address &ipv4) const
    {
        return std::hash<uint32_t>()(ipv4.Get());
    }
};

class NS3Config
{
public:
    static constexpr int numNodes = 10;
    static constexpr int numNeighbours = (numNodes == 2 ? 1 : 2);
    static constexpr uint16_t port = 9;

    // csma channel settings
    static constexpr int csmaDataRate = 100000000;  // bps
    static constexpr int csmaDelay = 1;             // us

    // onoff application settings
    static constexpr int onoffDataRate = 5000;      // bps
    static constexpr int packetSize = 64;           // bytes

    // rate limiting settings
    static constexpr int rateLimit = 4000;          // bps
    static constexpr int numWindow = 10;
    static constexpr int windowSize = 100;          // ms
    static constexpr int totalWindows = 100;        // total 100 * 100 ms

    // simulation settings
    static constexpr int iterationTime = 2;
    static constexpr int iterationNum = 100;
    static constexpr int startTime = 0;
    static constexpr int stopTime = iterationTime * (iterationNum + 10);

    // rpc settings
    static constexpr int rpcPort = 9090;

    // algorithm settings
    static constexpr double c3pEta = 6;

    static std::array<double, numNodes> onoffDataRates;
    static std::array<double, numNodes> limitRates;

    static std::unordered_map<ns3::Ipv4Address, int, Ipv4AddressHash> ip2idx;

    static std::array<std::array<double, NS3Config::numNodes>, NS3Config::numNodes> matrix;

    static std::string traceDir;

    static std::array<double, NS3Config::numNodes> InitOnoffDataRates();

    static std::array<double, NS3Config::numNodes> InitLimitRates();
};
