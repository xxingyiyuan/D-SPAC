#ifndef ALGORITHM_MWBM_H
#define ALGORITHM_MWBM_H

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <set>
#include "settings.h"
#include "driver.h"
#include "passenger.h"
#include "coalition_cfg_os_2.h"
#include <chrono>
#include <iomanip>
#include "ComHelper.h"
#include <algorithm>

using namespace std;

class Algorithm_MWBM
{
public:
    Algorithm_MWBM(const vector<Driver *> &driSet, const vector<vector<Passenger *>> &dcaMat, const vector<Passenger *> &passSet, const vector<vector<Driver *>> &pcaMat);
    ~Algorithm_MWBM();

private:
    void init();
    Driver* bm(Passenger * p);
    Passenger* bm(Driver* d);
    void oneToManyMatching();
    void pricing();
    void determineRoles(Coalition_CFG_OS_2 *gr, bool isWin);
    

private:
    vector<Driver *> m_driSet;            // The set of drivers
    vector<vector<Passenger *>> m_dcaMat; // The candidate passengers for each driver
    vector<Coalition_CFG_OS_2 *> m_gVec;  // The container storing groups
    vector<Passenger *> m_passSet;        // The set of passengers
    vector<vector<Driver *>> m_pcaMat;    // The candidate drivers for each passenger

};

#endif // ALGORITHM_MWBM_H