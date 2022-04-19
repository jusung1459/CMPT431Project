#ifndef BINARYFILECONTROLLER_H
#define BINARYFILECONTROLLER_H

#include <fstream>
#include <iostream>
#include "core/cxxopts.h"
#include "core/get_time.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <vector>
using namespace std;

int binRead(vector<float> *array, const string& file, unsigned long N, unsigned long start);

int binWrite(vector<float> *array, const string& file, unsigned long N, int append);

void createFloatingPoints(vector<float> *arr, unsigned long N, int seed);

#endif 
