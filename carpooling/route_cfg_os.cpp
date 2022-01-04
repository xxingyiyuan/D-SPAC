#include "route_cfg_os.h"
#include <iostream>
Route_CFG_OS::Route_CFG_OS(Driver *dri)
    : m_dri(dri), m_key(1)
{
    Node src(0, 0, this->m_dri->getOrg(), 0);
    Node dst(0, 1, this->m_dri->getDst(), this->m_dri->getInitDist());
    this->m_se.push_back(src);
    this->m_se.push_back(dst);
}

bool Route_CFG_OS::addPassenger(Passenger *pass)
{
    // Find the feasible insertion positions for source
    vector<vector<Node>::iterator> srcIdxVec;
    findSourceInsertion(pass, srcIdxVec);

    if (!srcIdxVec.size())
        return false;

    // Find the feasible insertion positions for destination
    vector<vector<vector<Node>::iterator>> dstIdxMat;
    findDestinationInsertion(pass, srcIdxVec, dstIdxMat);

    bool isQuit = true;
    for (vector<vector<vector<Node>::iterator>>::iterator it = dstIdxMat.begin(); it != dstIdxMat.end(); ++it)

        if ((*it).size())
        {
            isQuit = false;
            break;
        }
    // if there is not exist feasible insertion, isQuit = true
    if (isQuit)
        return false;

    // Get the best insertion position to obtain minimum distance
    double dist = 1e10;
    vector<Node>::iterator src = this->m_se.end(), dst = this->m_se.end();
    vector<vector<Node>::iterator>::iterator srcIdx = srcIdxVec.begin();

    for (vector<vector<vector<Node>::iterator>>::iterator it = dstIdxMat.begin(); it != dstIdxMat.end(); ++it, ++srcIdx)
    {
        //(*it) -- dstIdxVec corresponding to srcIdx
        for (vector<vector<Node>::iterator>::iterator dstIdx = (*it).begin(); dstIdx != (*it).end(); ++dstIdx)
        {
            //(*dstIdx) -- vector<Node>::iterator

            map<int, double> sDistMap = calShareDistances(pass, *srcIdx, *dstIdx);

            //sDistMap[0] is the value of key = 0, which means the share trip distance of driver
            if (sDistMap[0] < dist)
            {
                dist = sDistMap[0];
                src = *srcIdx;
                dst = *dstIdx;
            }
        }
    }
    update(pass, src, dst);
    return true;
}

void Route_CFG_OS::updateAllInfos()
{

    if (this->m_idxMap.size())
    {
        int seatNum = 0;
        // Update the information of the passenger
        for (map<int, Passenger *>::iterator it = this->m_idxMap.begin(); it != this->m_idxMap.end(); ++it)
        {

            it->second->setDist(this->m_sDistMap[it->first]);
            it->second->setTime(it->second->getDist() / SPEED);
            seatNum += it->second->getSeatNum();
        }

        // Update the information of the driver
        this->m_dri->setDist(this->m_sDistMap[0]);
        this->m_dri->setTime(this->m_dri->getDist() / SPEED);
        this->m_dri->setSharedSeatNum(seatNum);
        this->m_dri->setSelectNum(this->m_idxMap.size());
    }
    else
    {

        this->m_dri->setDist(0);
        this->m_dri->setTime(0);
        this->m_dri->setCost(0);
        this->m_dri->setPayoff(0);
        this->m_dri->setSharedSeatNum(0);
        this->m_dri->setSelectNum(0);
        this->m_dri->setUti(0);
        this->m_dri->setIsShare(false);
    }
}

double Route_CFG_OS::getShareTravelCost() const
{
    return this->m_sDistMap.begin()->second;
}

void Route_CFG_OS::findSourceInsertion(Passenger *pass, vector<vector<Node>::iterator> &srcIdxVec)
{
    for (vector<Node>::iterator srcIdx = this->m_se.begin() + 1; srcIdx != this->m_se.end(); ++srcIdx)
        if (insertSource(pass, srcIdx))
            srcIdxVec.push_back(srcIdx);
}

void Route_CFG_OS::findDestinationInsertion(Passenger *pass, vector<vector<Node>::iterator> srcIdxVec, vector<vector<vector<Node>::iterator>> &dstIdxMat)
{
    for (vector<vector<Node>::iterator>::iterator srcIdx = srcIdxVec.begin(); srcIdx != srcIdxVec.end(); ++srcIdx)
    {
        vector<vector<Node>::iterator> dstIdxVec;
        //dstIdxVec corresponding to srcIdx
        for (vector<Node>::iterator dstIdx = *srcIdx; dstIdx != this->m_se.end(); ++dstIdx)

            if (insertDestination(pass, *srcIdx, dstIdx))
                dstIdxVec.push_back(dstIdx);

        dstIdxMat.push_back(dstIdxVec);
    }
}

bool Route_CFG_OS::insertSource(Passenger *pass, vector<Node>::iterator srcIdx)
{
    double dist = 0;
    map<int, double> distMap;
    for (vector<Node>::iterator it = this->m_se.begin() + 1; it != this->m_se.end(); ++it)
    {

        if (it != srcIdx)
            dist += (*it).getPrCuDistance();
        else
        {
            dist += (*(it - 1)).getCoordinate().calDist(pass->getOrg());
            //waitTime constrant for pass
            if ((dist / SPEED - pass->getWaitTime()) > 1e-6)
                return false;
            dist += pass->getOrg().calDist((*it).getCoordinate());
        }
        // an orgin node
        if ((*it).getWhere() == 0)
        {
            // Waiting time constraint
            if ((dist / SPEED - this->m_idxMap.find((*it).getWho())->second->getWaitTime()) > 1e-6)
                return false;
            //distMap --store the distance from the origin of driver to the origin of passenger (*it)
            distMap.insert(map<int, double>::value_type((*it).getWho(), dist));
        }
        else
        {
        // a destination node
            if (it == this->m_se.end() - 1)
            {
                // dirver Detour constraint
                if ((dist - this->m_dri->getInitDist() * (1 + this->m_dri->getDeRatio())) > 1e-6)
                    return false;
                else
                    break;
            }

            // Detour constraint
            Passenger *pa = this->m_idxMap.find((*it).getWho())->second;
            //dist -- the distance from the origin of driver to the destination of passenger (*it)
            //(dist - distMap[(*it).getWho()]) -- the share trip distance of passenger pa
            if ((dist - distMap[(*it).getWho()] - pa->getInitDist() * (1 + pa->getDeRatio())) > 1e-6)
                return false;
        }
    }

    return true;
}

bool Route_CFG_OS::insertDestination(Passenger *pass, vector<Node>::iterator srcIdx, vector<Node>::iterator dstIdx)
{
    double dist = 0, passDist = 0;
    bool isInsert = false;
    map<int, double> distMap;
    for (vector<Node>::iterator it = this->m_se.begin() + 1; it != this->m_se.end(); ++it)
    {
        //dist -- the distance from the origin of driver to current node (*it)
        if (it == srcIdx && it == dstIdx)
        {
            // insert source and destination bewteen (it-1) and it
            dist += (*(it - 1)).getCoordinate().calDist(pass->getOrg()) + pass->getInitDist() + pass->getDst().calDist((*it).getCoordinate());
            isInsert = true;
        }
        else if (it == srcIdx)
        {
            dist += (*(it - 1)).getCoordinate().calDist(pass->getOrg()) + pass->getOrg().calDist((*it).getCoordinate());
            isInsert = true;
        }
        else if (it == dstIdx)
            dist += (*(it - 1)).getCoordinate().calDist(pass->getDst()) + pass->getDst().calDist((*it).getCoordinate());
        else
            dist += (*it).getPrCuDistance();

        if (isInsert)
        {

            if (it == srcIdx && it == dstIdx)
                isInsert = false;
            else if (it == srcIdx)
                passDist += pass->getOrg().calDist((*it).getCoordinate());
            else if (it == dstIdx)
            {
                passDist += (*(it - 1)).getCoordinate().calDist(pass->getDst());
                // Detour constraint
                if (passDist - pass->getInitDist() * (1 + pass->getDeRatio()) > 1e-6)
                    return false;
                isInsert = false;
            }
            else
                passDist += (*it).getPrCuDistance();
        }

        if ((*it).getWhere() == 0)
        {
            // Waiting time constraint
            if ((dist / SPEED - this->m_idxMap.find((*it).getWho())->second->getWaitTime()) > 1e-6)
                return false;
            distMap.insert(map<int, double>::value_type((*it).getWho(), dist));
        }
        else //(*it).getWhere() == 1
        {

            if (it == this->m_se.end() - 1)
            {
                // Detour constraint
                if ((dist - this->m_dri->getInitDist() * (1 + this->m_dri->getDeRatio())) > 1e-6)
                    return false;
                else
                    break;
            }

            // Detour constraint
            Passenger *pa = this->m_idxMap.find((*it).getWho())->second;
            if ((dist - distMap[(*it).getWho()] - pa->getInitDist() * (1 + pa->getDeRatio())) > 1e-6)
                return false;
        }
    }

    return true;
}

map<int, double> Route_CFG_OS::calShareDistances(Passenger *pass, vector<Node>::iterator srcIdx, vector<Node>::iterator dstIdx)
{
    map<int, bool> isAddMap;
    // 0 for driver
    isAddMap.insert(map<int, bool>::value_type(0, true));
    map<int, double> sDistMap;
    //sDistMap -- driver (key = 0) and passengers (key > 0), value: the distance of share trip
    sDistMap.insert(map<int, double>::value_type(0, 0.0));

    double dist = 0.0;
    bool isUpdate = false;
    for (vector<Node>::iterator it = this->m_se.begin() + 1; it != this->m_se.end(); ++it)
    {

        double tmp = 0.0;

        if (it == srcIdx && it == dstIdx)
        {
            tmp = (*(it - 1)).getCoordinate().calDist(pass->getOrg()) + pass->getInitDist() + pass->getDst().calDist((*it).getCoordinate());
            isUpdate = true;
        }
        else if (it == srcIdx)
        {
            tmp = (*(it - 1)).getCoordinate().calDist(pass->getOrg()) + pass->getOrg().calDist((*it).getCoordinate());
            isUpdate = true;
        }
        else if (it == dstIdx)
            tmp = (*(it - 1)).getCoordinate().calDist(pass->getDst()) + pass->getDst().calDist((*it).getCoordinate());
        else
            tmp = (*it).getPrCuDistance();

        //tmp -- the distance from node (it-1) to node it
        //dist -- the distance from the origin to the destination of passenger
        // isUpdate = true -- between it == srcIdx and it == dstIdx
        if (isUpdate)
        {
            if (it == srcIdx && it == dstIdx)
            {
                dist = pass->getInitDist();
                isUpdate = false;
            }
            else if (it == srcIdx)
                dist += pass->getOrg().calDist((*it).getCoordinate());
            else if (it == dstIdx)
            {
                dist += (*(it - 1)).getCoordinate().calDist(pass->getDst());
                isUpdate = false;
            }
            else
                dist += tmp;
        }
        // compute the share trip distance for participants
        for (map<int, double>::iterator sd = sDistMap.begin(); sd != sDistMap.end(); ++sd)
            if (isAddMap[sd->first])
                sd->second += tmp;

        if ((*it).getWhere() == 0)
        {
            //the participant (*it).getWho() is added to isAddMap when (*it).getWhere() == 0
            isAddMap.insert(map<int, bool>::value_type((*it).getWho(), true));
            sDistMap.insert(map<int, double>::value_type((*it).getWho(), 0.0));
        }
        else
            //(*it).getWhere() == 1, means the share trip of the participant is finished
            isAddMap[(*it).getWho()] = false;
    }
    //this->m_key represents the added passenger's key of the share trip
    sDistMap.insert(map<int, double>::value_type(this->m_key, dist));
    return sDistMap;
}

void Route_CFG_OS::update(Passenger *pass, vector<Node>::iterator srcIdx, vector<Node>::iterator dstIdx)
{
    this->m_sDistMap = calShareDistances(pass, srcIdx, dstIdx);

    int i = 1, src = 0, dst = 0;
    for (vector<Node>::iterator it = this->m_se.begin() + i; it != this->m_se.end(); ++it, ++i)
    {

        if (it == srcIdx)
            src = i;
        if (it == dstIdx)
            dst = i;
    }
    //update the PrCuDistance of this->m_se[src] and this->m_se[dst]
    (this->m_se[src]).setPrCuDistance(pass->getOrg().calDist((this->m_se[src]).getCoordinate()));
    (this->m_se[dst]).setPrCuDistance(pass->getDst().calDist((this->m_se[dst]).getCoordinate()));
    // insert destination node before this->m_se.begin() + dst
    if (src == dst)
        this->m_se.insert(this->m_se.begin() + dst, Node(this->m_key, 1, pass->getDst(), pass->getInitDist()));
    else
        this->m_se.insert(this->m_se.begin() + dst, Node(this->m_key, 1, pass->getDst(), (this->m_se[dst - 1]).getCoordinate().calDist(pass->getDst())));
    //insert source node before this->m_se.begin() + src
    this->m_se.insert(this->m_se.begin() + src, Node(this->m_key, 0, pass->getOrg(), (this->m_se[src - 1]).getCoordinate().calDist(pass->getOrg())));
    // add the new passenger
    this->m_idxMap.insert(map<int, Passenger *>::value_type(this->m_key, pass));

    ++this->m_key;
}



