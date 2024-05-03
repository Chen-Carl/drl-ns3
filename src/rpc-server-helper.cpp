#include "rpc-server-helper.h"

#include <ns3/core-module.h>
#include <ns3/ipv4.h>

namespace drl {

RpcServerHelper::RpcServerHelper(ns3::Address local)
{
    m_factory.SetTypeId("drl::RpcServerApplication");
    m_factory.Set("Local", ns3::AddressValue(local));
}

void RpcServerHelper::SetAttribute(std::string name, const ns3::AttributeValue& value)
{
    m_factory.Set(name, value);
}

ns3::ApplicationContainer RpcServerHelper::Install(ns3::Ptr<ns3::Node> node) const
{
    return ns3::ApplicationContainer(InstallPriv(node));
}

ns3::ApplicationContainer RpcServerHelper::Install(std::string nodeName) const
{
    ns3::Ptr<ns3::Node> node = ns3::Names::Find<ns3::Node>(nodeName);
    return ns3::ApplicationContainer(InstallPriv(node));
}

ns3::ApplicationContainer RpcServerHelper::Install(ns3::NodeContainer c) const
{
    ns3::ApplicationContainer apps;
    for (auto i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }

    return apps;
}

ns3::Ptr<ns3::Application> RpcServerHelper::InstallPriv(ns3::Ptr<ns3::Node> node) const
{
    ns3::Ptr<ns3::Application> app = m_factory.Create<ns3::Application>();
    node->AddApplication(app);

    return app;
}

}