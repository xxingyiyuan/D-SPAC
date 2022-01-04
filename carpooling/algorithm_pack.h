#ifndef ALGORITHM_PACK_H
#define ALGORITHM_PACK_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include "settings.h"
#include "driver.h"
#include "passenger.h"
#include "coalition_cfg_os_2.h"
#include "auction_cfg_os_2.h"
#include "packing.h"
#include <chrono>
#include <iomanip>
#include "ComHelper.h"

using namespace std;

class Algorithm_PACK
{
public:
    Algorithm_PACK(const vector<Driver *> &driSet, const vector<vector<Passenger *>> &dcaMat, const vector<Passenger *> &passSet, const vector<vector<Driver *>> &pcaMat);
    ~Algorithm_PACK();

private:
    void init();
    void notInGroupBranch(Passenger *pass, vector<Driver *> pcaVec, double &opf);
    bool joinOperation(Passenger* pass, Coalition_CFG_OS_2* gr);
    vector<Packing*> findPackings();
    void update();
    void clear();

private:
    vector<Driver *> m_driSet;            // The set of drivers
    vector<vector<Passenger *>> m_dcaMat; // The candidate passengers for each driver
    vector<Coalition_CFG_OS_2 *> m_gVec;  // The container storing groups
    vector<Passenger *> m_passSet;        // The set of passengers
    vector<vector<Driver *>> m_pcaMat;    // The candidate drivers for each passenger
    Auction_CFG_OS_2 m_au;                // The auctioneer
    double m_time;
};

#endif // ALGORITHM_PACK_H