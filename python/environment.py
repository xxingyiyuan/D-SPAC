from generalRequest import Generator
import pandas as pd
import numpy as np
import settings
import random
from driver import Driver
from passenger import Passenger
from coalition import Coalition
from auctioneer import Auctioneer
from tool import Tool
# dataset
G = Generator(*settings.beijingRange)


class Environment:
    def __init__(self, drivers_num, passengers_num, file_num=0, waitTime=5, detourRatio=0.5):
        self.drivers_num = drivers_num
        self.passengers_num = passengers_num
        self.M = drivers_num + 1
        self.N = passengers_num
        self.auctioneer = Auctioneer()
        self.initDemands(waitTime, detourRatio, file_num)
        self.initParticipants()
        self.candidateActions, self.candidateTable = Tool.getCandidateActions(
            self.drivers, self.passengers)
        self.candidateIndex = [i for i in range(len(self.candidateActions))]
        self.actionMap = {}
        for (i, action) in enumerate(self.candidateActions):
            self.actionMap[action] = i
        self.passUti = self.getPassTotalUtility()
        self.passWindow = np.zeros(self.passengers_num)

    def initDemands(self, waitTime, detourRatio, file_num):
        # initialize demands
        # pickup_longitude  pickup_latitude  dropoff_longitude  dropoff_latitude  seatNum detourRatio waitTime

        st0 = np.random.get_state()
        seed = np.random.set_state(st0)
        random.seed(seed)
        np.random.seed(seed)
        # generate demands
        drivers_df = G.generateRequests(total_num=self.drivers_num, flag=0)
        passengers_df = G.generateRequests(
            total_num=self.passengers_num, flag=1)

        # --------- store demands
        # drivers_df.to_csv('./data/driver_requests/driver_requests_{}_{}.txt'.format(
        #     self.drivers_num, file_num), sep=' ', header=None, index=False)
        # passengers_df.to_csv('./data/passenger_requests/passenger_requests_{}_{}.txt'.format(
        #     self.passengers_num, file_num), sep=' ', header=None, index=False)

        # --------- read demands from files
        # drivers_df = pd.read_table('./data/driver_requests/driver_requests_{}_{}.txt'.format(self.drivers_num, file_num), sep=' ',
        #                            header=None, names=['pickup_longitude', 'pickup_latitude', 'dropoff_longitude', 'dropoff_latitude', 'seatNum'])
        # passengers_df = pd.read_table('./data/passenger_requests/passenger_requests_{}_{}.txt'.format(self.passengers_num, file_num), sep=' ',
        #                               header=None, names=['pickup_longitude', 'pickup_latitude', 'dropoff_longitude', 'dropoff_latitude', 'seatNum'])

        # add detour and waitTime
        drivers_df['detourRatio'] = detourRatio
        passengers_df['detourRatio'] = detourRatio
        passengers_df['waitTime'] = waitTime

        self.drivers_demand = drivers_df
        self.passengers_demand = passengers_df

    def initParticipants(self):
        drivers_demand = [tuple(de)
                          for de in self.drivers_demand.values]
        self.drivers = [Driver(id, de)
                        for id, de in enumerate(drivers_demand, start=1)]

        passengers_demand = [tuple(de)
                             for de in self.passengers_demand.values]
        self.passengers = [Passenger(id, de)
                           for id, de in enumerate(passengers_demand, start=1)]
        # initialize coalition
        self.coalitions = {}
        for d in self.drivers:
            self.coalitions[d.id] = Coalition(d)

    def resetEnv(self):
        # reset drivers and passengers
        self.initParticipants()
        self.passWindow = np.zeros(self.passengers_num)
        self.candidateIndex = [i for i in range(len(self.candidateActions))]
        self.passUti = self.getPassTotalUtility()
        return self.getObservation(), self.getPassTotalUtility()

    def initAssignment(self):
        for passIndex, canDri in enumerate(self.candidateTable):
            for driverId in canDri:
                coalition = self.coalitions[driverId]
                if coalition.addPassenger(self.passengers[passIndex]):
                    break
        self.auctioneer.auction(self.drivers, self.coalitions)

    def getObservation(self):
        return self.passWindow

    def getPassTotalUtility(self):
        pUti = [p.getUtility() for p in self.passengers]
        return sum(pUti)

    def getVaildIndex(self):
        # the index of assigned passengers
        invalidPassIndex = np.where(self.passWindow != 0)[0]
        invalidAction = [passIndex*self.M for passIndex in invalidPassIndex]
        for passIndex in invalidPassIndex:
            for driverId in self.candidateTable[passIndex]:
                action = passIndex*self.M + driverId
                invalidAction.append(action)
        invalidAction = list(set(invalidAction))
        candidateIndex = []
        for i, action in enumerate(self.candidateActions):
            if action in invalidAction:
                continue
            candidateIndex.append(i)
        return candidateIndex

    def step(self, action) -> tuple:
        # return: (observation_, reward, done)
        passengerIndex = int(action/self.M)
        passenger = self.passengers[passengerIndex]
        target_driverId = int(action % self.M)
        # no execution cur_driverId = target_driverId = 0
        if target_driverId == 0:
            reward = 0
            self.passWindow[passengerIndex] = 1
        else:
            # join: cur_driverId == 0 and target_driverId != 0 add passenger to target_driverId
            target_coalition = self.coalitions[target_driverId]
            flag = target_coalition.addPassenger(passenger)
            # break constraints
            if flag == False:
                reward = -50
                # self.passWindow[passengerIndex] = 1
                return (self.getObservation(), reward, 2)
            else:
                # join
                self.auctioneer.auction(self.drivers, self.coalitions)
                reward = (self.getPassTotalUtility() - self.passUti)
                self.passUti = self.getPassTotalUtility()
                self.passWindow[passengerIndex] = 1
        if self.passWindow[passengerIndex] == 1:
            self.updateValidIndex(passengerIndex)
        done = 0 not in self.passWindow
        observation_ = self.getObservation()
        return (observation_, reward, done)

    def updateValidIndex(self, passIndex):
        invalidAction = [passIndex*self.M]
        for driverId in self.candidateTable[passIndex]:
            invalidAction.append(passIndex*self.M + driverId)
        for action in invalidAction:
            actionIndex = self.actionMap[action]
            if actionIndex in self.candidateIndex:
                self.candidateIndex.remove(actionIndex)



