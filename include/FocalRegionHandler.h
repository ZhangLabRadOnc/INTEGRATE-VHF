/*
 * FocalRegionHandler.h
 *
 *  Created on: Aug 2, 2013
 *      Author: jinzhang
 */

#ifndef FOCALREGIONHANDLER_H_
#define FOCALREGIONHANDLER_H_

#include "MyTypes.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

// Could use interval tree;

class FocalRegionHandler {

  public:
    int getUion(vector<region_to_map_t> &vtp, vector<region_to_map_t> &vtup);
};

#endif /* FOCALREGIONHANDLER_H_ */
