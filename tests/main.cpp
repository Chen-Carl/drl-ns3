#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/internet-module.h>
#include <ns3/applications-module.h>
#include <ns3/csma-module.h>
#include <ns3/bridge-helper.h>
#include <ns3/on-off-helper.h>

#include "config.h"

int main()
{
    ns3::Time::SetResolution(ns3::Time::NS);
    ns3::LogComponentEnable("OnOffApplication", ns3::LOG_LEVEL_INFO);
    ns3::LogComponentEnable("PacketSink", ns3::LOG_LEVEL_INFO);
    // 1 create topology

    // 1.1. create nodes: 1 switch, 10 hosts
    ns3::NodeContainer hostNodes;
    hostNodes.Create(NS3Config::numNodes + 1);

    ns3::NodeContainer switchNodes;
    switchNodes.Create(1);

    ns3::CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", ns3::DataRateValue(NS3Config::csmaDataRate));
    csma.SetChannelAttribute("Delay", ns3::TimeValue(ns3::MilliSeconds(NS3Config::csmaDelay)));

    // 1.2 create csma connection
    /**
     *   hostNodes (hostDevice)                          (bridge) switchNodes
     * 
     * host 0 (hostDevice 0: 10.1.1.1)  ---------
     *                                          |
     * host 1 (hostDevice 1: 10.1.1.2)  ----------------- (bridge) switch 0
     * ...                                      |
     * host 9 (hostDevice 9: 10.1.1.10) ---------
     *                                          |
     * host 10 (hostDevice 10: 10.1.1.11) -------   <-- sending Application
     */
    ns3::NetDeviceContainer hostDevices;
    ns3::NetDeviceContainer switchDevices;

    for (int i = 0; i < hostNodes.GetN(); i++)
    {
        ns3::NetDeviceContainer link = csma.Install(ns3::NodeContainer(switchNodes.Get(0), hostNodes.Get(i)));
        switchDevices.Add(link.Get(0));
        hostDevices.Add(link.Get(1));
    }

    ns3::BridgeHelper bridge;
    bridge.Install(switchNodes.Get(0), switchDevices);

    // 1.3 install internet stack
    ns3::InternetStackHelper internet;
    internet.Install(hostNodes);

    ns3::Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    ipv4.Assign(hostDevices);

    // 2. data stream

    // 2.1 sending

    /**
     * host 10
     * 
     * onoff 0 --> host 0 (PacketSinkApplication 10.1.1.1:9)
     * 
     * onoff 1 --> host 1 (PacketSinkApplication 10.1.1.2:9)
     * ...
     * onoff 9 --> host 9 (PacketSinkApplication 10.1.1.9:9)
     */
    ns3::OnOffHelper onoff("ns3::UdpSocketFactory", ns3::Address());
    onoff.SetAttribute("OnTime", ns3::StringValue("ns3::ConstantRandomVariable[Constant=1]"));  // always on
    onoff.SetAttribute("OffTime", ns3::StringValue("ns3::ConstantRandomVariable[Constant=0]")); // always off
    onoff.SetAttribute("PacketSize", ns3::UintegerValue(NS3Config::packetSize));

    ns3::PacketSinkHelper sink("ns3::UdpSocketFactory", ns3::Address());

    ns3::ApplicationContainer sendApps;
    ns3::ApplicationContainer recvApps;

    for (int i = 0; i < hostNodes.GetN() - 1; i++)
    {
        ns3::Ipv4Address ipv4Address = hostNodes.Get(i)->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();
        ns3::AddressValue hostAddress(ns3::InetSocketAddress(ipv4Address, NS3Config::port));

        onoff.SetAttribute("Remote", hostAddress);
        onoff.SetAttribute("DataRate", ns3::DataRateValue(NS3Config::onoffDataRates[i]));
        sendApps.Add(onoff.Install(hostNodes.Get(hostNodes.GetN() - 1)));

        sink.SetAttribute("Local", hostAddress);
        recvApps.Add(sink.Install(hostNodes.Get(i)));
    }

    recvApps.Start(ns3::Seconds(0.0));
    recvApps.Stop(ns3::Seconds(10.0));
    sendApps.Start(ns3::Seconds(1.0));
    sendApps.Stop(ns3::Seconds(10.0));
    
    ns3::Simulator::Run();
    ns3::Simulator::Destroy();

    return 0;
}