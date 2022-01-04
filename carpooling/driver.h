#ifndef DRIVER_H
#define DRIVER_H

#include <vector>
#include "settings.h"
#include "coordinate.h"
#include "demand.h"

using namespace std;

class Driver
{
public:
    Driver(const int &id, const Demand &de, const double &ratio);

    // Getter
    int getId() const;
    Coordinate getOrg() const;
    Coordinate getDst() const;
    double getWaitTime() const;
    double getDeRatio() const;
    int getSeatNum() const;
    double getInitDist() const;
    double getInitTime() const;
    double getDist() const;
    double getTime() const;
    double getCost() const;
    double getDetourCost() const;
    double getPayoff() const;
    int getSharedSeatNum() const;
    int getSelectNum() const;
    double getBid() const;
    double getUti() const;
    bool getIsShare() const;

    // Setter
    void setDist(const double &dist);
    void setTime(const double &time);
    void setCost(const double &cost);
    void setPayoff(const double &payoff);
    void setSharedSeatNum(const int &seat);
    void setSelectNum(const int &selectNum);
    void setBid(const double &bid);
    void setUti(const double &uti);
    void setIsShare(const bool &isShare);

private:
    int m_id;                           // The identification number
    Demand m_de;                        // The travel demand
    double m_iDist;                     // The initial travel distance
    double m_iTime;                     // The initial travel time
    double m_sDist;                     // The travel distance in sharing trip
    double m_sTime;                     // The travel time in sharing trip
    double m_sCost;                     // The travel cost suffered in sharing trip
    double m_sPayoff;                   // The payoff received in sharing trip
    int m_sSeat;                        // # of shared seats in sharing trip
    double m_bid;                       // The bid for sharing trip
    double m_uti;                       // The utility
    bool m_isShare;                     // The identifier to indicate whether join in sharing trip (true) or not (false)
    double m_ratio;
    int m_selectNum;
};

#endif // DRIVER_H
