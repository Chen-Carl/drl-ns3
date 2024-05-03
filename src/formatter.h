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
struct fmt::formatter<ns3::Address> : fmt::formatter<std::string> 
{
    auto format(ns3::Address ip, format_context &ctx) -> decltype(ctx.out()) 
    {
        ns3::Ipv4Address address = ns3::InetSocketAddress::ConvertFrom(ip).GetIpv4();
        uint16_t port = ns3::InetSocketAddress::ConvertFrom(ip).GetPort();
        std::stringstream ss;
        ss << address << ":" << port;
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