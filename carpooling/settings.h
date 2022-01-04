#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

using namespace std;

// PATH
const string DRIVER_DEMAND_PATH = "./Beijing/drivers_requests/requests_";
const string PASSENGER_DEMAND_PATH = "./Beijing/passengers_requests/requests_";

//const string DRIVER_DEMAND_PATH = "./Manhattan/drivers_requests/requests_";
//const string PASSENGER_DEMAND_PATH = "./Manhattan/passengers_requests/requests_";

// const string DRIVER_DEMAND_PATH = "./Guangzhou/drivers_requests/requests_";
// const string PASSENGER_DEMAND_PATH = "./Guangzhou/passengers_requests/requests_";

const string UNDERLINE = "_";
const string FILE_SUFFIX = ".txt";

// NUMBER
const int DRIVER_NUM = 300;
const int PASSENGER_NUM = 600;
const int FILE_TOTAL_NUM = 10;
const int INDICATOR_NUM = 17;

// NETWORK & REGION
// Beijing   116.330087, 39.846352
// Manhattan -74.018184, 40.700422
// Guangzhou 113.222425, 23.097940
const double LEFTBOTTOM_CORNER_LONGITUDE =  116.330087;
const double LEFTBOTTOM_CORNER_LATITUDE =  39.846352;
const int ROW_NUM = 11;
const int COL_NUM = 11;

// Beijing   0.010109, 0.010109
// Manhattan 0.009297, 0.009843
// Guangzhou 0.010086, 0.010086
const double REGION_WIDTH = 0.010109;
const double REGION_HEIGHT = 0.010109;
// DEMAND
const int MAX_WAIT_TIME = 5;                        //  Default: 5
const double MAX_DETOUR_RATIO = 0.5;                // Default: 0.5
const int REQUIRED_SEAT = 2;
const int AVAILABLE_SEAT = 5;
const int RADIUS = 5;
// SPEED
const double SPEED = 48.0 / 60;

// ECONOMIC UTILITY
const double PRICE_PER_KILOMETER = 2.50;
const double PRICE_PER_MINUTE = 0.40;
const double FUEL_PRICE_PER_KILOMETER = 2.00;
const double LABOUR_PRICE_PER_MINUTE = 0.30;
const double RATIO_EXCLUDE_PLATFORM_CHARGE = 0.9;   // [1, 0.9, 0.8, 0.7, 0.6] - Default: 0.9
const double DISTCOUNT = 0.5;

// TRUTHFULNESS
const double BID_RATIO = 0.0;                       //  Default: 0.0

// ROUND
const int ROUND = 3;                                

#endif // SETTINGS_H
