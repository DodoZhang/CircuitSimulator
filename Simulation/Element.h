#ifndef ELEMENT_H
#define ELEMENT_H

namespace CirSim {

class Circuit;
class Element;
class Pin;

typedef struct CVR
{
    double *current;
    double *voltage;
    double offset;

    CVR(const Element *element);
    ~CVR();
} CVR;

class Element
{
    friend class Circuit;
protected:
    Circuit *m_circuit;
private:
    int m_index;
public:
    Element(Circuit *circuit);
    virtual ~Element();
    Circuit *circuit();
    virtual int pinCount() const;
    virtual Pin *pin(int index);
    virtual const Pin *pin(int index) const;
    // An Element must have <pinCount> sets of CVR equations
    virtual const CVR *cvr(int index) const = 0;
    virtual void tick(double time, double deltaTime);
    virtual void iterate(double time, double deltaTime);
    virtual double error();
};

}

#endif // ELEMENT_H
