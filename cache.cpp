#include <cmath>
#include <iostream>
#include <string>
#include "cache.h"
#include <stdlib.h>

using namespace::std;

RWObject::RWObject(char* data): _data(data) {}

CacheConfig::CacheConfig(
            int blockSize, 
            int setAssociativity, 
            WritePolicy writePolicy, 
            ReplacementPolicy replacementPolicy
        ):  
            _blockSize(blockSize),
            _setAssociativity(setAssociativity),
            _writePolicy(writePolicy),
            _replacementPolicy(replacementPolicy) {}

Cache::Cache(char* data, CacheConfig* config, RWObject* lowerRW) : RWObject(data) {
    this->_config = config;
    this->_lower = lowerRW;
}

void Cache::setConfig(CacheConfig* config) {
    this->_config = config;
}