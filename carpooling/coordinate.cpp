#include "coordinate.h"

Coordinate::Coordinate(const double &longitude, const double &latitude)
    : m_long(longitude), m_lat(latitude)
{
}

/**
 * @brief Calculate the distance between two Coordinate
 * @param oth:
 *  The other Coordinate
 * @return The distance
 */
double Coordinate::calDist(const Coordinate &oth) const
{
    return sqrt(
               pow((this->getLongitude() - oth.getLongitude()) / 0.00001, 2) +
               pow((this->getLatitude() - oth.getLatitude()) / 0.00001, 2)) /
           1000;
}

double Coordinate::getLongitude() const
{
    return this->m_long;
}

double Coordinate::getLatitude() const
{
    return this->m_lat;
}
