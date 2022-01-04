#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "coordinate.h"
#include "demand.h"
#include "loader.h"
#include "driver.h"
#include "passenger.h"
#include "network.h"
#include "algorithm_cfg_os_2.h"
#include "algorithm_pack.h"
#include "algorithm_mwbm.h"
#include "stdlib.h"

using namespace std;
using namespace chrono;

void initParticipants(vector<Demand> &driDeList, vector<Demand> &passDeList, Network *net, vector<Driver *> &driSet, vector<vector<Passenger *>> &dcaMat, vector<Passenger *> &passSet, vector<vector<Driver *>> &pcaMat)
{
    // Initialize the drivers
    int driId = 1;

    for (vector<Demand>::iterator it = driDeList.begin(); it != driDeList.end(); ++it, ++driId)
    {
        Driver *dri = new Driver(driId, *it, BID_RATIO);
        driSet.push_back(dri);
        net->addDriver2Region(dri);
    }
    // Initialize the passengers and get the candidate drivers for each passenger
    int passId = 1;
    for (vector<Demand>::iterator it = passDeList.begin(); it != passDeList.end(); ++it, ++passId)
    {
        Passenger *pass = new Passenger(passId, *it, BID_RATIO);
        passSet.push_back(pass);

        vector<Driver *> caVec;
        net->selectCandidate(pass, caVec);
        pcaMat.push_back(caVec);
    }

    // Get the candidate passengers for each driver
    for (vector<Driver *>::iterator it = driSet.begin(); it != driSet.end(); ++it)
    {
        vector<Passenger *> caVec;
        int outSize = passSet.size();
        for (int i = 0; i < outSize; ++i)
        {

            int inSize = pcaMat[i].size();
            for (int j = 0; j < inSize; ++j)
                if (pcaMat[i][j]->getId() == (*it)->getId())
                    caVec.push_back(passSet[i]);
        }
        dcaMat.push_back(caVec);
    }
}

vector<double> carpool_new(const int &select, const string &driNum, const string &passNum, const string &fileNum, const int &waitTime, const double &detourRatio)
{
    // Load demands for both drivers and passengers
    Loader *driLoader = new Loader(DRIVER_DEMAND_PATH + driNum + UNDERLINE + fileNum + FILE_SUFFIX, waitTime, detourRatio, 0);
    Loader *passLoader = new Loader(PASSENGER_DEMAND_PATH + passNum + UNDERLINE + fileNum + FILE_SUFFIX, waitTime, detourRatio, 1);
    vector<Demand> driDeList = driLoader->getDemandList();
    vector<Demand> passDeList = passLoader->getDemandList();

    // Initialize the road network
    Coordinate coor(LEFTBOTTOM_CORNER_LONGITUDE, LEFTBOTTOM_CORNER_LATITUDE);
    Network *net = new Network(coor, ROW_NUM, COL_NUM, REGION_WIDTH, REGION_HEIGHT);

    // Initialize the drivers and passengers
    vector<Driver *> driSet;
    vector<vector<Passenger *>> dcaMat;
    vector<Passenger *> passSet;
    vector<vector<Driver *>> pcaMat;
    initParticipants(driDeList, passDeList, net, driSet, dcaMat, passSet, pcaMat);

    // Start timer
    auto start = system_clock::now();

    // Algorithms
    Algorithm_CFG_OS_2 *cfg_os = nullptr;
    Algorithm_PACK *alg_pack = nullptr;
    Algorithm_MWBM *alg_mwbm = nullptr;
    switch (select)
    {

    case 1:
        cfg_os = new Algorithm_CFG_OS_2(driSet, dcaMat, passSet, pcaMat);
        break;
    case 2:
        alg_pack = new Algorithm_PACK(driSet, dcaMat, passSet, pcaMat);
        break;
    case 3:
        alg_mwbm = new Algorithm_MWBM(driSet, dcaMat, passSet, pcaMat);
        break;
    }

    // Stop timer
    auto end = system_clock::now();

    // Calcluate the execution time (second)
    auto duration = duration_cast<microseconds>(end - start);
    double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;

    vector<double> out;
    double socialWelfare = 0;         // Social welfare
    double passUtility = 0;           // Overall utility of all passengers (i.e., all winning passengers)
    double driUtility = 0;            // Overall utility of all drivers (i.e., all winning drivers)
    double passValuation = 0;         // Overall valuation of all passengers
    double passWinValuation = 0;      // Overall valuation of all winning passengers
    double passWinPayment = 0;        // Overall payment of all winning passengers
    double driCost = 0;               // Overall valuation of all drivers
    double driWinCost = 0;            // Overall detour cost of all winning drivers
    double driWinPayoff = 0;          // Overall payoff of all winning drivers
    double originDistance = 0;        // Overall distance in non-sharing travel of all passengers and drivers
    double passWinOriginDistance = 0; // Overall distance in non-sharing travel of all winning passengers
    double driWinOriginDistance = 0;  // Overall distance in non-sharing travel of all winning drivers
    double driWinShareDistance = 0;   // Overall distance in sharing travel of all winning drivers
    double passWinNumber = 0;         // Number of winning passengers
    double driWinNumber = 0;          // Number of winning drivers
    double seatNum = 0;               // Number of sharing seats

    for (vector<Passenger *>::iterator it = passSet.begin(); it != passSet.end(); ++it)
    {
        passValuation += (*it)->getInitFare();
        originDistance += (*it)->getInitDist();
        if ((*it)->getIsShare())
        {
            //winner passengers
            socialWelfare += (*it)->getInitFare();
            passUtility += (*it)->getUti();
            passWinValuation += (*it)->getInitFare();
            passWinPayment += (*it)->getFare();
            passWinOriginDistance += (*it)->getInitDist();
            ++passWinNumber;
        }

        delete (*it);
    }

    for (vector<Driver *>::iterator it = driSet.begin(); it != driSet.end(); ++it)
    {

        driCost += (*it)->getBid();
        originDistance += (*it)->getInitDist();
        if ((*it)->getIsShare())
        {
            //winner drivers
            socialWelfare -= (*it)->getDetourCost();
            driUtility += (*it)->getUti();
            driWinCost += (*it)->getDetourCost();
            driWinPayoff += (*it)->getPayoff();
            driWinOriginDistance += (*it)->getInitDist();
            driWinShareDistance += (*it)->getDist();
            ++driWinNumber;
            seatNum += (*it)->getSharedSeatNum();
        }
        delete (*it);
    }

    out.push_back(socialWelfare);
    out.push_back(passUtility);
    out.push_back(driUtility);
    out.push_back(passValuation);
    out.push_back(passWinValuation);
    out.push_back(passWinPayment);
    out.push_back(driCost);
    out.push_back(driWinCost);
    out.push_back(driWinPayoff);
    out.push_back(originDistance);
    out.push_back(passWinOriginDistance);
    out.push_back(driWinOriginDistance);
    out.push_back(driWinShareDistance);
    out.push_back(passWinNumber);
    out.push_back(driWinNumber);
    out.push_back(seatNum);
    out.push_back(time);

    // Release the used resources
    if (cfg_os != nullptr)
        delete cfg_os;
    if (alg_pack != nullptr)
        delete alg_pack;
    if (alg_mwbm != nullptr)
        delete alg_mwbm;

    delete driLoader;
    delete passLoader;
    delete net;

    return out;
}

void collectData_new(const int &waitTime, const double &detourRatio)
{
    // Collected Data from algorithms CFG_OS, CFG_NO, RANK_OS & RANK_NO
    /* **
     * 1.   Social welfare
     * 2.   Overall utility of all passengers (i.e., all winning passengers)
     * 3.   Overall utility of all drivers (i.e., all winning drivers)
     * 4.   Overall valuation of all passengers
     * 5.   Overall valuation of all winning passengers
     * 6.   Overall payment of all winning passengers
     * 7.   Overall valuation of all drivers
     * 8.   Overall detour cost of all winning drivers
     * 9.   Overall payoff of all winning drivers
     * 10.  Overall distance in non-sharing travel of all passengers and drivers
     * 11.  Overall distance in non-sharing travel of all winning passengers
     * 12.  Overall distance in non-sharing travel of all winning drivers
     * 13.  Overall distance in sharing travel of all winning drivers
     * 14.  Number of winning passengers
     * 15.  Number of winning drivers
     * 16.  Number of sharing seats
     * 17.  Running time
     * **/

    vector<double> cfgOS(INDICATOR_NUM, 0), pack(INDICATOR_NUM, 0);
// 
    for (int i = 1; i <= FILE_TOTAL_NUM; ++i)
    {

        vector<double> tmp;
        tmp = carpool_new(1, to_string(DRIVER_NUM), to_string(PASSENGER_NUM), to_string(i), waitTime, detourRatio);
        for (vector<double>::iterator o = cfgOS.begin(), t = tmp.begin(); o != cfgOS.end(); ++o, ++t)
            *o = *o + *t;

        tmp.clear();
        tmp = carpool_new(3, to_string(DRIVER_NUM), to_string(PASSENGER_NUM), to_string(i), waitTime, detourRatio);
        for (vector<double>::iterator o = pack.begin(), t = tmp.begin(); o != pack.end(); ++o, ++t)
            *o = *o + *t;
    }

    cout << "# (driver): " << DRIVER_NUM << " - # (passengers) " << PASSENGER_NUM << " - Watiting Time: "
         << waitTime << " - Detour Ratio: " << setprecision(3) << detourRatio;

    cout << endl
         << "ALG1 CFG_OS \t\t\t\t ALG2 PACK" << endl;
    cout << "Social welfare: " << setprecision(5) << cfgOS[0] / FILE_TOTAL_NUM << ", " << pack[0] / FILE_TOTAL_NUM << endl
         << "Utility (Passengers): " << setprecision(5) << cfgOS[1] / FILE_TOTAL_NUM << ", " << pack[1] / FILE_TOTAL_NUM << endl
         << "Utility (Drivers): " << setprecision(5) << cfgOS[2] / FILE_TOTAL_NUM << ", " << pack[2] / FILE_TOTAL_NUM << endl
         << "Valuation (all passengers): " << setprecision(5) << cfgOS[3] / FILE_TOTAL_NUM << ", " << pack[3] / FILE_TOTAL_NUM << endl
         << "Valuation (Winning passengers): " << setprecision(5) << cfgOS[4] / FILE_TOTAL_NUM << ", " << pack[4] / FILE_TOTAL_NUM << endl
         << "Payment (Winning passengers): " << setprecision(5) << cfgOS[5] / FILE_TOTAL_NUM << ", " << pack[5] / FILE_TOTAL_NUM << endl
         << "Valuation (all drivers): " << setprecision(5) << cfgOS[6] / FILE_TOTAL_NUM << ", " << pack[6] / FILE_TOTAL_NUM << endl
         << "Detour cost (Winning drivers):" << setprecision(5) << cfgOS[7] / FILE_TOTAL_NUM << ", " << pack[7] / FILE_TOTAL_NUM << endl
         << "Payoff (Winning drivers):" << setprecision(5) << cfgOS[8] / FILE_TOTAL_NUM << ", " << pack[8] / FILE_TOTAL_NUM << endl
         << "Distance (Non):" << setprecision(5) << cfgOS[9] / FILE_TOTAL_NUM << ", " << pack[9] / FILE_TOTAL_NUM << endl
         << "Distance (Non, Winning passengers):" << setprecision(5) << cfgOS[10] / FILE_TOTAL_NUM << ", " << pack[10] / FILE_TOTAL_NUM << endl
         << "Distance (Non, Winning drivers):" << setprecision(5) << cfgOS[11] / FILE_TOTAL_NUM << ", " << pack[11] / FILE_TOTAL_NUM << endl
         << "Distance (Share, Winning driver):" << setprecision(5) << cfgOS[12] / FILE_TOTAL_NUM << ", " << pack[12] / FILE_TOTAL_NUM << endl
         << "Number (Winning passengers):" << setprecision(5) << cfgOS[13] / FILE_TOTAL_NUM << ", " << pack[13] / FILE_TOTAL_NUM << endl
         << "Number (Winning drivers):" << setprecision(5) << cfgOS[14] / FILE_TOTAL_NUM << ", " << pack[14] / FILE_TOTAL_NUM << endl
         << "Number (Share seats):" << setprecision(5) << cfgOS[15] / FILE_TOTAL_NUM << ", " << pack[15] / FILE_TOTAL_NUM << endl
         << "Running time:" << setprecision(4) << cfgOS[16] / FILE_TOTAL_NUM << ", " << pack[16] / FILE_TOTAL_NUM << endl
         << "fare saving ratio:" << setprecision(4) << cfgOS[1] / cfgOS[4] << ", " << pack[1] / pack[4] << endl;
    cout << endl;
}

void collectData(int select, vector<double> &res, int waitTime, double detourRatio)
{
    // FILE_TOTAL_NUM
    for (int i = 1; i <= FILE_TOTAL_NUM; ++i)
    {
        vector<double> tmp;
        tmp = carpool_new(select, to_string(DRIVER_NUM), to_string(PASSENGER_NUM), to_string(i), waitTime, detourRatio);
        for (vector<double>::iterator r = res.begin(), t = tmp.begin(); r != res.end(); ++r, ++t)
            *r = *r + *t;

        tmp.clear();
    }
    for (vector<double>::iterator r = res.begin(); r != res.end(); r++)
    {
        *r = *r / FILE_TOTAL_NUM;
    }
}

void outputData(int select)
{
    vector<vector<double>> res1(5, vector<double>(INDICATOR_NUM, 0));
    vector<vector<double>> res2(5, vector<double>(INDICATOR_NUM, 0));
    vector<vector<double>> res3(5, vector<double>(INDICATOR_NUM, 0));
    int waitTime = 4, detourRatio = 5;
    int &t = select == 0 ? waitTime : detourRatio; 
    t = 2;
    for (int i = 0; i <= 4; ++t, ++i)
    {
        cout << "waitTime = " << waitTime << ", detourRatio = " << detourRatio << endl;
        collectData(1, res1[i], waitTime, detourRatio*0.1);
        collectData(2, res2[i], waitTime, detourRatio*0.1);
        collectData(3, res3[i], waitTime, detourRatio*0.1);
    }
    time_t now = time(0);
    string s = select == 0? "waitingTime" : "detourRatio";
    ofstream oFile;
    oFile.open("./result.xls", ios::app);
    oFile << endl;
    oFile << ctime(&now) << endl;
    oFile << s
          << "\t"
          << "2"
          << "\t"
          << "3"
          << "\t"
          << "4"
          << "\t"
          << "5"
          << "\t"
          << "6"
          << endl;
    oFile << endl;

    oFile << "passenger utility"
          << "\t"
          << res1[0][1]
          << "\t"
          << res1[1][1]
          << "\t"
          << res1[2][1]
          << "\t"
          << res1[3][1]
          << "\t"
          << res1[4][1]
          << endl;
    oFile << " "
          << "\t"
          << res2[0][1]
          << "\t"
          << res2[1][1]
          << "\t"
          << res2[2][1]
          << "\t"
          << res2[3][1]
          << "\t"
          << res2[4][1]
          << endl;
    oFile << " "
          << "\t"
          << res3[0][1]
          << "\t"
          << res3[1][1]
          << "\t"
          << res3[2][1]
          << "\t"
          << res3[3][1]
          << "\t"
          << res3[4][1]
          << endl;
    oFile << endl;

    oFile << "driver utility"
          << "\t"
          << res1[0][2]
          << "\t"
          << res1[1][2]
          << "\t"
          << res1[2][2]
          << "\t"
          << res1[3][2]
          << "\t"
          << res1[4][2]
          << endl;
    oFile << " "
          << "\t"
          << res2[0][2]
          << "\t"
          << res2[1][2]
          << "\t"
          << res2[2][2]
          << "\t"
          << res2[3][2]
          << "\t"
          << res2[4][2]
          << endl;
    oFile << " "
          << "\t"
          << res3[0][2]
          << "\t"
          << res3[1][2]
          << "\t"
          << res3[2][2]
          << "\t"
          << res3[3][2]
          << "\t"
          << res3[4][2]
          << endl;
    oFile << endl;

    oFile << "running time"
          << "\t"
          << res1[0][16]
          << "\t"
          << res1[1][16]
          << "\t"
          << res1[2][16]
          << "\t"
          << res1[3][16]
          << "\t"
          << res1[4][16]
          << endl;
    oFile << " "
          << "\t"
          << res2[0][16]
          << "\t"
          << res2[1][16]
          << "\t"
          << res2[2][16]
          << "\t"
          << res2[3][16]
          << "\t"
          << res2[4][16]
          << endl;
    oFile << " "
          << "\t"
          << res3[0][16]
          << "\t"
          << res3[1][16]
          << "\t"
          << res3[2][16]
          << "\t"
          << res3[3][16]
          << "\t"
          << res3[4][16]
          << endl;
    oFile << endl;

    oFile << "number of winning passengers"
          << "\t"
          << res1[0][13]
          << "\t"
          << res1[1][13]
          << "\t"
          << res1[2][13]
          << "\t"
          << res1[3][13]
          << "\t"
          << res1[4][13]
          << endl;
    oFile << " "
          << "\t"
          << res2[0][13]
          << "\t"
          << res2[1][13]
          << "\t"
          << res2[2][13]
          << "\t"
          << res2[3][13]
          << "\t"
          << res2[4][13]
          << endl;
     oFile << " "
          << "\t"
          << res3[0][13]
          << "\t"
          << res3[1][13]
          << "\t"
          << res3[2][13]
          << "\t"
          << res3[3][13]
          << "\t"
          << res3[4][13]
          << endl;
    oFile << endl;

    oFile << "number of winning drivers"
          << "\t"
          << res1[0][14]
          << "\t"
          << res1[1][14]
          << "\t"
          << res1[2][14]
          << "\t"
          << res1[3][14]
          << "\t"
          << res1[4][14]
          << endl;
    oFile << " "
          << "\t"
          << res2[0][14]
          << "\t"
          << res2[1][14]
          << "\t"
          << res2[2][14]
          << "\t"
          << res2[3][14]
          << "\t"
          << res2[4][14]
          << endl;
    oFile << " "
          << "\t"
          << res3[0][14]
          << "\t"
          << res3[1][14]
          << "\t"
          << res3[2][14]
          << "\t"
          << res3[3][14]
          << "\t"
          << res3[4][14]
          << endl;
    oFile << endl;

    oFile << "fare saving rate"
          << "\t"
          << res1[0][1] / res1[0][4]
          << "\t"
          << res1[1][1] / res1[1][4]
          << "\t"
          << res1[2][1] / res1[2][4]
          << "\t"
          << res1[3][1] / res1[3][4]
          << "\t"
          << res1[4][1] / res1[4][4]
          << endl;
     oFile << " "
          << "\t"
          << res2[0][1] / res2[0][4]
          << "\t"
          << res2[1][1] / res2[1][4]
          << "\t"
          << res2[2][1] / res2[2][4]
          << "\t"
          << res2[3][1] / res2[3][4]
          << "\t"
          << res2[4][1] / res2[4][4]
          << endl;
    oFile << " "
          << "\t"
          << res3[0][1] / res3[0][4]
          << "\t"
          << res3[1][1] / res3[1][4]
          << "\t"
          << res3[2][1] / res3[2][4]
          << "\t"
          << res3[3][1] / res3[3][4]
          << "\t"
          << res3[4][1] / res3[4][4]
          << endl;
    oFile << endl;

    oFile.close();
}


int main()
{
    cout << "start.." << endl;
    outputData(0);
    outputData(1);
    system("pause");
    return 0;
}