#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <QList>

namespace CirSim {

class Element;
class Pin;
class Net;

class Circuit
{
    friend class Element;
    friend class Net;
public:
    enum IterateLevel : unsigned short
    {
        NoIteration = 0,
        AsyncIteration = 1,
        SyncIteration = 2
    };

public:
    int maxIterations;
    double maxAcceptableError;
    IterateLevel iterateLevel;

protected:
    QList<Element *> m_elements;
    QList<Net *> m_nets;
    Net *m_ground;
    double *m_mat;

private:
    bool m_solved;
    int *m_matEleInd;
    int m_matVolInd;
    int m_matSize;

public:
    Circuit();
    ~Circuit();
    void setGround(Net *net);
    const QList<Element *> &elements() const;
    const QList<Net *> &nets() const;
    Net *ground();
    const Net *ground() const;

    bool solve();
    double current(Pin *pin);
    double voltage(Net *net);

    void tick(double time, double deltaTime);

#ifdef QT_DEBUG
    QString debug();
#endif
};

}

#endif // CIRCUIT_H
