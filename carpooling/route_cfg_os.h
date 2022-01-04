#ifndef ROUTE_CFG_OS_H
#define ROUTE_CFG_OS_H

#include <vector>
#include <map>
#include "settings.h"
#include "node.h"
#include "driver.h"
#include "passenger.h"

using namespace std;

class Route_CFG_OS
{
public:
    Route_CFG_OS(Driver* dri);

    // Judge the feasibility on adding the passenger into route
    bool addPassenger(Passenger* pass);
    // Update the information of all involved users and the route
    void updateAllInfos();

    // Getter
    double getShareTravelCost() const;

private:
    // Find the feasible insertion positions for source
    void findSourceInsertion(Passenger *pass, vector<vector<Node>::iterator> &srcIdxVec);
    // Find the feasible insertion positions for destination
    void findDestinationInsertion(Passenger* pass, vector<vector<Node>::iterator> srcIdxVec, vector<vector<vector<Node>::iterator> > &dstIdxMat);
    // Judge the feasibility on inserting the source into route
    bool insertSource(Passenger* pass, vector<Node>::iterator srcIdx);
    // Judge the feasibility on inserting the destination into route
    bool insertDestination(Passenger* pass, vector<Node>::iterator srcIdx, vector<Node>::iterator dstIdx);
    // Calculate the distance of share trip for driver and passengers
    map<int, double> calShareDistances(Passenger* pass, vector<Node>::iterator srcIdx, vector<Node>::iterator dstIdx);
    // Update the information in route
    void update(Passenger* pass, vector<Node>::iterator srcIdx, vector<Node>::iterator dstIdx);

private:
    Driver* m_dri;                          // The driver
    vector<Node> m_se;                      // The node sequence in route
    int m_key;                              // The key value
    map<int, Passenger*> m_idxMap;          // The container indexing the assigned passengers
    map<int, double> m_sDistMap;            // The container storing the distance of share trip for driver (key = 0) and passengers (key > 0)
};

#endif // ROUTE_CFG_OS_H
