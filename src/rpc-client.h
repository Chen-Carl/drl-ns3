#pragma once

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

private:
    static std::shared_ptr<spdlog::logger> logger;

    ns3::Ptr<ns3::Socket> m_socket;

    void HandleRead(ns3::Ptr<ns3::Socket> socket);
};

}