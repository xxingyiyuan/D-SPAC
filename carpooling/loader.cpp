#include "loader.h"

Loader::Loader(const string &path, const int &waitTime, const double &detourRatio, const bool &isRand)
    : m_path(path)
{
    fstream in(this->m_path, ios::in);
    if (!in.is_open())
        cout << "Failed to open the file : " << this->m_path << endl;

    default_random_engine e(0);
    uniform_real_distribution<double> w(waitTime, waitTime + 1); //generate double number
    uniform_real_distribution<double> d(detourRatio, detourRatio + 0.1);
    uniform_int_distribution<unsigned> s(1, 2);                  //generate int number

    string str;
    while (getline(in, str))
    {

        stringstream ss;
        ss << str;
        double orgLong, orgLat, dstLong, dstLat;
        ss >> orgLong >> orgLat >> dstLong >> dstLat;

        Coordinate org(orgLong, orgLat);
        Coordinate dst(dstLong, dstLat);
        // Random for the demands of passengers, fixed for the demands of drivers
        int seat = isRand ? s(e) : AVAILABLE_SEAT;
        double wait_time = w(e);
        double detour_ratio = d(e);
        Demand de(org, dst, wait_time, detour_ratio, seat);
        // Demand de(org, dst, waitTime, detourRatio, seat);
        this->m_deList.push_back(de);
    }
    in.close();
}

vector<Demand> Loader::getDemandList() const
{
    return this->m_deList;
}
