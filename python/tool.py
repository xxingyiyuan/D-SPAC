import matplotlib.pyplot as plt
import math
import numpy as np
from settings import SPEED, RADIUS
import matplotlib
matplotlib.use('tkAgg')


class Tool:
    @classmethod
    def calNodeDist(cls, coordinate1, coordinate2) -> float:
        return math.sqrt(
            math.pow((coordinate1[0] - coordinate2[0]) / 0.00001, 2) +
            math.pow((coordinate1[1] - coordinate2[1]) / 0.00001, 2)
        ) / 1000

    @classmethod
    def getCandidateActions(cls, driList, passList):
        M = len(driList) + 1
        n = len(passList)
        candidateActions = [M*i for i in range(n)]
        cadidateTable = []
        for passIndex, passenger in enumerate(passList):
            canDri = []
            for driverId, driver in enumerate(driList, start=1):
                if cls.judgeConstraint(passenger, driver):
                    canDri.append(driverId)
                    candidateActions.append(M*passIndex + driverId)
            cadidateTable.append(canDri)

        return candidateActions, cadidateTable

    @classmethod
    def judgeConstraint(cls, passenger, driver) -> bool:
        # radium constraint for driver
        orgDist = cls.calNodeDist(driver.getOrg(), passenger.getOrg())
        if orgDist > RADIUS:
            return False
        # waitTime constraint for passenger
        if orgDist/SPEED > passenger.getWaitTime():
            return False
        # detour constraint for driver
        sharedDist = orgDist + passenger.iDist + \
            cls.calNodeDist(driver.getDst(), passenger.getDst())
        if sharedDist > (1+driver.getDetourRatio())*driver.iDist:
            return False
        return True

    @classmethod
    def plotData(cls, data, labels: tuple, filename):
        plt.figure(str(labels))
        plt.plot(data)
        plt.xlabel(labels[0])
        plt.ylabel(labels[1])
        # plt.savefig('./figures/{}.jpg'.format(filename))

    @classmethod
    def pltShow(cls):
        plt.show()
    
    @classmethod
    def storeData(cls, data: list, filename):
        data = np.array(data)
        np.save('./data/data_result/{}.npy'.format(filename), data)


