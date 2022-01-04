#ifndef PACKING
#define PACKING

#include <vector>
#include "driver.h"
#include "passenger.h"
#include "route_cfg_os.h"

using namespace std;

/**
 * @brief 
 */
class Packing
{
public:
    Packing(Driver *dri);
    // Judge the feasibility on adding passengers into coalition
    bool tryAddPassenger(vector<Passenger *> passVec);
    bool isFeasible();
    bool operator<( Packing *p)
    {
        return this->getUtility() < p->getUtility();
    }
    // Getter
    Driver *getDriver() const;
    vector<Passenger *> getParticipants();
    double getUtility() ;
    double getTotalInitialFare();

private:
    Driver *m_dri;               // The driver
    vector<Passenger *> m_paVec; // The participants
};

#endif // PACKING
