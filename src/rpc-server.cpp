#include "formatter.h"
#include "rpc-server.h"
#include "rpc-client.h"
#include "config.h"

#include <ns3/socket.h>
#include <ns3/ipv4.h>
#include <ns3/pointer.h>
#include <ns3/tcp-socket-factory.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace drl {

// NS_LOG_COMPONENT_DEFINE("drl::RpcServerApplication");

NS_OBJECT_ENSURE_REGISTERED(RpcServerApplication);

std::shared_ptr<spdlog::logger> RpcServerApplication::logger = spdlog::stdout_color_mt("RpcServerApplication", spdlog::color_mode::always);

ns3::TypeId RpcServerApplication::GetTypeId()
{
    static ns3::TypeId tid = ns3::TypeId("drl::RpcServerApplication")
        .SetParent<ns3::Application>()
        .SetGroupName("Applications")
        .AddConstructor<RpcServerApplication>()
        .AddAttribute("Local",
            "The Address on which to Bind the rx socket.",
            ns3::PointerValue(),
            ns3::MakePointerAccessor(&RpcServerApplication::m_local),
            ns3::MakePointerChecker<ns3::Node>());
    return tid;
}

RpcServerApplication::RpcServerApplication() 
{ 
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%n] %^[%l]%$ %g:%# %v");
    SPDLOG_LOGGER_TRACE(logger, "RpcServerApplication::RpcServerApplication()");
}

RpcServerApplication::~RpcServerApplication() 
{
    SPDLOG_LOGGER_TRACE(logger, "RpcServerApplication::~RpcServerApplication()");
}

void RpcServerApplication::StartApplication()
{
    SPDLOG_LOGGER_TRACE(logger, "RpcServerApplication::StartApplication()");

    m_socket = ns3::Socket::CreateSocket(GetNode(), ns3::TcpSocketFactory::GetTypeId());
    ns3::Ipv4Address localAddress = GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    m_socket->Bind(ns3::InetSocketAddress(localAddress, NS3Config::rpcPort));
    m_socket->Listen();
    m_socket->SetAcceptCallback(
        ns3::MakeNullCallback<bool, ns3::Ptr<ns3::Socket>, const ns3::Address &>(),
        ns3::MakeCallback(&RpcServerApplication::HandleRpcRequest, this)
    );
}

void RpcServerApplication::StopApplication()
{
    SPDLOG_LOGGER_TRACE(logger, "RpcServerApplication::StopApplication()");

    if (m_socket != nullptr)
    {
        m_socket->Close();
        m_socket = nullptr;
    }
}

void RpcServerApplication::HandleRpcRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &from)
{
    ns3::Ipv4Address address = ns3::InetSocketAddress::ConvertFrom(from).GetIpv4();
    uint16_t port = ns3::InetSocketAddress::ConvertFrom(from).GetPort();
    SPDLOG_LOGGER_TRACE(logger, "RpcServerApplication::HandleRpcRequest(socket, {}:{})", address, port);

    socket->SetRecvCallback(ns3::MakeCallback(&RpcServerApplication::HandleRead, this));
}

void RpcServerApplication::HandleRead(ns3::Ptr<ns3::Socket> socket)
{
    SPDLOG_LOGGER_TRACE(logger, "RpcServerApplication::HandleRead(socket)");

    ns3::Ptr<ns3::Packet> packet;
    ns3::Address from;
    ns3::Address localAddress;
    while (packet = socket->RecvFrom(14, 0, from))
    {
        ns3::Ptr<ns3::Application> app = m_local->GetApplication(1);
        ns3::Ptr<RpcClientApplication> client = app->GetObject<RpcClientApplication>();
        double value1 = client->GetLimitRate();
        double value2 = client->GetInputRate();
        SPDLOG_LOGGER_INFO(logger, "{} rpc server {} received {} bytes request from {}:{}, response: limitRate={}, inputRate={}", ns3::Simulator::Now().As(ns3::Time::S), m_local->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), packet->GetSize(), ns3::InetSocketAddress::ConvertFrom(from).GetIpv4(), ns3::InetSocketAddress::ConvertFrom(from).GetPort(), value1, value2);
        ns3::Ptr<ns3::Packet> response = ns3::Create<ns3::Packet>((uint8_t*)&value1, sizeof(double));
        response->AddAtEnd(ns3::Create<ns3::Packet>((uint8_t*)&value2, sizeof(double)));

        socket->Send(response);
        SPDLOG_LOGGER_INFO(logger, "{} rpc server {} sent {} bytes response to {}:{}", ns3::Simulator::Now().As(ns3::Time::S), m_local->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), response->GetSize(), ns3::InetSocketAddress::ConvertFrom(from).GetIpv4(), ns3::InetSocketAddress::ConvertFrom(from).GetPort());
    }
}

}