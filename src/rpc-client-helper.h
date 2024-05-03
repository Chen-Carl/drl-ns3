#pragma once

#include <ns3/application-container.h>
#include <ns3/node-container.h>
#include <ns3/object-factory.h>

namespace drl {

class RpcClientHelper
{
public:
    RpcClientHelper();

    void SetAttribute(std::string name, const ns3::AttributeValue &value);

    ns3::ApplicationContainer Install(ns3::Ptr<ns3::Node> node) const;
    ns3::ApplicationContainer Install(std::string nodeName) const;
    ns3::ApplicationContainer Install(ns3::NodeContainer c) const;

private:
    ns3::Ptr<ns3::Application> InstallPriv(ns3::Ptr<ns3::Node> node) const;

    ns3::ObjectFactory m_factory;
};

}