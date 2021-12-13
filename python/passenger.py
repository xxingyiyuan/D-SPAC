from tool import Tool
from settings import SPEED, PRICE_PER_KILOMETER, PRICE_PER_MINUTE


class Passenger:
    def __init__(self, id, demand):
        # demand: (pickup_longitude, pickup_latitude, dropoff_longitude, dropoff_latitude, seatNum, detourRatio, waitTime)
        self.id = id
        self.demand = demand
        self.iDist = Tool.calNodeDist(self.getOrg(), self.getDst())
        self.budget = self.iDist*PRICE_PER_KILOMETER + \
            self.getTime(self.iDist)*PRICE_PER_MINUTE
        self.driverId = 0
        self.sDist = 0
        self.payment = 0
        self.isWin = False

    def getUtility(self):
        if self.isWin:
            return self.budget - self.payment
        else:
            return 0

    def getTime(self, dist):
        return dist/SPEED

    def isSelect(self):
        return self.driverId != 0

    def getOrg(self):
        return (self.demand[0], self.demand[1])

    def getDst(self):
        return (self.demand[2], self.demand[3])

    def getRequiredSeats(self):
        return self.demand[4]

    def getDetourRatio(self):
        return self.demand[5]

    def getWaitTime(self):
        return self.demand[6]

    def gatherAttrs(self):
        return ", ".join("'{}': {}"
                         .format(k, getattr(self, k))
                         for k in self.__dict__.keys())

    def __str__(self):
        return "{{{}}}".format(self.gatherAttrs())


if __name__ == '__main__':
    pass
