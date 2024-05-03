#pragma once

#include <string>
#include <array>
#include <cstdint>

class NS3Config
{
public:
    static constexpr int numNodes = 10;
    static constexpr uint16_t port = 9;

    // csma channel settings
    static constexpr int csmaDataRate = 100000;     // bps
    static constexpr int csmaDelay = 1;             // ms

    // onoff application settings
    static constexpr int onoffDataRate = 5000;      // bps
    static constexpr int packetSize = 64;           // bytes

    // rate limiting settings
    static constexpr int rateLimit = 4000;          // bps
    static constexpr int numWindow = 10;
    static constexpr int windowSize = 100;          // ms
    static constexpr int totalWindows = 100;        // total 100 * 100 ms

    // rpc settings
    static constexpr int rpcPort = 9090;

    static std::array<int, numNodes> onoffDataRates;
};
