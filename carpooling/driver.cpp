#include "driver.h"

Driver::Driver(const int &id, const Demand &de, const double &ratio)
    : m_id(id), m_de(de), m_sDist(0), m_sTime(0), m_sCost(0),
      m_sPayoff(0), m_sSeat(0), m_selectNum(0), m_bid(0), m_uti(0), m_isShare(false), m_ratio(ratio)
{
    this->m_iDist = this->m_de.getOrg().calDist(this->m_de.getDst());
    this->m_iTime = this->m_iDist / SPEED;
    this->m_bid = this->getInitDist() * FUEL_PRICE_PER_KILOMETER + this->getInitTime() * LABOUR_PRICE_PER_MINUTE;
}

int Driver::getId() const
{
    return this->m_id;
}

Coordinate Driver::getOrg() const
{
    return this->m_de.getOrg();
}

Coordinate Driver::getDst() const
{
    return this->m_de.getDst();
}

double Driver::getWaitTime() const
{
    return this->m_de.getWaitTime();
}

double Driver::getDeRatio() const
{
    return this->m_de.getDeRatio();
}

int Driver::getSeatNum() const
{
    return this->m_de.getSeatNum();
}

double Driver::getInitDist() const
{
    return this->m_iDist;
}

double Driver::getInitTime() const
{
    return this->m_iTime;
}

double Driver::getDist() const
{
    return this->m_sDist;
}

double Driver::getTime() const
{
    return this->m_sTime;
}

double Driver::getCost() const
{
    return this->m_sCost;
}
double Driver::getDetourCost() const
{
    return this->getCost() - this->getBid();
}

double Driver::getPayoff() const
{
    return this->m_sPayoff;
}

int Driver::getSharedSeatNum() const
{
    return this->m_sSeat;
}
int Driver::getSelectNum() const
{
    return this->m_selectNum;
}

double Driver::getBid() const
{
    return this->m_bid;
}

double Driver::getUti() const
{
    return this->m_uti;
}

bool Driver::getIsShare() const
{
    return this->m_isShare;
}

void Driver::setDist(const double &dist)
{
    this->m_sDist = dist;
}

void Driver::setTime(const double &time)
{
    this->m_sTime = time;
}

void Driver::setCost(const double &cost)
{
    this->m_sCost = cost;
}

void Driver::setPayoff(const double &payoff)
{
    this->m_sPayoff = payoff;
}

void Driver::setSharedSeatNum(const int &seat)
{
    this->m_sSeat = seat;
}
void Driver::setSelectNum(const int &selectNum)
{
    this->m_selectNum = selectNum;
}

void Driver::setBid(const double &bid)
{
    this->m_bid = bid;
}

void Driver::setUti(const double &uti)
{
    this->m_uti = uti;
}

void Driver::setIsShare(const bool &isShare)
{
    this->m_isShare = isShare;
}
