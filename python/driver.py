from tool import Tool
from settings import SPEED, FUEL_PRICE_PER_KILOMETER, LABOUR_PRICE_PER_MINUTE


class Driver:
    def __init__(self, id, demand):
        # demand: (pickup_longitude, pickup_latitude, dropoff_longitude, dropoff_latitude, seatNum, detourRatio)
        self.id = id
        self.demand = demand
        self.iDist = Tool.calNodeDist(self.getOrg(), self.getDst())
        self.askPrice = self.iDist*FUEL_PRICE_PER_KILOMETER + \
            self.getTime(self.iDist)*LABOUR_PRICE_PER_MINUTE
        self.sDist = 0
        self.sSeatNum = 0
        self.sPassengerNum = 0
        self.payoff = 0
        self.isWin = False

    def getUtility(self):
        if self.isWin:
            return self.payoff - self.getdetourCost()
        else:
            return 0

    def getShareTotalCost(self):
        if self.sDist == 0:
            return 0
        else:
            return self.sDist*FUEL_PRICE_PER_KILOMETER + \
                self.getTime(self.sDist)*LABOUR_PRICE_PER_MINUTE

    def getTime(self, dist):
        return dist/SPEED

    def getdetourCost(self):
        if self.sDist == 0:
            return 0
        else:
            return self.getShareTotalCost() - self.askPrice()

    def getOrg(self):
        return (self.demand[0], self.demand[1])

    def getDst(self):
        return (self.demand[2], self.demand[3])

    def getAvaliableSeats(self):
        return self.demand[4] - self.sSeatNum

    def getDetourRatio(self):
        return self.demand[5]

    def gatherAttrs(self):
        return ", ".join("'{}': {}"
                         .format(k, getattr(self, k))
                         for k in self.__dict__.keys())

    def __str__(self):
        return "{{{}}}".format(self.gatherAttrs())


if __name__ == '__main__':
    pass
