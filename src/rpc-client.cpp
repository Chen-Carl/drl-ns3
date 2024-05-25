#include "formatter.h"
#include "rpc-client.h"
#include "config.h"
#include "markov.h"
#include "trace.h"

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

    m_matrix = std::array<double, NS3Config::numNodes>();
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
        m_socket = nullptr;
    }
}

void RpcClientApplication::SendRpcRequest(ns3::Ptr<ns3::Node> remote, int msgId)
{
    SPDLOG_LOGGER_TRACE(logger, "{} RpcClientApplication::SendRpcRequest(remote={})", ns3::Simulator::Now().As(ns3::Time::S), remote);

    // Set parameters for the new iteration
    ns3::IntegerValue status(msgId);
    GetNode()->SetAttribute("Status", status);
    ns3::IntegerValue iteration;
    GetNode()->GetAttribute("Iteration", iteration);
    GetNode()->SetAttribute("Process", ns3::IntegerValue(0));
    if (msgId == 0)
    {
        SPDLOG_LOGGER_INFO(logger, "{} [*iteration={}] rpc client {} requests for updating transfer matrix to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), remote->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal());
        int self = GetNode()->GetId();
        m_matrix[self] = NS3Config::matrix[self][self];
    }
    else if (msgId == 1)
    {
        SPDLOG_LOGGER_INFO(logger, "{} [iteration={}] rpc client {} requests for updating limit rate", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal());
        // reset local input rate and limit rate
        ns3::DoubleValue inputRateValue;
        ns3::DoubleValue limitRateValue;
        GetNode()->GetAttribute("InputRate", inputRateValue);
        GetNode()->GetAttribute("LimitRate", limitRateValue);
        m_inputRate = inputRateValue.Get();
        m_limitRate = 0;
    }
    // ns3::IntegerValue process(0);
    // GetNode()->SetAttribute("Process", process);

    m_socket = ns3::Socket::CreateSocket(GetNode(), ns3::TcpSocketFactory::GetTypeId());
    ns3::Ipv4Address remoteAddress = remote->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    ns3::Ipv4Address localAddress = m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    m_socket->Connect(ns3::InetSocketAddress(remoteAddress, NS3Config::rpcPort));

    ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(reinterpret_cast<const uint8_t*>(&msgId), sizeof(int));
    m_socket->Send(packet);
    SPDLOG_LOGGER_DEBUG(logger, "{} rpc client {} sent {} bytes (msg id = {}) request to {}:{}", ns3::Simulator::Now().As(ns3::Time::S), m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), packet->GetSize(), msgId, remoteAddress, NS3Config::rpcPort);

    m_socket->SetRecvCallback(ns3::MakeCallback(&RpcClientApplication::HandleRead, this));
}

void RpcClientApplication::ScheduleSendRpcRequest(int msgId, ns3::NodeContainer hostNodes)
{
    ns3::IntegerValue iteration;
    GetNode()->GetAttribute("Iteration", iteration);
    GetNode()->SetAttribute("Iteration", ns3::IntegerValue(iteration.Get() + 1));
    ns3::DoubleValue inputRateValue;
    GetNode()->GetAttribute("InputRate", inputRateValue);
    m_limitRate = 0;
    m_inputRate = inputRateValue.Get();
    GetNode()->SetAttribute("Status", ns3::IntegerValue(msgId));

    int self = GetNode()->GetId();
    SendRpcRequest(hostNodes.Get((self + 1) % NS3Config::numNodes), msgId);
    if (NS3Config::numNeighbours == 2)
    {
        SendRpcRequest(hostNodes.Get((self - 1 + NS3Config::numNodes) % NS3Config::numNodes), msgId);
    }
}

void RpcClientApplication::HandleRead(ns3::Ptr<ns3::Socket> socket)
{
    SPDLOG_LOGGER_TRACE(logger, "RpcClientApplication::HandleRead(socket)");

    ns3::Ptr<ns3::Packet> packet;
    ns3::Address from;
    ns3::Address localAddress;
    packet = socket->RecvFrom(sizeof(double) * 2, 0, from);
    std::vector<uint8_t> buffer;
    buffer.resize(packet->GetSize());
    packet->CopyData(buffer.data(), buffer.size());
    double limitRate = *reinterpret_cast<double*>(buffer.data());
    double inputRate = *reinterpret_cast<double*>(buffer.data() + sizeof(double));
    SPDLOG_LOGGER_DEBUG(logger, "{} rpc client {} received {} bytes response from {}:{}, receive: limitRate={}, inputRate={}", ns3::Simulator::Now().As(ns3::Time::S), m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), packet->GetSize(), ns3::InetSocketAddress::ConvertFrom(from).GetIpv4(), ns3::InetSocketAddress::ConvertFrom(from).GetPort(), limitRate, inputRate);
    socket->Close();

    ns3::IntegerValue process;
    GetNode()->GetAttribute("Process", process);
    GetNode()->SetAttribute("Process", ns3::IntegerValue(process.Get() + 1));
    ns3::IntegerValue iteration;
    GetNode()->GetAttribute("Iteration", iteration);
    SPDLOG_LOGGER_INFO(logger, "{} [iteration={}] rpc client {} receive from neighbour {}/{}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), process.Get() + 1, NS3Config::numNeighbours);

    ns3::IntegerValue status;
    GetNode()->GetAttribute("Status", status);
    int other = NS3Config::ip2idx[ns3::InetSocketAddress::ConvertFrom(from).GetIpv4()];
    if (status.Get() == 0)
    {
        // update input rate
        UpdateInputRate(inputRate, limitRate, other);
    }
    else if (status.Get() == 1)
    {
        // update limit rate
        UpdateLimitRate(inputRate, limitRate, other);
    }
}

void RpcClientApplication::UpdateInputRate(double inputRate, double limitRate, int other)
{
    int self = GetNode()->GetId();
    m_matrix[other] = std::min(NS3Config::matrix[other][self], m_inputRate / inputRate * NS3Config::matrix[self][other]);
    m_matrix[self] += NS3Config::matrix[self][other] - std::min(NS3Config::matrix[self][other], inputRate / m_inputRate * NS3Config::matrix[other][self]);
    ns3::IntegerValue iteration;
    GetNode()->GetAttribute("Iteration", iteration);
    SPDLOG_LOGGER_DEBUG(logger, "{} [iteration={}] rpc client {} updates matrix to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), m_matrix);

    ns3::IntegerValue process;
    GetNode()->GetAttribute("Process", process);
    if (process.Get() == NS3Config::numNeighbours)
    {
        SPDLOG_LOGGER_INFO(logger, "{} [iteration={}] rpc client {} updates matrix to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), m_matrix);
    }
}

void RpcClientApplication::UpdateLimitRate(double inputRate, double limitRate, int other)
{
    ns3::IntegerValue iteration;
    GetNode()->GetAttribute("Iteration", iteration);
    int self = GetNode()->GetId();
    m_limitRate += m_matrix[other] * limitRate;
    SPDLOG_LOGGER_DEBUG(logger, "{} [iteration={}] rpc client {} updates limit rate to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), m_limitRate);
    ns3::IntegerValue process;
    GetNode()->GetAttribute("Process", process);
    if (process.Get() == NS3Config::numNeighbours)
    {
        ns3::DoubleValue lastLimitRateValue;
        GetNode()->GetAttribute("LimitRate", lastLimitRateValue);
        m_limitRate += m_matrix[self] * lastLimitRateValue.Get();
        ns3::DoubleValue limitRateValue(m_limitRate);
        GetNode()->SetAttribute("LimitRate", limitRateValue);
        ns3::IntegerValue iteration;
        GetNode()->GetAttribute("Iteration", iteration);
        SPDLOG_LOGGER_INFO(logger, "{} [iteration={}] rpc client {} updates limit rate to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), m_limitRate);
    }
}

void RpcClientApplication::C3PUpdateInputRate(double inputRate, double limitRate, int other)
{
    int self = GetNode()->GetId();
    m_matrix[other] = NS3Config::c3pEta / m_inputRate;
    m_matrix[self] = 1 - NS3Config::c3pEta / inputRate * (NS3Config::numNodes - 1);
}

void RpcClientApplication::C3PUpdateLimitRate(double inputRate, double limitRate, int other)
{
    ns3::IntegerValue iteration;
    GetNode()->GetAttribute("Iteration", iteration);
    int self = GetNode()->GetId();
    m_limitRate += m_matrix[other] * limitRate;
    SPDLOG_LOGGER_DEBUG(logger, "{} [iteration={}] rpc client {} updates limit rate to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), m_socket->GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), m_limitRate);
    ns3::IntegerValue process;
    GetNode()->GetAttribute("Process", process);
    if (process.Get() == NS3Config::numNeighbours)
    {
        ns3::DoubleValue lastLimitRateValue;
        GetNode()->GetAttribute("LimitRate", lastLimitRateValue);
        m_limitRate += m_matrix[self] * lastLimitRateValue.Get();
        ns3::DoubleValue limitRateValue(m_limitRate);
        GetNode()->SetAttribute("LimitRate", limitRateValue);
        ns3::IntegerValue iteration;
        GetNode()->GetAttribute("Iteration", iteration);
        SPDLOG_LOGGER_INFO(logger, "{} [iteration={}] rpc client {} updates limit rate to {}", ns3::Simulator::Now().As(ns3::Time::S), iteration.Get(), GetNode()->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal(), m_limitRate);
    }
}


}