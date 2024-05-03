#pragma once

#include <ns3/applications-module.h>

#include <spdlog/spdlog.h>

namespace drl {

class RpcServerApplication : public ns3::Application
{
public:
    static ns3::TypeId GetTypeId();

    RpcServerApplication();
    virtual ~RpcServerApplication();

    virtual void StartApplication() override;
    virtual void StopApplication() override;

    uint64_t GetLimitRate() const { return m_limitRate; }
    uint64_t GetInputRate() const { return m_inputRate; }

    void SetLimitRate(uint64_t limitRate) { m_limitRate = limitRate; }
    void SetInputRate(uint64_t inputRate) { m_inputRate = inputRate; }

private:
    static std::shared_ptr<spdlog::logger> logger;

    ns3::Ptr<ns3::Socket> m_socket;
    ns3::Ptr<ns3::Node> m_local;

    uint64_t m_limitRate;
    uint64_t m_inputRate;

    void HandleRpcRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &from);
    void HandleRead(ns3::Ptr<ns3::Socket> socket);
};

}