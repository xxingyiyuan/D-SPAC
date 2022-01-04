#ifndef DEMAND_H
#define DEMAND_H

#include "coordinate.h"

class Demand
{
public:
    Demand(const Coordinate &org, const Coordinate &dst, const double &waitTime, const double &deRatio, const int &seatNum);

    // Getter
    Coordinate getOrg() const;
    Coordinate getDst() const;
    double getWaitTime() const;
    double getDeRatio() const;
    double getSeatNum() const;

private:
    Coordinate m_org;       // The coordinate of the starting point
    Coordinate m_dst;       // The coordinate of the destination
    double m_waitTime;      // The maximum waiting time
    double m_deRatio;       // The maximum detour ratio
    int m_seatNum;          // The number of available (driver) / required (passenger) seats
};

#endif // DEMAND_H
