#include "auction_cfg_os_2.h"
#include "algorithm"
#include "ComHelper.h"
Auction_CFG_OS_2::Auction_CFG_OS_2()
{
}

double Auction_CFG_OS_2::auction(vector<Driver *> driSet, vector<Coalition_CFG_OS_2 *> grSet)
{
    clear();

    vector<Driver *> driVec = driSet;
    vector<Coalition_CFG_OS_2 *> grVec = grSet;
    // Determine the winners and the losers
    return determineRoles(driVec, grVec);
}

//McAfee based auction
double Auction_CFG_OS_2::auction2(vector<Driver *> driSet, vector<Coalition_CFG_OS_2 *> grSet)
{
    clear();

    vector<Driver *> driVec = driSet;
    vector<Coalition_CFG_OS_2 *> grVec = grSet;
    // Exclude the drivers and the groups which do not place a bid
    exclude(driVec, grVec);
    // Determine the winners and the losers
    return determineRoles2(driVec, grVec);
}

void Auction_CFG_OS_2::clear()
{
    this->m_driWinner.clear();
    this->m_driLoser.clear();
    this->m_grWinner.clear();
    this->m_grLoser.clear();
    this->m_unitPayment.clear();
}

vector<Driver *> Auction_CFG_OS_2::getDriverWinners() const
{
    return this->m_driWinner;
}

vector<Driver *> Auction_CFG_OS_2::getDriverLosers() const
{
    return this->m_driLoser;
}

vector<Coalition_CFG_OS_2 *> Auction_CFG_OS_2::getGroupWinners() const
{
    return this->m_grWinner;
}

vector<Coalition_CFG_OS_2 *> Auction_CFG_OS_2::getGroupLosers() const
{
    return this->m_grLoser;
}

vector<double> Auction_CFG_OS_2::getUnitPayments() const
{
    return this->m_unitPayment;
}

void Auction_CFG_OS_2::exclude(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec)
{
    vector<Driver *> tDriVec;
    vector<Coalition_CFG_OS_2 *> tGrVec;

    vector<Coalition_CFG_OS_2 *>::iterator grIt = grVec.begin();
    for (vector<Driver *>::iterator driIt = driVec.begin(); driIt != driVec.end(); ++driIt, ++grIt)
    {
        if ((*driIt)->getSharedSeatNum() == 0)
        {
            //the driver do not have assigned passengers
            this->m_driLoser.push_back(*driIt);
            this->m_grLoser.push_back(*grIt);
        }
        else if (((*driIt)->getBid() - (*grIt)->getUnitBid() * (*grIt)->getSelectNum() * RATIO_EXCLUDE_PLATFORM_CHARGE) > 1e-6)
        {
            this->m_driLoser.push_back(*driIt);
            this->m_grLoser.push_back(*grIt);
        }
        else
        {
            tDriVec.push_back(*driIt);
            tGrVec.push_back(*grIt);
        }
    }

    driVec = tDriVec;
    grVec = tGrVec;
}

double Auction_CFG_OS_2::determineRoles(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec)
{
    double opf = 0;
    int size = grVec.size();
    //sort
    sort(grVec.begin(), grVec.end(), ComHelper());
    //determineRole
    for (int i = 0; i < size; ++i)
    {
        Driver *d = driVec[grVec[i]->getId() - 1];
        double buyoutPrice = d->getBid();
        int coalition_size = grVec[i]->getSelectNum();

        if (coalition_size == 0 || (grVec[i]->getUnitBid() * coalition_size * RATIO_EXCLUDE_PLATFORM_CHARGE < buyoutPrice))
        {
            this->m_driLoser.push_back(d);
            this->m_grLoser.push_back(grVec[i]);
            continue;
        }
        bool isExist = false;
        double unitPayment = 0;
        for (int j = i + 1; j < size; ++j)
        {
            if (grVec[j]->getUnitBid() * coalition_size * RATIO_EXCLUDE_PLATFORM_CHARGE >= buyoutPrice)
            {
                unitPayment = grVec[j]->getUnitBid();
                isExist = true;
            }
            else
                break;
        }
        
        if (isExist)
        { // Win
            this->m_driWinner.push_back(d);
            this->m_grWinner.push_back(grVec[i]);
            this->m_unitPayment.push_back(unitPayment);
            //opf: total utility of passengers = totalInitialFare - total payment
            opf += grVec[i]->getTotalInitialFare() - unitPayment * coalition_size;
        }
        else
        { // Lose
            this->m_driLoser.push_back(d);
            this->m_grLoser.push_back(grVec[i]);
        }
    }
    return opf;
}

double Auction_CFG_OS_2::determineRoles2(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec)
{
    double opf = 0;
    //descending order
    sort(grVec.begin(), grVec.end(), ComHelper());
    //increasing order
    sort(driVec.begin(), driVec.end(), ComHelper());
    int size = grVec.size();
    int k = -1;
    //find the maximum k
    for (int i = 0; i < size; ++i)
    {
        if (grVec[i]->getUnitBid() * RATIO_EXCLUDE_PLATFORM_CHARGE >= driVec[i]->getBid() / driVec[i]->getSelectNum())
        {
            k = i;
        }
        else
            break;
    }
    if (k > 0)
    {
        //find maximum alpha and beta
        int alpha = k, beta = k;
        for (int i = alpha + 1; i < size; ++i)
        {
            if (grVec[i]->getUnitBid() * RATIO_EXCLUDE_PLATFORM_CHARGE >= driVec[k]->getBid() / driVec[k]->getSelectNum())
            {
                alpha = i;
            }
        }
        for (int i = beta + 1; i < size; ++i)
        {
            if (grVec[k]->getUnitBid() * RATIO_EXCLUDE_PLATFORM_CHARGE >= driVec[i]->getBid() / driVec[i]->getSelectNum())
            {
                beta = i;
            }
        }
        if (this->countWinner(driVec, grVec, alpha, k) > this->countWinner(driVec, grVec, k, beta))
        {
            opf = this->determineWinner(driVec, grVec, alpha, k);
        }
        else
        {
            opf = this->determineWinner(driVec, grVec, k, beta);
        }
    }
    else
    {
        // no winner
        opf = determineWinner(driVec, grVec, 0, 0);
    }
    return opf;
}

double Auction_CFG_OS_2::determineWinner(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec, int b, int s)
{
    //loser coalition
    for (int i = b; i < grVec.size(); ++i)
    {
        this->m_grLoser.push_back(grVec[i]);
    }
    //loser driver
    for (int i = s; i < driVec.size(); ++i)
    {
        this->m_driLoser.push_back(driVec[i]);
    }
    if (b == 0 && s == 0)
    {
        return 0;
    }
    double opf = 0;
    double unitPayment = grVec[b]->getUnitBid();
    //determine coalition
    map<int, Driver *> driverMap;
    for (int i = 0; i < s; i++)
    {
        driverMap.insert(map<int, Driver *>::value_type(driVec[i]->getId(), driVec[i]));
    }
    for (int i = 0; i < b; ++i)
    {
        if (driverMap.find(grVec[i]->getId()) != driverMap.end())
        {
            this->m_grWinner.push_back(grVec[i]);
            this->m_unitPayment.push_back(unitPayment);
            opf += grVec[i]->getTotalInitialFare() - unitPayment * grVec[i]->getSelectNum();
        }
        else
        {
            this->m_grLoser.push_back(grVec[i]);
        }
    }
    //determine driver
    map<int, Coalition_CFG_OS_2 *> coalitionMap;
    for (int i = 0; i < b; i++)
    {
        coalitionMap.insert(map<int, Coalition_CFG_OS_2 *>::value_type(grVec[i]->getId(), grVec[i]));
    }
    for (int i = 0; i < s; ++i)
    {
        if (coalitionMap.find(driVec[i]->getId()) != coalitionMap.end())
        {
            this->m_driWinner.push_back(driVec[i]);
        }
        else
        {
            this->m_driLoser.push_back(driVec[i]);
        }
    }
    driverMap.clear();
    coalitionMap.clear();
    return opf;
}

int Auction_CFG_OS_2::countWinner(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec, int b, int s)
{
    int count = 0;
    map<int, Driver *> driverMap;
    //store the driver before s
    for (int i = 0; i < s; i++)
    {
        driverMap.insert(map<int, Driver *>::value_type(driVec[i]->getId(), driVec[i]));
    }
    //find winner
    for (int i = 0; i < b; i++)
    {
        if (driverMap.find(grVec[i]->getId()) != driverMap.end())
        {
            count++;
        }
    }
    driverMap.clear();
    return count;
}
