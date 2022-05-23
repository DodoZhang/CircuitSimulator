#ifndef PIN_H
#define PIN_H

namespace CirSim {

class Net;
class Element;

class Pin
{
    friend class Circuit;
    friend class Element;
    friend class Net;
protected:
    Element *m_element;
    Net *m_net;
private:
    int m_index;
public:
    Pin(Element *element, int index);
    ~Pin();
    Element *element() const;
    Net *net() const;
    void connect(Pin *pin) const;
};

}

#endif // PIN_H
