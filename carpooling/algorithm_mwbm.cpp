#include "algorithm_mwbm.h"

Algorithm_MWBM::Algorithm_MWBM(const vector<Driver *> &driSet, const vector<vector<Passenger *>> &dcaMat, const vector<Passenger *> &passSet, const vector<vector<Driver *>> &pcaMat) : m_driSet(driSet), m_dcaMat(dcaMat), m_passSet(passSet), m_pcaMat(pcaMat)
{
    init();
    oneToManyMatching();
    pricing();
}

Algorithm_MWBM::~Algorithm_MWBM()
{
    for (vector<Coalition_CFG_OS_2 *>::iterator it = this->m_gVec.begin(); it != this->m_gVec.end(); ++it)
        delete (*it);
}

void Algorithm_MWBM::init()
{
    // generation a coalition for each driver
    int size = this->m_driSet.size();
    for (int i = 0; i < size; i++)
    {
        Coalition_CFG_OS_2 *c = new Coalition_CFG_OS_2(this->m_driSet[i]);
        this->m_gVec.push_back(c);
    }
}

void Algorithm_MWBM::pricing()
{
    for (vector<Coalition_CFG_OS_2 *>::iterator it = this->m_gVec.begin(); it != this->m_gVec.end(); it++)
    {
        Driver *d = (*it)->getDriver();
        vector<Passenger *> passVec = (*it)->getParticipants();
        bool flag = true;
        double totalPayment = 0;
        for (vector<Passenger *>::iterator pit = passVec.begin(); pit != passVec.end(); pit++)
        {
            double payment = ((*pit)->getDist()*PRICE_PER_KILOMETER + (*pit)->getDist()/SPEED*PRICE_PER_MINUTE)*DISTCOUNT;
            if ((*pit)->getBid() < payment)
            {
                flag = false;
                break;
            }
            totalPayment += payment;
            (*pit)->setFare(payment);
        }
        if (flag && totalPayment >= d->getBid())
        {
            this->determineRoles(*it, true);
        }
        else
        {
            this->determineRoles(*it, false);
        }
    }
}

void Algorithm_MWBM::determineRoles(Coalition_CFG_OS_2 *gr, bool isWin)
{
    vector<Passenger *> passVec = gr->getParticipants();
    Driver *d = gr->getDriver();
    for (vector<Passenger *>::iterator it = passVec.begin(); it != passVec.end(); ++it)
    {
        if (isWin)
        {
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
    if (isWin)
    {
        //buyout ask price
        d->setPayoff(d->getBid());
        //the utility of driver = buyout ask price - detour cost
        d->setUti(d->getPayoff() - d->getDetourCost());
        d->setIsShare(true);
    }
    else
    {
        d->setUti(0);
        d->setIsShare(false);
        d->setPayoff(0);
    }
}

void Algorithm_MWBM::oneToManyMatching()
{
    //D: the set of domiating pair
    map<int, int> D;
    /* find the domiating pair: 
    driver and passenger are mutually best match for each other*/
    for (vector<Driver *>::iterator it = this->m_driSet.begin(); it != this->m_driSet.end(); it++)
    {
        Passenger *bestp = bm(*it);
        if (!bestp)
            continue;
        Driver *bestd = bm(bestp);
        if (bestd->getId() == (*it)->getId())
        {
            D.insert(map<int, int>::value_type(bestp->getId(), bestd->getId()));
        }
    }
    for (map<int, int>::iterator it = D.begin(); it != D.end(); it++)
    {
        Coalition_CFG_OS_2 *gr = this->m_gVec[it->second - 1];
        Passenger *pass = this->m_passSet[it->first - 1];
        gr->tryAddPassenger(pass, true);
        gr->update(pass, true);
    }

    while (!D.empty())
    {
        map<int, int>::iterator it = D.begin();
        int driverID = it->second;
        //iterate through all the candidate passengers of driverID
        vector<Passenger *> passVec = this->m_dcaMat[driverID - 1];
        for (vector<Passenger *>::iterator pit = passVec.begin(); pit != passVec.end(); pit++)
        {
            if ((*pit)->getIsSelect())
                continue;
            Driver *bestd = bm(*pit);
            if (!bestd)
                continue;
            Passenger *bestp = bm(bestd);
            if (bestp->getId() == (*pit)->getId())
            {
                Coalition_CFG_OS_2 *gr = this->m_gVec[bestd->getId() - 1];
                gr->tryAddPassenger(*pit, true);
                gr->update(*pit, true);
                D.insert(map<int, int>::value_type(bestp->getId(), bestd->getId()));
            }
        }
        // remove one pair
        D.erase(it->first);
    }
}

Driver *Algorithm_MWBM::bm(Passenger *p)
{
    vector<Driver *> driVec = this->m_pcaMat[p->getId() - 1];
    double dist = 1e6;
    Driver *bestDriver = nullptr;
    for (vector<Driver *>::iterator it = driVec.begin(); it != driVec.end(); it++)
    {
        Coalition_CFG_OS_2 *gr = this->m_gVec[(*it)->getId() - 1];
        double old_dist = max((*it)->getDist(), (*it)->getInitDist());
        if (gr->tryAddPassenger(p, true)) 
        {
            double incrementDist = (*it)->getDist() - old_dist;
            if (incrementDist < dist)
            {
                dist = incrementDist;
                bestDriver = *it;
            }
            gr->restore();
        }
    }
    return bestDriver;
}

Passenger *Algorithm_MWBM::bm(Driver *d)
{
    vector<Passenger *> passVec = this->m_dcaMat[d->getId() - 1];
    Coalition_CFG_OS_2 *gr = this->m_gVec[d->getId() - 1];
    double dist = 1e6;
    Passenger *bestPassenger = nullptr;
    double old_dist = max(d->getDist(), d->getInitDist());
    for (vector<Passenger *>::iterator it = passVec.begin(); it != passVec.end(); it++)
    {
        if ((*it)->getIsSelect())
            continue;
        
        if (gr->tryAddPassenger(*it, true))
        {
            double incrementDist = d->getDist() - old_dist;
            if (incrementDist < dist)
            {
                dist = incrementDist;
                bestPassenger = *it;
            }
            gr->restore();
        }
    }
    return bestPassenger;
}
