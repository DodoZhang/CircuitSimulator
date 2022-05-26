#ifndef NET_H
#define NET_H

#include <QList>

namespace CirSim {

class Circuit;
class Pin;

class Net
{
    friend class Circuit;
    friend class Element;
    friend class Pin;
protected:
    Circuit *m_circuit;
    QVector<Pin *> m_pins;
private:
    int m_index;
public:
    Net(Circuit *circuit);
    ~Net();
    Circuit *circuit();
    const QVector<Pin *> &pins() const;
    static Net *merge(Net *net1, Net *net2);
};

}

#endif // NET_H
