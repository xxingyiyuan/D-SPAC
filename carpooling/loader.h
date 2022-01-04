#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <random>
#include <algorithm>
#include "coordinate.h"
#include "demand.h"
#include "settings.h"
#include <ctime>
using namespace std;

class Loader
{
public:
    Loader(const string &path, const int &waitTime, const double &detourRatio, const bool &isRand);

    // Getter
    vector<Demand> getDemandList() const;

private:
    string m_path;              // The path
    vector<Demand> m_deList;      // The container storing the demands stored in 'path'
};

#endif // LOADER_H
