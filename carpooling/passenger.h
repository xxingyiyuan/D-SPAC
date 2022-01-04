#ifndef PASSENGER_H
#define PASSENGER_H

#include <vector>
#include "settings.h"
#include "coordinate.h"
#include "demand.h"

using namespace std;

class Passenger
{
public:
    Passenger(const int &id, const Demand &de, const double &ratio);

    // Getter
    int getId() const;
    Coordinate getOrg() const;
    Coordinate getDst() const;
    double getWaitTime() const;
    double getDeRatio() const;
    int getSeatNum() const;
    double getInitDist() const;
    double getInitTime() const;
    double getInitFare() const;
    double getDist() const;
    double getTime() const;
    double getFare() const;
    double getBid() const;
    double getUti() const;
    bool getIsShare() const;
    bool getIsSelect() const;
    int getCollectiveId() const;

    // Setter
    void setDist(const double &dist);
    void setTime(const double &time);
    void setFare(const double &fare);
    void setUti(const double &uti);
    void setIsShare(const bool &isShare);
    void setIsSelect(const bool &isSelect);
    void setCollectiveId(const int &id);

private:
    int m_id;                       // The identification number
    Demand m_de;                    // The travel demand
    double m_iDist;                 // The initial travel distance
    double m_iTime;                 // The initial travel time
    double m_iFare;                 // The initial travel fare (budget)
    double m_sDist;                 // The travel distance in sharing trip
    double m_sTime;                 // The travel time in sharing trip
    double m_sFare;                 // The travel fare in sharing trip (actual payment)
    double m_bid;                   // The bid for sharing trip
    double m_uti;                   // The utility
    bool m_isShare;                 // The identifier to indicate whether join in sharing trip (true) or not (false)
    bool m_isSelect;                // The identifier to indicate whether be selected in a coalition
    int m_coId;                     // The identification number of the collective (coalition or group) which the passenger belongs to
};

#endif // PASSENGER_H
