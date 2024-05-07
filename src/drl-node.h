#pragma once

#include <ns3/node.h>

namespace drl {

class DrlNode : public ns3::Node
{
public:
    static ns3::TypeId GetTypeId();

    DrlNode() { }

    double GetLimitRate() const { return m_limitRate; }
    double GetInputRate() const { return m_inputRate; }
    int GetIteration() const { return m_iteration; }
    int GetProcess() const { return m_process; }

    void SetLimitRate(double limitRate) { m_limitRate = limitRate; }
    void SetInputRate(double inputRate) { m_inputRate = inputRate; }
    void SetIteration(int iteration) { m_iteration = iteration; }
    void SetProcess(int process) { m_process = process; }

private:
    double m_limitRate = 0;
    double m_inputRate = 0;
    int m_iteration = 0;
    int m_process = 0;
};

}