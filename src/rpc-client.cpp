#include "formatter.h"
#include "rpc-client.h"
#include "config.h"

#include <ns3/socket.h>
#include <ns3/ipv4.h>
#include <ns3/tcp-socket-factory.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace drl {

NS_LOG_COMPONENT_DEFINE("RpcClientApplication");

NS_OBJECT_ENSURE_REGISTERED(RpcClientApplication);

std::shared_ptr<spdlog::logger> RpcClientApplication::logger = spdlog::stdout_color_mt("RpcClientApplication", spdlog::color_mode::always);

ns3::TypeId RpcClientApplication::GetTypeId()
{
    static ns3::TypeId tid = ns3::TypeId("drl::RpcClientApplication")
        .SetParent<ns3::Application>()
        .SetGroupName("Applications")
        .AddConstructor<RpcClientApplication>();
    return tid;
}

RpcClientApplication::RpcClientApplication() 
{ 
    logger->set_pattern("[%n] %^[%l]%$ %g:%# %v");
    logger->set_level(spdlog::level::trace);
    SPDLOG_LOGGER_TRACE(logger, "RpcClientApplication::RpcClientApplication()");
}

RpcClientApplication::~RpcClientApplication() 
{
    SPDLOG_LOGGER_TRACE(logger, "RpcClientApplication::~RpcClientApplication()");
}

void RpcClientApplication::StartApplication()
{
    SPDLOG_LOGGER_TRACE(logger, "RpcClientApplication::StartApplication()");
}

void RpcClientApplication::StopApplication()
{
    SPDLOG_LOGGER_TRACE(logger, "RpcClientApplication::StopApplication()");

    if (m_socket != nullptr)
    {
        m_socket->Close();
    }
}

void RpcClientApplication::SendRpcRequest(ns3::Ptr<ns3::Node> remote)
{
    SPDLOG_LOGGER_TRACE(logger, "{} RpcClientApplication::SendRpcRequest(remote={})", ns3::Simulator::Now().As(ns3::Time::S), remote);

    m_socket = ns3::Socket::CreateSocket(GetNode(), ns3::TcpSocketFactory::GetTypeId());
    ns3::Ipv4Address remoteAddress = remote->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    ns3::Ipv4Address localAddress = m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    m_socket->Connect(ns3::InetSocketAddress(remoteAddress, NS3Config::rpcPort));

    std::string msg = "Hello, server!";
    ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(reinterpret_cast<const uint8_t*>(msg.c_str()), msg.size());
    m_socket->Send(packet);
    SPDLOG_LOGGER_INFO(logger, "{} rpc client {} sent {} bytes ({}) request to {}:{}", ns3::Simulator::Now().As(ns3::Time::S), m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), packet->GetSize(), msg, remoteAddress, NS3Config::rpcPort);

    m_socket->SetRecvCallback(ns3::MakeCallback(&RpcClientApplication::HandleRead, this));
}

void RpcClientApplication::HandleRead(ns3::Ptr<ns3::Socket> socket)
{
    SPDLOG_LOGGER_TRACE(logger, "RpcClientApplication::HandleRead(socket)");

    ns3::Ptr<ns3::Packet> packet;
    ns3::Address from;
    ns3::Address localAddress;
    // while (packet = socket->RecvFrom(16, 0, from))
    // {
    //     uint8_t buffer[16];
    //     packet->CopyData(buffer, 16);
    //     uint64_t limitRate = *reinterpret_cast<uint64_t*>(buffer);
    //     uint64_t inputRate = *reinterpret_cast<uint64_t*>(buffer + sizeof(uint64_t));
    //     SPDLOG_LOGGER_INFO(logger, "{} rpc client received {} bytes response from {}:{}, limitRate={}, inputRate={}", ns3::Simulator::Now().As(ns3::Time::S), packet->GetSize(), ns3::InetSocketAddress::ConvertFrom(from).GetIpv4(), ns3::InetSocketAddress::ConvertFrom(from).GetPort(), limitRate, inputRate);
    // }
    packet = socket->RecvFrom(16, 0, from);
    uint8_t buffer[16];
    packet->CopyData(buffer, 16);
    uint64_t limitRate = *reinterpret_cast<uint64_t*>(buffer);
    uint64_t inputRate = *reinterpret_cast<uint64_t*>(buffer + sizeof(uint64_t));
    SPDLOG_LOGGER_INFO(logger, "{} rpc client received {} bytes response from {}:{}, receive: limitRate={}, inputRate={}", ns3::Simulator::Now().As(ns3::Time::S), packet->GetSize(), ns3::InetSocketAddress::ConvertFrom(from).GetIpv4(), ns3::InetSocketAddress::ConvertFrom(from).GetPort(), limitRate, inputRate);
    socket->Close();
}

}