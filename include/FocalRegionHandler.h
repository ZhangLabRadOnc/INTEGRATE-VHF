/*
 * FocalRegionHandler.h
 *
 *  Created on: Aug 2, 2013
 *      Author: jinzhang
 */

#pragma once

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
