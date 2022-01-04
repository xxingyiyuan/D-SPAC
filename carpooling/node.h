#ifndef NODE_H
#define NODE_H

#include "coordinate.h"

class Node
{
public:
    Node(const int &who, const int &where, const Coordinate &co, const double dist);

    // Getter
    int getWho() const;
    int getWhere() const;
    Coordinate getCoordinate() const;
    double getPrCuDistance() const;

    // Setter
    void setPrCuDistance(const double &dist);

private:
    int m_who;                  // The identifier indicating the owner (0 for driver, other # for passenger)
    int m_where;                // The identifier indicating the place (0 for origin, 1 for destination)
    Coordinate m_co;            // The coordinate of current node
    double m_pcDist;            // The distance between previous node and current node
};

#endif // NODE_H
