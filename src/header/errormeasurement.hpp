#ifndef _ERRORMEASUREMENT_HPP 
#define _ERRORMEASUREMENT_HPP 

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "quadtree.hpp"

using namespace std;

namespace ErrorMeasurement { 
    RGB computeAvgColor(const vector<vector<RGB>>& image, int x, int y, int width, int height);
    float computeVariance(const vector<vector<RGB>>& image, int x, int y, int width, int height); 
    float computeMAD(const vector<vector<RGB>>& image, int x, int y, int width, int height); 
    float computeMaxPixelDiff(const vector<vector<RGB>>& image, int x, int y, int width, int height); 
    float computeEntropy(const vector<vector<RGB>>& image, int x, int y, int width, int height); 
}

#endif
