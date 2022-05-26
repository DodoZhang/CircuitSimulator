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

Net *Net::merge(Net *net1, Net *net2)
{
    if (net1 == net2) return net1;
    if (net2 != net1->m_circuit->ground())
    {
        for (int i = 0; i < net2->m_pins.count(); i ++)
            net2->m_pins[i]->m_net = net1;
        net1->m_pins.append(net2->m_pins);
        net2->m_pins.clear();
        delete net2;
        return net1;
    }
    else return merge(net2, net1);
}
