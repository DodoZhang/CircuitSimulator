#include "Circuit.h"

#include "Element.h"
#include "Pin.h"
#include "Net.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

using namespace CirSim;

Circuit::Circuit()
{
    m_ground = nullptr;
    m_mat = nullptr;
    m_maxIterations = 64;
    m_maxAcceptableError = 1e-6;
    m_solved = false;
}

Circuit::~Circuit()
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++) delete *iter;
    for (auto iter = m_nets.begin(); iter != m_nets.end(); iter ++) delete *iter;
    delete[] m_mat;
}

void Circuit::setGround(Net *net)
{
    m_ground = net;
}

const QList<Element *> &Circuit::elements() const
{
    return m_elements;
}

const QList<Net *> &Circuit::nets() const
{
    return m_nets;
}

const Net *Circuit::ground() const
{
    return m_ground;
}

#define mat(i, j) (m_mat[(i) * (m_matSize + 1) + (j)])
#define eps (1e-7)

bool Circuit::solve()
{
    int i = 0;
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++, i ++)
        (*iter)->m_index = i;
    i = 0;
    for (auto iter = m_nets.begin(); iter != m_nets.end(); iter ++, i ++)
        (*iter)->m_index = i;
    // Allocte the matrix
    if (!m_mat)
    {
        m_matSize = 0;
        m_matEleInd = new int[m_elements.count()];
        int i = 0;
        for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++, i ++)
        {
            m_matEleInd[i] = m_matSize;
            m_matSize += (*iter)->pinCount();
        }
        m_matVolInd = m_matSize;
        m_matSize += m_nets.count();
        m_mat = new double[m_matSize * (m_matSize + 1)];
        std::fill_n(m_mat, m_matSize * (m_matSize + 1), 0.f);
    }
    // Write the matrix
    int cvrCount = 0;
    i = 0;
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++, i ++)
    {
        for (int j = 0; j < (*iter)->pinCount(); j ++)
        {
            const CVR *cvr = (*iter)->cvr(j);
            memcpy(&mat(cvrCount, m_matEleInd[i]), cvr->current, (*iter)->pinCount() * sizeof(double));
            for (int k = 0; k < (*iter)->pinCount(); k ++)
                mat(cvrCount, m_matVolInd + (*iter)->pin(k)->net()->m_index) += cvr->voltage[k];
            mat(cvrCount, m_matSize) = cvr->offset;
            cvrCount ++;
        }
    }
    mat(cvrCount, m_matVolInd + m_ground->m_index) = 1;
    cvrCount ++;
    for (auto netIter = m_nets.begin() + 1; netIter != m_nets.end(); netIter ++)
    {
        for (auto pinIter = (*netIter)->m_pins.begin(); pinIter != (*netIter)->m_pins.end(); pinIter ++)
            mat(cvrCount, m_matEleInd[(*pinIter)->element()->m_index] + (*pinIter)->m_index) = 1;
        cvrCount ++;
    }
//    for (int i = 0; i < m_matSize; i ++) {
//        for (int j = 0; j < m_matSize + 1; j ++) {
//            std::cout << mat(i, j) << '\t';
//        }
//        std::cout << std::endl;
//    }
    // Solve the matrix
    for (int i = 0; i < m_matSize; i ++)
    {
        if (mat(i, i) > -eps && mat(i, i) < eps)
        {
            for (int k = i + 1; k < m_matSize; k ++)
            {
                if (mat(k, i) <= -eps || mat(k, i) >= eps)
                {
                    // row i <-> row k
                    double *tmp = new double[m_matSize + 1];
                    memcpy(tmp, &mat(i, 0), (m_matSize + 1) * sizeof(double));
                    memcpy(&mat(i, 0), &mat(k, 0), (m_matSize + 1) * sizeof(double));
                    memcpy(&mat(k, 0), tmp, (m_matSize + 1) * sizeof(double));
                    delete[] tmp;
                    if (mat(i, i) < 0)
                    {
                        // row i *= -1
                        for (int j = 0; j < m_matSize + 1; j ++) mat(i, j) = -mat(i, j);
                    }
                    goto eliminate;
                }
            }
            return false;
        }
        eliminate:
        // row i /= mat(i, i)
        double mag = mat(i, i);
        for (int j = 0; j < m_matSize + 1; j ++) mat(i, j) /= mag;
        for (int k = 0; k < m_matSize; k ++)
        {
            if (k == i) continue;
            // row k -= mat(k, i) / mat(i, i) * row i;
            double mag = mat(k, i) / mat(i, i);
            for (int j = 0; j < m_matSize + 1; j ++)
            {
                mat(k, j) -= mag * mat(i, j);
            }
        }
    }
//    for (int i = 0; i < m_matSize; i ++) {
//        for (int j = 0; j < m_matSize + 1; j ++) {
//            std::cout << mat(i, j) << '\t';
//        }
//        std::cout << std::endl;
//    }
    m_solved = true;
    return true;
}

double Circuit::current(Pin *pin)
{
    if (!m_solved) return 0;
    return mat(m_matEleInd[pin->element()->m_index] + pin->m_index, m_matSize);
}

double Circuit::voltage(Net *net)
{
    if (!m_solved) return 0;
    return mat(m_matVolInd + net->m_index, m_matSize);
}

void Circuit::tick(double time, double deltaTime)
{
    for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
        (*iter)->tick(time, deltaTime);
    solve();
    for (int i = 0; i < m_maxIterations; i ++)
    {
        double error = 0;
        for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
            error += (*iter)->error();
        if (error <= m_maxAcceptableError) return;
        for (auto iter = m_elements.begin(); iter != m_elements.end(); iter ++)
            (*iter)->iterate(time, deltaTime);
        solve();
    }
#ifdef QT_DEBUG
    qDebug() << "Unacceptable Error! Please increase Max Iterations or reduce Max Acceptable Error.";
#endif
}