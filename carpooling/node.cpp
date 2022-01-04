#include "node.h"

Node::Node(const int &who, const int &where, const Coordinate &co, const double dist)
    : m_who(who), m_where(where), m_co(co), m_pcDist(dist)
{

}

int Node::getWho() const
{
    return this->m_who;
}

int Node::getWhere() const
{
    return this->m_where;
}

Coordinate Node::getCoordinate() const
{
    return this->m_co;
}

double Node::getPrCuDistance() const
{
    return this->m_pcDist;
}

void Node::setPrCuDistance(const double &dist)
{
    this->m_pcDist = dist;
}
