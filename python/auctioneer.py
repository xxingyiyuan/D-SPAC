from driver import Driver
from coalition import Coalition
from settings import RATIO_EXCLUDE_PLATFORM_CHARGE


class Auctioneer:
    # The auctioneer determines winning passengers and drivers, then returns matching result, payoffs and payments.

    def auction(self, drivers: list, coalitions: dict):
        # sort coalitions by unitBid in descending order
        # reverse=True: descending order
        tmp_coalitions = list(coalitions.values())
        tmp_coalitions.sort(
            key=lambda coalition: coalition.unitBid, reverse=True)
        coalitionNum = len(coalitions)
        for i, coalition in enumerate(tmp_coalitions):
            dri = drivers[coalition.id - 1]
            payoff = 0
            unitPayment = 0
            passNum = coalition.getPassengerNum()
            if coalition.unitBid*passNum*RATIO_EXCLUDE_PLATFORM_CHARGE < dri.askPrice:
                self.updateResult((coalition, dri),
                                  (unitPayment, payoff), False)
                continue
            # determine payments and payoffs

            for j in range(i+1, coalitionNum):
                if tmp_coalitions[j].unitBid*passNum*RATIO_EXCLUDE_PLATFORM_CHARGE >= dri.askPrice:
                    unitPayment = tmp_coalitions[j].unitBid
                else:
                    break

            if unitPayment:
                payoff = dri.askPrice
                self.updateResult((coalition, dri),
                                  (unitPayment, payoff), True)
            else:
                self.updateResult((coalition, dri),
                                  (unitPayment, payoff), False)

    # participants: (coalition, driver), price: (unitPayment, payoff), isWin
    def updateResult(self, participants: tuple, price: tuple, isWin: bool):
        coalition, driver = participants
        unitPayment, payoff = price
        # update passengers
        for p in coalition.curPassengers:
            p.isWin = isWin
            p.payment = unitPayment

        # update driver
        driver.isWin = isWin
        driver.payoff = payoff
