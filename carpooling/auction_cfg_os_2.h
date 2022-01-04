#ifndef AUCTION_CFG_OS_H_2
#define AUCTION_CFG_OS_H_2

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include "driver.h"
#include "coalition_cfg_os_2.h"

using namespace std;

class Auction_CFG_OS_2
{
public:
    Auction_CFG_OS_2();
    // The process of auction
    double auction(vector<Driver*> driSet, vector<Coalition_CFG_OS_2*> grSet);
    double auction2(vector<Driver*> driSet, vector<Coalition_CFG_OS_2*> grSet);
    // Clear messages
    void clear();
    

    // Getter
    vector<Driver*> getDriverWinners() const;
    vector<Driver*> getDriverLosers() const;
    vector<Coalition_CFG_OS_2*> getGroupWinners() const;
    vector<Coalition_CFG_OS_2*> getGroupLosers() const;
    vector<double> getUnitPayments() const;

private:
    // Exclude the drivers and the groups which do not place a bid
    void exclude(vector<Driver*> &driVec, vector<Coalition_CFG_OS_2*> &grVec);
    // Determine the winners and the losers
    double determineRoles(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec);
    double determineRoles2(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec);
    int countWinner(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec, int b, int s);
    double determineWinner(vector<Driver *> &driVec, vector<Coalition_CFG_OS_2 *> &grVec,int b, int s);

private:
    vector<Driver*> m_driWinner;                    // The set of winning drivers
    vector<Driver*> m_driLoser;                     // The set of losing drivers
    vector<Coalition_CFG_OS_2*> m_grWinner;           // The set of winning groups
    vector<Coalition_CFG_OS_2*> m_grLoser;            // The set of losing groups
    vector<double> m_unitPayment;                   // The set of unitPayment for winning drivers and winning groups
    
};

#endif // AUCTION_CFG_OS_H_2
