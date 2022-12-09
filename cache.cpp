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

Cache::Cache(char* cache_data, CacheConfig* config, RWObject* lowerRW) : RWObject(cache_data) {
    this->_config = config;
    this->_lower = lowerRW;
}

char* RWObject::getData() {
    return this->_data;
}

void Cache::setConfig(CacheConfig* config) {
    this->_config = config;
}

bool Cache::read(char* dest, unsigned long address) {
    return true;
}

void Cache::write(char* src, unsigned long address) {
    return;
}

Memory::Memory(char* memory_data): RWObject(memory_data) {
}

bool Memory::read(char* dest, unsigned long address) {
    return true;
}

void Memory::write(char* src, unsigned long address) {
    return;
}