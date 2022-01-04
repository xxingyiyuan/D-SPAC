#include "demand.h"

Demand::Demand(const Coordinate &org, const Coordinate &dst, const double &waitTime, const double &deRatio, const int &seatNum)
    : m_org(org), m_dst(dst), m_waitTime(waitTime), m_deRatio(deRatio), m_seatNum(seatNum)
{

}

Coordinate Demand::getOrg() const
{
    return this->m_org;
}

Coordinate Demand::getDst() const
{
    return this->m_dst;
}

double Demand::getWaitTime() const
{
    return this->m_waitTime;
}

double Demand::getDeRatio() const
{
    return this->m_deRatio;
}

double Demand::getSeatNum() const
{
    return this->m_seatNum;
}
