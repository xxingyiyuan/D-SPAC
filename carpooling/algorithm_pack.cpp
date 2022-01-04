#include "algorithm_pack.h"
#include <algorithm>
Algorithm_PACK::Algorithm_PACK(const vector<Driver *> &driSet, const vector<vector<Passenger *>> &dcaMat, const vector<Passenger *> &passSet, const vector<vector<Driver *>> &pcaMat) : m_driSet(driSet), m_dcaMat(dcaMat), m_passSet(passSet), m_pcaMat(pcaMat), m_au(), m_time(0)
{
    //1. packing two passengers for each driver
    init();
    double opf = this->m_au.auction2(this->m_driSet, this->m_gVec);
    this->update();
    //2. add remaining passengers into feasible coalition
    for (vector<Passenger *>::iterator passIt = this->m_passSet.begin(); passIt != this->m_passSet.end(); ++passIt)
    {
        Passenger *pass = (*passIt);
        if (!(*passIt)->getIsSelect())
        {
            vector<Driver *> pcaVec = this->m_pcaMat.at(pass->getId() - 1);
            if (!pcaVec.size())
                continue;
            notInGroupBranch(pass, pcaVec, opf);
        }
    }
    opf = this->m_au.auction2(this->m_driSet, this->m_gVec);
    this->update();
    this->clear();
}

Algorithm_PACK::~Algorithm_PACK()
{
    for (vector<Coalition_CFG_OS_2 *>::iterator it = this->m_gVec.begin(); it != this->m_gVec.end(); ++it)
        delete (*it);
}

void Algorithm_PACK::notInGroupBranch(Passenger *pass, vector<Driver *> pcaVec, double &opf)
{
    Coalition_CFG_OS_2 *select = nullptr;
    for (vector<Driver *>::iterator it = pcaVec.begin(); it != pcaVec.end(); ++it)
    {
        if((*it)->getSelectNum() == 0){
            continue;
        }
        Coalition_CFG_OS_2 *gr = this->m_gVec.at((*it)->getId() - 1);
        if (joinOperation(pass, gr))
        {
            gr->update(pass, true);
            break;
        }
    }
}

bool Algorithm_PACK::joinOperation(Passenger *pass, Coalition_CFG_OS_2 *gr)
{
    return gr->tryAddPassenger(pass, true);
}

void Algorithm_PACK::init()
{
    //generate a coalition for each driver
    int size = this->m_driSet.size();
    for (int i = 0; i < size; ++i)
    {
        Coalition_CFG_OS_2 *gr = new Coalition_CFG_OS_2(this->m_driSet[i]);
        this->m_gVec.push_back(gr);
    }
    //1. find the feasible packing for each driver
    vector<Packing *> caPacking = findPackings();
    //2. sort the packing in non-descending order, back() is the maximum element
    sort(caPacking.begin(), caPacking.end(),ComHelper());
    //3. select the packing one by one, if feasible, add them into the coalition of its matched driver.
    while (caPacking.size())
    {
        //get the last element
        Packing *pack = caPacking.back();
        //remove the last one
        caPacking.pop_back();
        //driver has been selected
        if (pack->getDriver()->getSharedSeatNum())
            continue;
        //one or both the passengers in pack have been selected
        if (!pack->isFeasible())
            continue;
        //update
        this->m_gVec[pack->getDriver()->getId() - 1]->tryAddPassenger(pack->getParticipants());
    }
}

void Algorithm_PACK::update()
{
    vector<Coalition_CFG_OS_2 *> grWinner = this->m_au.getGroupWinners();
    vector<double> unitPayment = this->m_au.getUnitPayments();

    int size = grWinner.size();
    for (int i = 0; i < size; ++i)
        grWinner[i]->determineRoles(unitPayment[i], true);

    vector<Coalition_CFG_OS_2 *> grLoser = this->m_au.getGroupLosers();
    size = grLoser.size();
    for (vector<Coalition_CFG_OS_2 *>::iterator it = grLoser.begin(); it != grLoser.end(); ++it)
        (*it)->determineRoles(0, false);
}

vector<Packing *> Algorithm_PACK::findPackings()
{

    int driSize = this->m_driSet.size();
    vector<Packing *> caPacking;
    //1. find the feasible packing for each driver
    for (int i = 0; i < driSize; i++)
    {
        Driver *d = this->m_driSet[i];
        //the candidate passengers for driver i
        vector<Passenger *> dPass = this->m_dcaMat[i];

        //packing two passengers
        for (vector<Passenger *>::iterator p1 = dPass.begin(); p1 != dPass.end(); ++p1)
        {
            for (vector<Passenger *>::iterator p2 = p1 + 1; p2 != dPass.end(); ++p2)
            {
                Packing *pack = new Packing(d);
                vector<Passenger *> passVec;
                passVec.push_back(*p1);
                passVec.push_back(*p2);
                if (pack->tryAddPassenger(passVec))
                {
                    caPacking.push_back(pack);
                }
            }
        }
    }
    return caPacking;
}

void Algorithm_PACK::clear()
{
    for (vector<Coalition_CFG_OS_2 *>::iterator it = this->m_gVec.begin(); it != this->m_gVec.end(); ++it)
        if (!(*it)->getDriver()->getIsShare())
        {

            vector<Passenger *> passVec = (*it)->getParticipants();
            for (vector<Passenger *>::iterator passIt = passVec.begin(); passIt != passVec.end(); ++passIt)
            {

                (*passIt)->setDist(0);
                (*passIt)->setTime(0);
                (*passIt)->setFare(0);
                (*passIt)->setUti(0);
                (*passIt)->setIsShare(false);
                (*passIt)->setIsSelect(false);
                (*passIt)->setCollectiveId(0);
            }

            Driver *dri = (*it)->getDriver();
            dri->setDist(0);
            dri->setTime(0);
            dri->setCost(0);
            dri->setPayoff(0);
            dri->setSharedSeatNum(0);
            dri->setSelectNum(0);
            dri->setUti(0);
            dri->setIsShare(false);
        }
}