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

    // msgId = 0: request for updating input rate
    // msgId = 1: request for updating limit rate
    void SendRpcRequest(ns3::Ptr<ns3::Node> remote, int msgId);

    void ScheduleSendRpcRequest(int msgId, ns3::NodeContainer hostNodes);

    double GetInputRate() const { return m_inputRate; }
    double GetLimitRate() const { return m_limitRate; }
    const std::array<double, NS3Config::numNodes> &GetMatrix() const { return m_matrix; }

    void SetInputRate(double inputRate) { m_inputRate = inputRate; }
    void SetLimitRate(double limitRate) { m_limitRate = limitRate; }

private:
    static std::shared_ptr<spdlog::logger> logger;

    ns3::Ptr<ns3::Socket> m_socket;

    double m_inputRate;
    double m_limitRate;
    std::array<double, NS3Config::numNodes> m_matrix;

    void HandleRead(ns3::Ptr<ns3::Socket> socket);

    void UpdateInputRate(double inputRate, double limitRate, int other);
    void UpdateLimitRate(double inputRate, double limitRate, int other);

    void C3PUpdateInputRate(double inputRate, double limitRate, int other);
    void C3PUpdateLimitRate(double inputRate, double limitRate, int other);
};

}