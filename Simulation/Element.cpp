#include "Element.h"

#include "Circuit.h"
#include "Pin.h"
#include "Net.h"

using namespace CirSim;

CVR::CVR(const Element *element)
{
    current = new double[element->pinCount()];
    voltage = new double[element->pinCount()];
}

CVR::~CVR()
{
    delete current;
    delete voltage;
}

Element::Element(Circuit *circuit)
{
    m_circuit = circuit;
    m_circuit->m_elements.append(this);
}

Element::~Element()
{
    m_circuit->m_elements.removeOne(this);
}

int Element::pinCount() const
{
    return 0;
}

Pin *Element::pin(int)
{
    return nullptr;
}

const Pin *Element::pin(int) const
{
    return nullptr;
}

Circuit *Element::circuit()
{
    return m_circuit;
}

void Element::tick(double, double)
{
    // Do nothing by default
}

void Element::iterate(double, double)
{
    // Do nothing by default
}

double Element::error()
{
    return 0;
}

#ifdef QT_DEBUG
QString Element::debug()
{
    return "Unknow Element";
}
#endif
