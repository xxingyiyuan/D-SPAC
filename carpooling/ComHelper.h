#ifndef CompHelper_H
#define CompHelper_H
#include "Packing.h"
#include "coalition_cfg_os_2.h"
#include "driver.h"
#include "passenger.h"
class ComHelper
{
private:
    /* data */
public:
    bool operator()( Packing *p1, Packing *p2)
    {
        return p1->getUtility() < p2->getUtility();
    }
    bool operator()(Coalition_CFG_OS_2 *c1, Coalition_CFG_OS_2 *c2)
    {
        return c1->getUnitBid() > c2->getUnitBid();
    }
    bool operator()(Driver *d1, Driver *d2)
    {
        return d1->getBid() / d1->getSelectNum() < d2->getBid() / d2->getSelectNum();
    }
     bool operator()(Passenger *p1, Passenger *p2)
    {
        return p1->getBid() > p2->getBid();
    }
};

#endif