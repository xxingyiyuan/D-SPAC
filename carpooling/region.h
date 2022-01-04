#ifndef REGION_H
#define REGION_H

#include <list>
#include "coordinate.h"
#include "driver.h"

using namespace std;

class Region
{
public:
    Region(const int &id, const Coordinate &centre);

    // Add the other region except of 'this'
    void addRestRegion(Region* re, const double &dist);
    // Add the driver located in 'this'
    void addInnerDriver(Driver* dri);

    // Getter
    int getId() const;
    Coordinate getCentre() const;
    list<Driver*> getInnerDriverList() const;
    list<Region*> getRestRegionList() const;
    list<double> getRestRegionDistList() const;

private:
    int m_id;                       // The identification number
    Coordinate m_centre;            // The coordinate of the centre
    list<Driver*> m_iDriList;       // The container storing all drivers located in 'this'
    list<Region*> m_rReList;        // The container storing all other regions in an ascending order based on the distances
    list<double> m_rDistList;       // The container storing the distances between this region and other regions in 'm_rReList'
};

#endif // REGION_H
