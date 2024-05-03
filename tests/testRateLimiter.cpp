#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>

#include <format>

#include "config.h"
#include "rpc-server-helper.h"
#include "rpc-client-helper.h"
#include "rpc-client.h"

void recordTotalRx(ns3::Ptr<ns3::PacketSink> sinkApp);
void setContinueRecord(bool cont);

int main()
{
    ns3::Time::SetResolution(ns3::Time::NS);
    // ns3::LogComponentEnable("OnOffApplication", ns3::LOG_LEVEL_INFO);
    // ns3::LogComponentEnable("PacketSink", ns3::LOG_LEVEL_INFO);
    ns3::LogComponentEnable("RpcClientApplication", ns3::LOG_LEVEL_INFO);

    // topology
    /**
     *                p2p (datarate = 100000bps, delay = 1ms)
     *      nodes 0 <----------------------------------------> nodes 1 
     *  -------------------------
     *  |  OnOffApplication     |                           PacketSink
     *  |  datarate = 5000bps   |                       RpcServerApplication
     *  |  packet = 64B         | 
     *  -------------------------
     */

    ns3::NodeContainer nodes;
    nodes.Create(2);

    ns3::PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", ns3::DataRateValue(NS3Config::csmaDataRate));
    pointToPoint.SetChannelAttribute("Delay", ns3::TimeValue(ns3::MilliSeconds(NS3Config::csmaDelay)));

    ns3::NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    ns3::InternetStackHelper stack;
    stack.Install(nodes);

    ns3::Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    ns3::Ipv4InterfaceContainer interfaces = address.Assign(devices);

    ns3::Ipv4Address node0Address = nodes.Get(0)->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    ns3::AddressValue clientAddress(ns3::InetSocketAddress(node0Address, 9));

    ns3::Ipv4Address node1Address = nodes.Get(1)->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
    ns3::AddressValue serverAddress(ns3::InetSocketAddress(node1Address, 9));

    ns3::OnOffHelper onoff("ns3::UdpSocketFactory", ns3::Address());
    onoff.SetAttribute("OnTime", ns3::StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", ns3::StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    onoff.SetAttribute("DataRate", ns3::DataRateValue(ns3::DataRate(NS3Config::onoffDataRate)));
    onoff.SetAttribute("PacketSize", ns3::UintegerValue(NS3Config::packetSize));
    onoff.SetAttribute("Remote", serverAddress);

    ns3::ApplicationContainer sendApps = onoff.Install(nodes.Get(0));
    sendApps.Start(ns3::Seconds(2.0));
    sendApps.Stop(ns3::Seconds(3.0));

    ns3::PacketSinkHelper sink("ns3::UdpSocketFactory", ns3::InetSocketAddress(node1Address, 9));

    ns3::ApplicationContainer recvApps = sink.Install(nodes.Get(1));
    recvApps.Start(ns3::Seconds(0.0));
    recvApps.Stop(ns3::Seconds(10.0));

    drl::RpcServerHelper rpcServerHelper;
    ns3::ApplicationContainer rpcServers = rpcServerHelper.Install(nodes.Get(1));
    rpcServers.Start(ns3::Seconds(0.0));
    rpcServers.Stop(ns3::Seconds(10.0));

    drl::RpcClientHelper rpcClientHelper;
    ns3::ApplicationContainer rpcClients = rpcClientHelper.Install(nodes.Get(0));
    rpcClients.Start(ns3::Seconds(0.0));
    rpcClients.Stop(ns3::Seconds(10.0));

    ns3::Ptr<drl::RpcClientApplication> rpcClient = ns3::DynamicCast<drl::RpcClientApplication>(rpcClients.Get(0));

    ns3::Simulator::Schedule(ns3::Seconds(5), &drl::RpcClientApplication::SendRpcRequest, rpcClient, nodes.Get(1));

    ns3::Ptr<ns3::PacketSink> sinkApp = ns3::DynamicCast<ns3::PacketSink>(recvApps.Get(0));

    ns3::EventId event = ns3::Simulator::Schedule(ns3::Seconds(1), &recordTotalRx, sinkApp);

    ns3::Simulator::Schedule(ns3::Seconds(10), &setContinueRecord, false);

    ns3::Simulator::Run();
    ns3::Simulator::Destroy();
    return 0;
}

bool continueRecording = true;

void setContinueRecord(bool cont)
{
    continueRecording = cont;
}

void recordTotalRx(ns3::Ptr<ns3::PacketSink> sinkApp) 
{
    if (continueRecording)
    {
        uint64_t totalRx = sinkApp->GetTotalRx();
        double now = ns3::Simulator::Now().GetMicroSeconds() / 1000000.0;
        std::cout << std::format("[{:.6}] total rx = {}", now, totalRx) << std::endl;
        ns3::Simulator::Schedule(ns3::Seconds(1), &recordTotalRx, sinkApp);
    }
};