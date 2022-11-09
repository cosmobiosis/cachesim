#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "cache.h"

using namespace::std;

namespace MemoryCacheSim
{

/*
*/
RWObject::RWObject(char* data): _data(data) {
    if (sizeof(data) == 0){
        throw std::invalid_argument("data size cannot be empty");
    }
    this->_read_count = 0;
    this->_miss_read_count = 0;
    this->_write_count = 0;
    this->_miss_write_count = 0;
}

RWObject::~RWObject() {}

}