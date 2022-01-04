#include "algorithm_cfg_os_2.h"

Algorithm_CFG_OS_2::Algorithm_CFG_OS_2(const vector<Driver *> &driSet, const vector<vector<Passenger *>> &dcaMat, const vector<Passenger *> &passSet, const vector<vector<Driver *>> &pcaMat)
    : m_driSet(driSet), m_dcaMat(dcaMat), m_passSet(passSet), m_pcaMat(pcaMat), m_au(), m_time(0)
{
    //coalition formulation generation this->m_gVec
    init_new();
    double opf = this->m_au.auction(this->m_driSet, this->m_gVec);
    this->update();

    // Start timer
    auto start = system_clock::now();
    double tmp;
    do
    // for (int i = 0; i < ROUND; i++)
    {
        tmp = opf;
        for (vector<Passenger *>::iterator passIt = this->m_passSet.begin(); passIt != this->m_passSet.end(); ++passIt)
        {
            process(*passIt, opf);
            this->m_opfVector.push_back(opf);
        }
    }
    while (opf - tmp > 1);
    // for (vector<double>::iterator it = this->m_opfVector.begin(); it != this->m_opfVector.end(); it++){
    //     cout << (*it) << ",";
    // }
    this->clear();

    // Stop timer
    auto end = system_clock::now();
    // Calcluate the execution time (second)
    auto duration = duration_cast<microseconds>(end - start);
    this->m_time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
}

Algorithm_CFG_OS_2::~Algorithm_CFG_OS_2()
{
    for (vector<Coalition_CFG_OS_2 *>::iterator it = this->m_gVec.begin(); it != this->m_gVec.end(); ++it)
        delete (*it);
}

vector<double> Algorithm_CFG_OS_2::getOverallPayoffVector() const
{
    return this->m_opfVector;
}

double Algorithm_CFG_OS_2::getRunningTime() const
{
    return this->m_time;
}
void Algorithm_CFG_OS_2::init_new()
{
    //initialize a coalition for each driver
    int size = this->m_driSet.size();
    for (int i = 0; i < size; ++i)
    {
        Coalition_CFG_OS_2 *gr = new Coalition_CFG_OS_2(this->m_driSet[i]);
        this->m_gVec.push_back(gr);
    }
    //find a suitable driver for each passenger
    for (vector<Passenger *>::iterator it = this->m_passSet.begin(); it != this->m_passSet.end(); it++)
    {
        Driver *bestd = bm(*it);
        if (!bestd)
            continue;
        Coalition_CFG_OS_2 *gr = this->m_gVec[bestd->getId() - 1];
        gr->tryAddPassenger(*it, true);
        gr->update(*it, true);
    }
    
}

Driver *Algorithm_CFG_OS_2::bm(Passenger *p)
{
    vector<Driver *> driVec = this->m_pcaMat[p->getId() - 1];
    double dist = 1e6;
    Driver *bestDriver = nullptr;
    for (vector<Driver *>::iterator it = driVec.begin(); it != driVec.end(); it++)
    {
        Coalition_CFG_OS_2 *gr = this->m_gVec[(*it)->getId() - 1];
        if (gr->tryAddPassenger(p, true))
        {
            double originDist = (*it)->getOrg().calDist(p->getOrg());
            if (originDist < dist)
            {
                dist = originDist;
                bestDriver = *it;
            }
            gr->restore();
        }
    }
    return bestDriver;
}
Passenger *Algorithm_CFG_OS_2::bm(Driver *d)
{
    vector<Passenger *> passVec = this->m_dcaMat[d->getId() - 1];
    Coalition_CFG_OS_2 *gr = this->m_gVec[d->getId() - 1];
    double dist = 1e6;
    Passenger *bestPassenger = nullptr;
    for (vector<Passenger *>::iterator it = passVec.begin(); it != passVec.end(); it++)
    {
        if ((*it)->getIsSelect())
            continue;
        double old_dist = max(d->getDist(), d->getInitDist());
        if (gr->tryAddPassenger(*it, true))
        {
            double originDist = d->getOrg().calDist((*it)->getOrg());
            //double incrementDist = d->getDist() - old_dist;
            //double detourDist = d->getDist() / d->getInitDist() - 1;
            if (originDist < dist)
            {
                dist = originDist;
                bestPassenger = *it;
            }
            gr->restore();
        }
    }
    return bestPassenger;
}

void Algorithm_CFG_OS_2::process(Passenger *pass, double &opf)
{
    vector<Driver *> pcaVec = this->m_pcaMat.at(pass->getId() - 1);
    if (pcaVec.size() == 0)
        return; // No candidate drivers for 'pass'

    if (pass->getIsSelect())
        inGroupBranch(pass, pcaVec, opf); // 'pass' is in a group: switching operation
    else
        notInGroupBranch(pass, pcaVec, opf); // 'pass' is not in a group: joinning operation or replacing operation
}

void Algorithm_CFG_OS_2::inGroupBranch(Passenger *pass, vector<Driver *> pcaVec, double &opf)
{
    //src -- the group containing pass
    Coalition_CFG_OS_2 *src = this->m_gVec.at(pass->getCollectiveId() - 1);
    // double u1 = pass->getUti();
    // --remove the passenger from src
    bool isLeave = false;
    if (leaveOperation(pass, src))
    {
        double tmp = this->m_au.auction(this->m_driSet, this->m_gVec);
        if (tmp > opf)
        {
            opf = tmp;
            isLeave = true;
        }
    }
    else
    {
        src->restore();
        return;
    }
    Coalition_CFG_OS_2 *select = nullptr;
    for (vector<Driver *>::iterator it = pcaVec.begin(); it != pcaVec.end(); ++it)
    {
        // the source group
        if (pass->getCollectiveId() == (*it)->getId())
            continue;

        Coalition_CFG_OS_2 *gr = this->m_gVec.at((*it)->getId() - 1);
        if (joinOperation(pass, gr))
        {
            double tmp = this->m_au.auction(this->m_driSet, this->m_gVec);
            if (tmp > opf)
            {
                opf = tmp;
                select = gr;
            }
        }
        gr->restore();
    }
    //select -- the group which pass wants to join
    if (select != nullptr)
    { // Switching operation (Leaving operation + joining operation)
        joinOperation(pass, select);
        this->m_au.auction(this->m_driSet, this->m_gVec);
        src->update(pass, false);
        select->update(pass, true);
        this->update();
    }
    else if (isLeave)
    {
        // src->restore();
        // leaveOperation(pass, src);
        this->m_au.auction(this->m_driSet, this->m_gVec);
        src->update(pass, false);
        this->update();
    }
    else
    {
        // No operation
        src->restore();
        this->m_au.auction(this->m_driSet, this->m_gVec);
    }
}

void Algorithm_CFG_OS_2::notInGroupBranch(Passenger *pass, vector<Driver *> pcaVec, double &opf)
{
    Coalition_CFG_OS_2 *select = nullptr;
    for (vector<Driver *>::iterator it = pcaVec.begin(); it != pcaVec.end(); ++it)
    {
        Coalition_CFG_OS_2 *gr = this->m_gVec.at((*it)->getId() - 1);
        if (joinOperation(pass, gr))
        {
            //adding the pass into gr to compute opf (the payoffs of drivers)
            double tmp = this->m_au.auction(this->m_driSet, this->m_gVec);
            if (tmp > opf)
            {
                opf = tmp;
                select = gr;
            }
        }
        gr->restore();
    }
    // select -- the group which pass wants to join
    if (select != nullptr)
    { // Joining operation
        joinOperation(pass, select);
        this->m_au.auction(this->m_driSet, this->m_gVec);
        select->update(pass, true);
        this->update();
    }
    else
    {
        this->m_au.auction(this->m_driSet, this->m_gVec);
    }
}

bool Algorithm_CFG_OS_2::joinOperation(Passenger *pass, Coalition_CFG_OS_2 *gr)
{
    return gr->tryAddPassenger(pass, true);
}

bool Algorithm_CFG_OS_2::leaveOperation(Passenger *pass, Coalition_CFG_OS_2 *gr)
{
    return gr->tryRemovePassenger(pass, true);
}

void Algorithm_CFG_OS_2::update()
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

void Algorithm_CFG_OS_2::clear()
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

