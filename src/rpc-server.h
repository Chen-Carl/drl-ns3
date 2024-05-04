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

private:
    static std::shared_ptr<spdlog::logger> logger;

    ns3::Ptr<ns3::Socket> m_socket;
    ns3::Ptr<ns3::Node> m_local;

    void HandleRpcRequest(ns3::Ptr<ns3::Socket> socket, const ns3::Address &from);
    void HandleRead(ns3::Ptr<ns3::Socket> socket);
};

}