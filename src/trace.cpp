#include "trace.h"

#include <format>

#include <ns3/core-module.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Checker::logger = spdlog::stdout_color_mt("Checker", spdlog::color_mode::always);

double Checker::TraceTotalRate(ns3::NodeContainer hostNodes)
{
    static const double epsilon = 1e-6;
    double totalRate = 0;
    for (int i = 0; i < NS3Config::numNodes; i++)
    {
        ns3::DoubleValue limitRateValue;
        hostNodes.Get(i)->GetAttribute("LimitRate", limitRateValue);
        if (limitRateValue.Get() > 1 + epsilon)
        {
            SPDLOG_LOGGER_ERROR(logger, "Limit rate {} exceeds 1.0", limitRateValue.Get());
            throw std::logic_error("Limit rate exceeds 1.0");
        }
        else if (limitRateValue.Get() < 0)
        {
            SPDLOG_LOGGER_ERROR(logger, "Limit rate {} is negative", limitRateValue.Get());
            throw std::logic_error("Limit rate is negative");
        }
        totalRate += limitRateValue.Get();
    }
    if (totalRate > 1.0 + epsilon)
    {
        SPDLOG_LOGGER_ERROR(logger, "Total rate {} exceeds 1.0", totalRate);
        throw std::logic_error("Total rate exceeds 1.0");
    }
    else
    {
        SPDLOG_LOGGER_INFO(logger, "Total rate {}", totalRate);
    }
    return totalRate;
}

void Checker::CheckTotalRate(ns3::NodeContainer hostNodes, uint64_t ts)
{
    try
    {
        TraceTotalRate(hostNodes);
    }
    catch(const std::logic_error& e)
    {
        for (int i = 0; i < NS3Config::numNodes; i++)
        {
            std::string filename = std::format("{}.{}.txt", i, ts);
            if (std::filesystem::exists(NS3Config::traceDir + "/limit-rate/" + filename))
            {
                std::filesystem::remove(filename);
            }
        }
        throw std::runtime_error("Total rate error!");
    }
}


void ClearDirectoryContents(const std::string& directoryPath)
{
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.path().extension() == ".txt")
        {
            std::ofstream file(entry.path());
        }
    }
}

uint32_t GetCurrentTs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    return static_cast<uint64_t>(ms.count());
}

