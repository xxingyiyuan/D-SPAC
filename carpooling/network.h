#ifndef NETWORK_H
#define NETWORK_H

#include <list>
#include <vector>
#include <cmath>
#include "settings.h"
#include "coordinate.h"
#include "region.h"
#include "driver.h"
#include "passenger.h"
#include "settings.h"

using namespace std;

class Network
{
public:
    Network(const Coordinate &lbCorner, const int &rowNum, const int &colNum, const double &reWidth, const double &reHeight);
    ~Network();

    // Add the driver into the corresponding region
    void addDriver2Region(Driver* dri);
    // Select candidates
    void selectCandidate(Passenger* pass, vector<Driver*> &caVec);

    // Getter
    Coordinate getLeftBottomCorner() const;
    int getRowNum() const;
    int getColNum() const;
    double getRegionWidth() const;
    double getRegionHeight() const;
    vector<vector<Region*> > getRegionMatrix() const;

private:
    // Initial the region matrix in network
    void initRegionMatrix();
    // Build relationship among all regions
    void buildRelationship(const vector<vector<double> > &distMat);
    // Judge whether constraints between 'pass' and 'dri' are satisfied
    bool judgeConstraint(Passenger* pass, Driver* dri);

private:
    Coordinate m_lbCorner;                      // The coordinate of the left bottom corner
    int m_rowNum;                               // # of rows (# of region in a column)
    int m_colNum;                               // # of columns (# of region in a row)
    double m_reWidth;                           // The width of a region
    double m_reHeight;                          // The height of a region
    vector<vector<Region*> > m_reMat;           // The matrix of regions in network
};

#endif // NETWORK_H
