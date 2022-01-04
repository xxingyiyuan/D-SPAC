#include "coalition_cfg_os_2.h"
#include <iostream>
Coalition_CFG_OS_2::Coalition_CFG_OS_2(Driver *dri)
    : m_dri(dri), m_ro(dri), m_unitBid(0), m_tro(dri), m_isCache(false)
{
}

bool Coalition_CFG_OS_2::tryAddPassenger(Passenger *pass, bool isCache)
{
    // Capacity constraint
    if (this->m_dri->getSeatNum() - this->m_dri->getSharedSeatNum() < pass->getSeatNum())
        return false;
    //cache
    if (isCache)
    {
        this->m_tro = this->m_ro;
        this->m_tpaVec = this->m_paVec;
        this->m_isCache = true;
    }

    if (this->m_ro.addPassenger(pass))
    {
        //add new passenger
        this->m_paVec.push_back(pass);
        this->m_ro.updateAllInfos();
        calBids();
        return true;
    }
    else
        return false;
}

bool Coalition_CFG_OS_2::tryAddPassenger(vector<Passenger *> passVec)
{

    int size = passVec.size();

    for (int seat = 0, i = 0; i < size; ++i)
        // capacity constrant
        // seat -- the number of seats has been assigned
        if (this->m_dri->getSeatNum() - seat >= passVec[i]->getSeatNum())
            seat += passVec[i]->getSeatNum();
        else
            return false;

    for (int i = 0; i < size; ++i)
        // waitTime、detour constrants
        if (!this->m_ro.addPassenger(passVec[i]))
        {
            // passVec[i] is not matched
            this->m_ro = Route_CFG_OS(this->m_dri);
            return false;
        }
    // passengers in passVec are satisfied with QoS constrants
    this->m_paVec = passVec;
    for (int i = 0; i < size; ++i)
    {
        this->m_paVec[i]->setIsSelect(true);
        //m_dri->getId() starts from 1
        this->m_paVec[i]->setCollectiveId(this->m_dri->getId());
    }
    //Update the information of all involved users and the route
    this->m_ro.updateAllInfos();
    //Calculate the unit bid of group and the bid of driver
    calBids();

    return true;
}

bool Coalition_CFG_OS_2::tryRemovePassenger(Passenger *pass, bool isCache)
{
    if (isCache)
    {
        this->m_tro = this->m_ro;
        this->m_tpaVec = this->m_paVec;
        this->m_isCache = true;
    }
    this->m_ro = Route_CFG_OS(this->m_dri);
    this->m_paVec.clear();
    for (vector<Passenger *>::iterator it = this->m_tpaVec.begin(); it != this->m_tpaVec.end(); ++it)
    {
        if ((*it)->getId() == pass->getId())
            continue;
        if (!this->m_ro.addPassenger(*it))
        {
            return false;
        }
        this->m_paVec.push_back(*it);
    }
    this->m_ro.updateAllInfos();
    calBids();
    return true;
}

void Coalition_CFG_OS_2::update(Passenger *pass, const bool &select)
{
    if (select)
        pass->setIsSelect(true);
    else
    {

        pass->setDist(0);
        pass->setTime(0);
        pass->setFare(0);
        pass->setUti(0);
        pass->setIsShare(false);
        pass->setIsSelect(false);
        pass->setCollectiveId(0);
    }

    this->m_tro = Route_CFG_OS(this->m_dri);
    this->m_tpaVec.clear();
    this->m_isCache = false;
}

void Coalition_CFG_OS_2::restore()
{
    if (this->m_isCache)
    {

        if (this->m_paVec.size() || this->m_tpaVec.size())
        {

            this->m_ro = this->m_tro;
            this->m_paVec = this->m_tpaVec;
            this->m_ro.updateAllInfos();
        }
    }
    calBids();
    this->m_tro = Route_CFG_OS(this->m_dri);
    this->m_tpaVec.clear();
    this->m_isCache = false;
}



void Coalition_CFG_OS_2::calBids()
{
    if (this->m_paVec.size())
    {
        this->m_unitBid = 1e10;
        for(vector<Passenger*>::iterator it = this->m_paVec.begin(); it != this->m_paVec.end(); ++it)
            if( (this->m_unitBid - (*it)->getBid() ) > 1e-6)
                this->m_unitBid = (*it)->getBid();
        //the share dist
        double dist = this->m_ro.getShareTravelCost();
        this->m_dri->setCost(dist * FUEL_PRICE_PER_KILOMETER + dist / SPEED * LABOUR_PRICE_PER_MINUTE);
    }
    else
    {
        this->m_unitBid = 0;
        this->m_dri->setCost(0);
    }
}

void Coalition_CFG_OS_2::determineRoles(const double &unitPayment, const bool &isWin)
{
    for (vector<Passenger *>::iterator it = this->m_paVec.begin(); it != this->m_paVec.end(); ++it)
    {
        // update the CollectiveId of passengers
        (*it)->setCollectiveId(this->m_dri->getId());

        if (isWin)
        {
            (*it)->setFare(unitPayment);
            (*it)->setUti((*it)->getInitFare() - (*it)->getFare());
            (*it)->setIsShare(true);
        }
        else
        {
            (*it)->setFare(0);
            (*it)->setUti(0);
            (*it)->setIsShare(false);
        }
    }
    //--edit
    Driver *d = this->m_dri;
    if (isWin)
    {
        //buyout ask price
        d->setPayoff(d->getBid());
        //the utility of driver = buyout ask price - detour cost
        d->setUti(this->m_dri->getPayoff() - this->m_dri->getDetourCost());
        d->setIsShare(true);
    }
    else
    {
        d->setUti(0);
        d->setIsShare(false);
        d->setPayoff(0);
    }
}

double Coalition_CFG_OS_2::getTotalInitialFare()
{
    if (!this->m_paVec.size())
        return 0;
    double TotalInitialFare = 0;
    for (vector<Passenger *>::iterator it = this->m_paVec.begin(); it != this->m_paVec.end(); ++it)
    {
        TotalInitialFare += (*it)->getInitFare();
    }
    return TotalInitialFare;
}


int Coalition_CFG_OS_2::getId() const
{
    return this->m_dri->getId();
}

int Coalition_CFG_OS_2::getSelectNum() const
{
    return this->m_paVec.size();
}

double Coalition_CFG_OS_2::getUnitBid() const
{
    return this->m_unitBid;
}

Driver *Coalition_CFG_OS_2::getDriver() const
{
    return this->m_dri;
}

vector<Passenger *> Coalition_CFG_OS_2::getParticipants()
{
    return this->m_paVec;
}