#include "packing.h"
#include <iostream>
Packing::Packing(Driver *dri)
    : m_dri(dri)
{
}

bool Packing::tryAddPassenger(vector<Passenger *> passVec)
{
    int size = passVec.size();

    for (int seat = 0, i = 0; i < size; ++i)
        // capacity constrant
        // seat -- the number of seats has been assigned
        if (this->m_dri->getSeatNum() - seat >= passVec[i]->getSeatNum())
            seat += passVec[i]->getSeatNum();
        else
            return false;

    Route_CFG_OS m_ro = Route_CFG_OS(this->getDriver());
    for (int i = 0; i < size; ++i)
        // waitTime、detour constrants
        if (!m_ro.addPassenger(passVec[i]))
        {
            // passVec[i] is not matched
            return false;
        }
    // passengers in passVec are satisfied with QoS constrants
    this->m_paVec = passVec;
    return true;
}

double Packing::getTotalInitialFare()
{
    double TotalInitialFare = 0;
    for (vector<Passenger *>::iterator it = this->m_paVec.begin(); it != this->m_paVec.end(); ++it)
    {
        TotalInitialFare += (*it)->getInitFare();
    }
    return TotalInitialFare;
}

double Packing::getUtility()
{
    return this->getTotalInitialFare() - this->getDriver()->getBid();
}

Driver *Packing::getDriver() const
{
    return this->m_dri;
}

vector<Passenger *> Packing::getParticipants()
{
    return this->m_paVec;
}

bool Packing::isFeasible()
{
    if(this->getUtility() < 0) return false;
    for (vector<Passenger *>::iterator it = this->m_paVec.begin(); it != this->m_paVec.end(); ++it)
    {
        if ((*it)->getIsSelect())
        {
            return false;
        }
    }
    return true;
}