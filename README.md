# Distributed Rate Limiting Simulation

## 1 System Prerequisites

### NS-3 Simulator

The simulator uses `PkgConfig` to load NS-3 libraries.


## 2 RPC Model

### Install

Install an RpcClientApplication on node 0.

``` cpp
drl::RpcClientHelper rpcClientHelper;
ns3::ApplicationContainer rpcClients = rpcClientHelper.Install(nodes.Get(0));
```

Install an RpcServerApplication on node 1. RpcServerApplication binds a local address.

``` cpp
ns3::Ipv4Address nodeAddress = nodes.Get(0)->GetObject<ns3::Ipv4>()->GetAddress(1, 0).GetLocal();   // get ipv4 address of node 0
drl::RpcServerHelper rpcServerHelper(nodeAddress);  // create application with helper
ns3::ApplicationContainer rpcServers = rpcServerHelper.Install(nodes.Get(0));   // install application on node 0
```


