#ifndef COORDINATE_H
#define COORDINATE_H

#include <cmath>

class Coordinate
{
public:
    Coordinate(const double &longitude, const double &latitude);

    // Calculate the distance between two Coordinate
    double calDist(const Coordinate &oth) const;

    // Getter
    double getLongitude() const;
    double getLatitude() const;

private:
    double m_long;      // The longitude
    double m_lat;       // The latitude
};

#endif // COORDINATE_H
