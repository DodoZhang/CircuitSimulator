#include "Net.h"

#include "Circuit.h"
#include "Pin.h"

using namespace CirSim;

Net::Net(Circuit *circuit)
{
    m_circuit = circuit;
    m_circuit->m_nets.append(this);
}

Net::~Net()
{
    m_circuit->m_nets.removeOne(this);
    for (int i = 0; i < m_pins.count(); i ++)
        m_pins[i]->m_net = new Net(m_circuit);
}

Circuit *Net::circuit()
{
    return m_circuit;
}

const QVector<Pin *> &Net::pins() const
{
    return m_pins;
}

void Net::merge(Net *net)
{
    for (int i = 0; i < net->m_pins.count(); i ++)
        net->m_pins[i]->m_net = this;
    m_pins.append(net->m_pins);
    net->m_pins.clear();
    delete net;
}
