#include "drl-node.h"

#include <ns3/core-module.h>

namespace drl {

NS_OBJECT_ENSURE_REGISTERED(DrlNode);

ns3::TypeId DrlNode::GetTypeId()
{
    static ns3::TypeId tid = ns3::TypeId("drl::DrlNode")
        .SetParent<ns3::Node>()
        .AddConstructor<DrlNode>()
        .AddAttribute("LimitRate", "The limit rate of the node.", ns3::DoubleValue(0.0), ns3::MakeDoubleAccessor(&DrlNode::m_limitRate), ns3::MakeDoubleChecker<double>())
        .AddAttribute("InputRate", "The input rate of the node.", ns3::DoubleValue(0.0), ns3::MakeDoubleAccessor(&DrlNode::m_inputRate), ns3::MakeDoubleChecker<double>())
        .AddAttribute("Iteration", "The iteration of the DRL Algorithm.", ns3::IntegerValue(0), ns3::MakeIntegerAccessor(&DrlNode::m_iteration), ns3::MakeIntegerChecker<int>())
        .AddAttribute("Process", "The processed neighbour nodes of the current iteration.", ns3::IntegerValue(0), ns3::MakeIntegerAccessor(&DrlNode::m_process), ns3::MakeIntegerChecker<int>());
    return tid;
}

}