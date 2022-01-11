## File Description

algorithm_cfg_os_2.cpp ----->  algorithm CFA 

algorithm_pack.cpp -----> algorithm TSG 

algorithm_mwbm.cpp -----> algorithm OTMBM

/Beijing -----> Requests based on Beijing dataset

/Guangzhou -----> Requests based on Guangzhou dataset

/ Manhattan ----->  Requests based on Manhattan dataset

## Build Setup

1.   Configure parameters for different data sets

- Beijing

```c++
const string DRIVER_DEMAND_PATH = "./Beijing/drivers_requests/requests_";
const string PASSENGER_DEMAND_PATH = "./Beijing/passengers_requests/requests_";

const double LEFTBOTTOM_CORNER_LONGITUDE =  116.330087;
const double LEFTBOTTOM_CORNER_LATITUDE =  39.846352;

const double REGION_WIDTH = 0.010109;
const double REGION_HEIGHT = 0.010109;
```

- Guangzhou

```c++
const string DRIVER_DEMAND_PATH = "./Manhattan/drivers_requests/requests_";
const string PASSENGER_DEMAND_PATH = "./Manhattan/passengers_requests/requests_";

const double LEFTBOTTOM_CORNER_LONGITUDE =  113.222425;
const double LEFTBOTTOM_CORNER_LATITUDE =  23.097940;

const double REGION_WIDTH = 0.010086;
const double REGION_HEIGHT = 0.010086;
```

- Manhattan

```c++
const string DRIVER_DEMAND_PATH = "./Manhattan/drivers_requests/requests_";
const string PASSENGER_DEMAND_PATH = "./Manhattan/passengers_requests/requests_";

const double LEFTBOTTOM_CORNER_LONGITUDE =  -74.018184;
const double LEFTBOTTOM_CORNER_LATITUDE =  40.700422;

const double REGION_WIDTH = 0.009297;
const double REGION_HEIGHT = 0.009843;
```

2. Compile

`g++ *.cpp -o main.exe`

3. Run Demo

`main.exe`

