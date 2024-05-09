#include "drl-node.h"
#include "config.h"

#include <ns3/core-module.h>

namespace drl {

NS_OBJECT_ENSURE_REGISTERED(DrlNode);

ns3::TypeId DrlNode::GetTypeId()
{
    static ns3::TypeId tid = ns3::TypeId("drl::DrlNode")
        .SetParent<ns3::Node>()
        .AddConstructor<DrlNode>()
        .AddAttribute("LimitRate", "The limit rate of the node.", ns3::DoubleValue(1.0 / NS3Config::numNodes), ns3::MakeDoubleAccessor(&DrlNode::m_limitRate), ns3::MakeDoubleChecker<double>())
        .AddAttribute("InputRate", "The input rate of the node.", ns3::DoubleValue(0.0), ns3::MakeDoubleAccessor(&DrlNode::m_inputRate), ns3::MakeDoubleChecker<double>())
        .AddAttribute("Iteration", "The iteration of the DRL Algorithm.", ns3::IntegerValue(0), ns3::MakeIntegerAccessor(&DrlNode::m_iteration), ns3::MakeIntegerChecker<int>())
        .AddAttribute("Process", "The processed neighbour nodes of the current iteration.", ns3::IntegerValue(0), ns3::MakeIntegerAccessor(&DrlNode::m_process), ns3::MakeIntegerChecker<int>())
        .AddAttribute("Status", "The status of the node.", ns3::IntegerValue(0), ns3::MakeIntegerAccessor(&DrlNode::m_status), ns3::MakeIntegerChecker<int>())
        .AddTraceSource("LimitRateTracer", "The limit rate of the node.", ns3::MakeTraceSourceAccessor(&DrlNode::m_limitRate), "ns3::TracedValueCallback::Double");
    return tid;
}

}