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
    std::cout << "tracer: writing to file ... " << std::endl;
    std::ofstream file;
    std::string filename = std::format("{}/limit-rate/{}.txt", NS3Config::traceDir, context);
    file.open(filename, std::ios::app);
    file << newValue << std::endl;
    file.close();
}

void ClearDirectoryContents(const std::string& directoryPath);

uint32_t GetCurrentTs();