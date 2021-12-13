from route import Route
from driver import Driver
from passenger import Passenger
from tool import Tool
import settings


class Coalition:
    def __init__(self, driver: 'Driver'):
        self.id = driver.id
        self.driver = driver
        # use for checking waitTime and detour constraints and updating shared distance
        self.route = Route(driver)
        self.curPassengers = []
        self.unitBid = 0

    def updateUnitBid(self):
        if len(self.curPassengers):
            passengerBids = [p.budget for p in self.curPassengers]
            self.unitBid = min(passengerBids)
        else:
            self.unitBid = 0

    def getPassengerNum(self):
        return len(self.curPassengers)

    def addPassenger(self, passenger: 'Passenger') -> bool:
        # radium constraint
        if Tool.calNodeDist(self.driver.getOrg(), passenger.getOrg()) > settings.RADIUS:
            return False
        # capacity constraint
        if self.driver.getAvaliableSeats() < passenger.getRequiredSeats():
            return False
        # add passenger
        if self.route.addPassenger(passenger):
            self.curPassengers.append(passenger)
            self.updateUnitBid()
            self.updateParticipantInfos(passenger, 1)
            return True
        else:
            return False

    def removePassenger(self, passenger: 'Passenger'):
        self.curPassengers.remove(passenger)
        self.updateUnitBid()
        new_route = Route(self.driver)
        for p in self.curPassengers:
            new_route.addPassenger(p)
        self.route = new_route  # update route
        self.updateParticipantInfos(passenger, 0)

    def updateParticipantInfos(self, passenger: 'Passenger', flag):
        # update shared distance passengers and driver
        self.route.updateSharedDist()
        # flag: 1 for add passenger
        if flag:
            # udpate shared seat and passenger numbers for driver, driverId for passenger
            self.driver.sSeatNum += passenger.getRequiredSeats()
            self.driver.sPassengerNum += 1
            passenger.driverId = self.driver.id
        # flag: 0 for remove passenger
        else:
            self.driver.sSeatNum -= passenger.getRequiredSeats()
            self.driver.sPassengerNum -= 1
            passenger.driverId = 0
            passenger.sDist = 0



