#ifndef ALGORITHM_CFG_OS_H_2
#define ALGORITHM_CFG_OS_H_2

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include "settings.h"
#include "driver.h"
#include "passenger.h"
#include "coalition_cfg_os_2.h"
#include "auction_cfg_os_2.h"
#include "ComHelper.h"
using namespace std;
using namespace chrono;

/**
 * @brief
 */
class Algorithm_CFG_OS_2
{
public:
    Algorithm_CFG_OS_2(const vector<Driver*> &driSet, const vector<vector<Passenger*> > &dcaMat, const vector<Passenger*> &passSet, const vector<vector<Driver*> > &pcaMat);
    ~Algorithm_CFG_OS_2();
    Driver* bm(Passenger *p);
    Passenger* bm(Driver* d);
    // Getter
    vector<double> getOverallPayoffVector() const;
    double getRunningTime() const;
    
private:
    // Initialize the coalition partition
    void init_new();
    // The process of group formation
    void process(Passenger* pass, double &opf);
    // The branch in which the passenger has been assigned in a group
    void inGroupBranch(Passenger* pass, vector<Driver *> pcaVec, double &opf);
    // The branch in which the passenger has not been assigned in a group
    void notInGroupBranch(Passenger* pass, vector<Driver *> pcaVec, double &opf);
    // The joining operation
    bool joinOperation(Passenger* pass, Coalition_CFG_OS_2* gr);
    // The leaving operation
    bool leaveOperation(Passenger* pass, Coalition_CFG_OS_2* gr);
    // Update messages
    void update();
    // Clear messages of the losers
    void clear();
    

private:
    vector<Driver*> m_driSet;                       // The set of drivers
    vector<vector<Passenger*> > m_dcaMat;           // The candidate passengers for each driver
    vector<Coalition_CFG_OS_2*> m_gVec;               // The container storing groups
    vector<Passenger*> m_passSet;                   // The set of passengers
    vector<vector<Driver*> > m_pcaMat;              // The candidate drivers for each passenger
    Auction_CFG_OS_2 m_au;                            // The auctioneer
    vector<double> m_opfVector;                     // The container storing the overall payoff of all winning drivers in each round
    double m_time;
};

#endif // ALGORITHM_CFG_OS_H_2
