#include "network.h"

Network::Network(const Coordinate &lbCorner, const int &rowNum, const int &colNum, const double &reWidth, const double &reHeight)
    : m_lbCorner(lbCorner), m_rowNum(rowNum), m_colNum(colNum),
      m_reWidth(reWidth), m_reHeight(reHeight)
{
    initRegionMatrix();
}

Network::~Network()
{
    for (int i = this->m_rowNum - 1; i >= 0; --i)
    {

        for (int j = this->m_colNum - 1; j >= 0; --j)
        {

            delete this->m_reMat[i][j];
            this->m_reMat[i].pop_back();
        }
        this->m_reMat.pop_back();
    }
}

/**
 * @brief Add the driver into the corresponding region
 * @param dri:
 *  The driver
 */
void Network::addDriver2Region(Driver *dri)
{
    double width = dri->getOrg().getLongitude() - LEFTBOTTOM_CORNER_LONGITUDE;
    double height = dri->getOrg().getLatitude() - LEFTBOTTOM_CORNER_LATITUDE;

    int colIdx = static_cast<int>(floor(width / this->m_reWidth));
    int rowIdx = static_cast<int>(floor(height / this->m_reHeight));

    this->m_reMat[rowIdx][colIdx]->addInnerDriver(dri);
}

/**
 * @brief Select candidates
 * @param pass:
 *  The passenger
 * @param caVec:
 *  The container storing the corresponding candidate drivers
 */
void Network::selectCandidate(Passenger *pass, vector<Driver *> &caVec)
{
    double width = pass->getOrg().getLongitude() - LEFTBOTTOM_CORNER_LONGITUDE;
    double height = pass->getOrg().getLatitude() - LEFTBOTTOM_CORNER_LATITUDE;
    // the region which pass belongs to
    int colIdx = static_cast<int>(floor(width / this->m_reWidth));
    int rowIdx = static_cast<int>(floor(height / this->m_reHeight));

    // Find the boundary index of region within the waiting time constraint
    int bound = 0;
    Region *re = this->m_reMat[rowIdx][colIdx];
    list<double> distList = re->getRestRegionDistList();
    for (list<double>::iterator it = distList.begin(); it != distList.end(); ++it)
        if (*it / SPEED <= RADIUS)
            ++bound;
        else
            break;

    // Select the candidate drivers for 'pass' in the located region
    list<Driver *> iDriList = re->getInnerDriverList();
    for (list<Driver *>::iterator it = iDriList.begin(); it != iDriList.end(); ++it)
        if (judgeConstraint(pass, *it))
            caVec.push_back(*it);

    // Select the candidate drivers for 'pass' in the all other region
    list<Region *> reList = re->getRestRegionList();
    for (list<Region *>::iterator it = reList.begin(); it != reList.end() && bound; ++it, --bound)
    {

        iDriList = (*it)->getInnerDriverList();
        for (list<Driver *>::iterator iter = iDriList.begin(); iter != iDriList.end(); ++iter)
            if (judgeConstraint(pass, *iter))
                caVec.push_back(*iter);
    }
}

Coordinate Network::getLeftBottomCorner() const
{
    return this->m_lbCorner;
}

int Network::getRowNum() const
{
    return this->m_rowNum;
}

int Network::getColNum() const
{
    return this->m_colNum;
}

double Network::getRegionWidth() const
{
    return this->m_reWidth;
}

double Network::getRegionHeight() const
{
    return this->m_reHeight;
}

vector<vector<Region *>> Network::getRegionMatrix() const
{
    return this->m_reMat;
}

/**
 * @brief Initial the region matrix in network
 */
void Network::initRegionMatrix()
{
    // Generate all regions
    for (int i = 1; i <= this->m_rowNum; ++i)
    {

        vector<Region *> reVec;
        for (int j = 1; j <= this->m_colNum; ++j)
        {

            Coordinate centre(this->m_lbCorner.getLongitude() + (2 * j - 1) / 2 * this->m_reWidth,
                              this->m_lbCorner.getLatitude() + (2 * i - 1) / 2 * this->m_reHeight);
            Region *re = new Region((i - 1) * this->m_colNum + j, centre);
            reVec.push_back(re);
        }
        this->m_reMat.push_back(reVec);
    }

    // Calculate the distances among regions
    int total = this->m_rowNum * this->m_colNum;
    vector<vector<double>> distMat;
    //init
    for (int i = 0; i < total; ++i)
    {

        vector<double> distVec;
        for (int j = 0; j < total; ++j)
            distVec.push_back(0);
        distMat.push_back(distVec);
    }

    for (int i = 0; i < total; ++i)
    {

        int rowIdx = static_cast<int>(floor(i / this->m_colNum));
        int colIdx = i - rowIdx * this->m_colNum;
        for (int j = rowIdx; j < this->m_rowNum; ++j)
        {

            for (int k = 0; k < this->m_colNum; ++k)
            {
                //dist -- the distance between region i and m_reMat[j][k]
                double dist = this->m_reMat[rowIdx][colIdx]->getCentre().calDist(this->m_reMat[j][k]->getCentre());
                // j * this->m_colNum + k -- the region id
                distMat[i][j * this->m_colNum + k] = dist;
                distMat[j * this->m_colNum + k][i] = dist;
            }
        }
    }

    // Build relationship among all regions
    buildRelationship(distMat);
}

/**
 * @brief Build relationship among all regions
 * @param distMat:
 *  The distance matrix
 */
void Network::buildRelationship(const vector<vector<double>> &distMat)
{
    int total = this->m_rowNum * this->m_colNum;
    for (int i = 0; i < total; ++i)
    {

        int rowIdx = static_cast<int>(floor(i / this->m_colNum));
        int colIdx = i - rowIdx * this->m_colNum;
        Region *re = this->m_reMat[rowIdx][colIdx];

        vector<double> distVec;
        vector<int> idxVec;
        // distVec -- the distance between region i and region j
        for (int j = 0; j < total; ++j)
        {

            idxVec.push_back(j);
            distVec.push_back(distMat[re->getId() - 1][j]);
        }

        // Use the selection sort to build relationship among regions based on the distances
        int bound = total;
        while (bound)
        {

            int idx = 0;
            for (int k = idx + 1; k < bound; ++k)
                if (distVec[idx] > distVec[k])
                    idx = k;
            // idx -- the region idx with the shortest distance from region i
            int row = static_cast<int>(floor(idxVec[idx] / this->m_colNum));
            int col = idxVec[idx] - row * this->m_colNum;

            if (idxVec[idx] + 1 != re->getId())
                re->addRestRegion(this->m_reMat[row][col], distVec[idx]);

            idxVec.erase(idxVec.begin() + idx);
            distVec.erase(distVec.begin() + idx);
            --bound;
        }
    }
}

/**
 * @brief Judge whether constraints between 'pass' and 'dri' are satisfied
 * @param pass:
 *  The passenger
 * @param dri:
 *  The driver
 * @return True or False
 */
bool Network::judgeConstraint(Passenger *pass, Driver *dri)
{
    // Radius constraint
    if (pass->getOrg().calDist(dri->getOrg()) > RADIUS)
        return false;

    // Capacity constraint
    if (pass->getSeatNum() > dri->getSeatNum() - dri->getSharedSeatNum())
        return false;

    // Waiting time constraint
    if ((dri->getOrg().calDist(pass->getOrg()) / SPEED - pass->getWaitTime()) > 1e-6)
        return false;

    // Detour constraint
    double dist = dri->getOrg().calDist(pass->getOrg()) + pass->getInitDist() + pass->getDst().calDist(dri->getDst()) - dri->getInitDist();
    if ((dist / dri->getInitDist() - dri->getDeRatio()) > 1e-6)
        return false;

    return true;
}
