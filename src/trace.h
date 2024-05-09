#pragma once

#include <format>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

#include <ns3/node-container.h>

#include <spdlog/spdlog.h>

#include "config.h"

class Checker
{
public:
    static void CheckTotalRate(ns3::NodeContainer hostNodes, uint64_t ts);

private:
    static std::shared_ptr<spdlog::logger> logger;

    static double TraceTotalRate(ns3::NodeContainer hostNodes);
};

template <typename T>
void TraceValue(std::string context, T oldValue, T newValue)
{
    std::ofstream file;
    std::string filename = std::format("{}/limit-rate/{}.txt", NS3Config::traceDir, context);
    file.open(filename, std::ios::app);
    if (file.tellp() == 0)
    {
        file << 1.0 / NS3Config::numNodes << std::endl;
    }
    file << newValue << std::endl;
    file.close();
}

void ClearDirectoryContents(const std::string& directoryPath);

uint32_t GetCurrentTs();