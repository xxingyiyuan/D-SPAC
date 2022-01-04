#ifndef COALITION_CFG_OS_H_2
#define COALITION_CFG_OS_H_2

#include <vector>
#include "settings.h"
#include "driver.h"
#include "passenger.h"
#include "route_cfg_os.h"

using namespace std;

/**
 * @brief The Group class (Actually, a group is a coalition)
 */
class Coalition_CFG_OS_2
{
public:
    Coalition_CFG_OS_2(Driver* dri);

    // Judge the feasibility on adding the passenger into group
    bool tryAddPassenger(Passenger* pass, bool isCache);
    // Judge the feasibility on adding passengers into coalition
    bool tryAddPassenger(vector<Passenger*> passVec);
    // Judge the feasibility on removing the passenger from group
    bool tryRemovePassenger(Passenger* pass,  bool isCache);
    // Update the information of group
    void update(Passenger* pass, const bool &select);
    // Restore the information of group
    void restore();
    // Calculate the unit bid of group and the bid of driver
    void calBids();
    // Determine role (winner or loser) of the group and the driver
    void determineRoles(const double &unitPayment, const bool &isWin);    

    // Getter
    int getId() const;
    int getSelectNum() const;
    double getUnitBid() const;
    Driver* getDriver() const;
    vector<Passenger*> getParticipants();
    double getTotalInitialFare();

private:
    Driver* m_dri;                      // The driver
    vector<Passenger*> m_paVec;         // The participants
    Route_CFG_OS m_ro;                  // The route
    double m_unitBid;                   // The average bid of passengers
    vector<Passenger*> m_tpaVec;        // The temporary participants
    Route_CFG_OS m_tro;                 // The temporary route
    bool m_isCache;                     // The identifier to indicate 'm_ro' is cached in 'm_tro'
};

#endif // COALITION_CFG_OS_H_2
