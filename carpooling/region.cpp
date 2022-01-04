#include "region.h"

Region::Region(const int &id, const Coordinate &centre)
    : m_id(id), m_centre(centre)
{

}

/**
 * @brief Add the other region except of 'this'
 * @param re:
 *  The other region
 * @param dist:
 *  The distance between the other region and 'this'
 */
void Region::addRestRegion(Region *re, const double &dist)
{
    this->m_rReList.push_back(re);
    this->m_rDistList.push_back(dist);
}

/**
 * @brief Add the driver located in 'this'
 * @param dri:
 *  The driver
 */
void Region::addInnerDriver(Driver *dri)
{
    this->m_iDriList.push_back(dri);
}

int Region::getId() const
{
    return this->m_id;
}

Coordinate Region::getCentre() const
{
    return this->m_centre;
}

list<Driver *> Region::getInnerDriverList() const
{
    return this->m_iDriList;
}

list<Region *> Region::getRestRegionList() const
{
    return this->m_rReList;
}

list<double> Region::getRestRegionDistList() const
{
    return this->m_rDistList;
}



