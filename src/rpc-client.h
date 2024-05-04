#pragma once

#include "config.h"

#include <ns3/applications-module.h>

#include <spdlog/spdlog.h>

namespace drl {

class RpcClientApplication : public ns3::Application
{
public:
    static ns3::TypeId GetTypeId();

    RpcClientApplication();
    virtual ~RpcClientApplication();

    virtual void StartApplication() override;
    virtual void StopApplication() override;

    void SendRpcRequest(ns3::Ptr<ns3::Node> remote);

    double GetInputRate() const { return m_inputRate; }
    double GetLimitRate() const { return m_limitRate; }

    void SetInputRate(double inputRate) { m_inputRate = inputRate; }
    void SetLimitRate(double limitRate) { m_limitRate = limitRate; }

private:
    static std::shared_ptr<spdlog::logger> logger;

    ns3::Ptr<ns3::Socket> m_socket;

    double m_inputRate;
    double m_limitRate;
    std::array<double, NS3Config::numNodes> m_matrix;

    void HandleRead(ns3::Ptr<ns3::Socket> socket);
};

}