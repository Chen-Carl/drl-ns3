#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO

#include <sstream>

#include <spdlog/spdlog.h>

#include <ns3/core-module.h>
#include <ns3/node.h>
#include <ns3/ipv4.h>
#include <ns3/ipv4-address.h>
#include <ns3/inet-socket-address.h>

template <>
struct fmt::formatter<ns3::TimeWithUnit> : fmt::formatter<std::string> 
{
    auto format(ns3::TimeWithUnit time, format_context &ctx) -> decltype(ctx.out()) 
    {
        std::stringstream ss;
        ss << time;
        return fmt::format_to(ctx.out(), "[ns3: {}]", ss.str());
    }
};

template <>
struct fmt::formatter<ns3::Ipv4Address> : fmt::formatter<std::string> 
{
    auto format(ns3::Ipv4Address ipv4, format_context &ctx) -> decltype(ctx.out()) 
    {
        std::stringstream ss;
        ss << ipv4;
        return fmt::format_to(ctx.out(), "{}", ss.str());
    }
};

template <>
struct fmt::formatter<ns3::Ptr<ns3::Node>> : fmt::formatter<std::string> 
{
    auto format(ns3::Ptr<ns3::Node> node, format_context &ctx) -> decltype(ctx.out()) 
    {
        ns3::Ipv4Address ipv4Address = node->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
        std::stringstream ss;
        ss << ipv4Address;
        return fmt::format_to(ctx.out(), "{}", ss.str());
    }
};

template <typename T, std::size_t N>
struct fmt::formatter<std::array<T, N>> : fmt::formatter<std::string> 
{
    auto format(std::array<T, N> arr, format_context &ctx) -> decltype(ctx.out()) 
    {
        std::stringstream ss;
        ss << "[";
        for (std::size_t i = 0; i < arr.size(); i++)
        {
            if (i != arr.size() - 1)
            {
                ss << arr[i] << ", ";
            }
            else
            {
                ss << arr[i] << "]";
            }
        }
        return fmt::format_to(ctx.out(), "{}", ss.str());
    }
};