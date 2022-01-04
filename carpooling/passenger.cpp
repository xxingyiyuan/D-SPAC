#include "passenger.h"

Passenger::Passenger(const int &id, const Demand &de, const double &ratio)
    : m_id(id), m_de(de), m_sDist(0), m_sTime(0),
      m_sFare(0), m_uti(0), m_isShare(false),
      m_isSelect(false), m_coId(0)
{
    this->m_iDist = this->m_de.getOrg().calDist(this->m_de.getDst() );
    this->m_iTime = this->m_iDist / SPEED;
    this->m_iFare = (this->m_iDist * PRICE_PER_KILOMETER + this->m_iTime * PRICE_PER_MINUTE);

    this->m_bid = this->m_iFare;
}

int Passenger::getId() const
{
    return this->m_id;
}

Coordinate Passenger::getOrg() const
{
    return this->m_de.getOrg();
}

Coordinate Passenger::getDst() const
{
    return this->m_de.getDst();
}

double Passenger::getWaitTime() const
{
    return this->m_de.getWaitTime();
}

double Passenger::getDeRatio() const
{
    return this->m_de.getDeRatio();
}

int Passenger::getSeatNum() const
{
    return this->m_de.getSeatNum();
}

double Passenger::getInitDist() const
{
    return this->m_iDist;
}

double Passenger::getInitTime() const
{
    return this->m_iTime;
}

double Passenger::getInitFare() const
{
    return this->m_iFare;
}

double Passenger::getDist() const
{
    return this->m_sDist;
}

double Passenger::getTime() const
{
    return this->m_sTime;
}

double Passenger::getFare() const
{
    return this->m_sFare;
}

double Passenger::getBid() const
{
    return this->m_bid;
}

double Passenger::getUti() const
{
    return this->m_uti;
}

bool Passenger::getIsShare() const
{
    return this->m_isShare;
}

bool Passenger::getIsSelect() const
{
    return this->m_isSelect;
}

int Passenger::getCollectiveId() const
{
    return this->m_coId;
}

void Passenger::setDist(const double &dist)
{
    this->m_sDist = dist;
}

void Passenger::setTime(const double &time)
{
    this->m_sTime = time;
}

void Passenger::setFare(const double &fare)
{
    this->m_sFare = fare;
}

void Passenger::setUti(const double &uti)
{
    this->m_uti = uti;
}

void Passenger::setIsShare(const bool &isShare)
{
    this->m_isShare = isShare;
}

void Passenger::setIsSelect(const bool &isSelect)
{
    this->m_isSelect = isSelect;
}

void Passenger::setCollectiveId(const int &id)
{
    this->m_coId = id;
}
