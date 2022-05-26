#include "Pin.h"

#include "Element.h"
#include "Net.h"

using namespace CirSim;

Pin::Pin(Element *element, int index)
{
    m_element = element;
    m_net = new Net(element->circuit());
    m_net->m_pins.append(this);
    m_index = index;
}

Pin::~Pin()
{
    m_net->m_pins.removeOne(this);
}

Element *Pin::element() const
{
    return m_element;
}

Net *Pin::net() const
{
    return m_net;
}

void Pin::connect(Pin *pin)
{
    m_net = Net::merge(m_net, pin->m_net);
    pin->m_net = m_net;
}
