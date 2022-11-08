#include <cmath>
#include <iostream>
#include <string>
#include "cache.h"
#include <stdlib.h>
#include <math.h>

using namespace::std;

RWObject::RWObject(char* data): _data(data) {
    this->_read_count = 0;
    this->_miss_read_count = 0;
    this->_write_count = 0;
    this->_miss_write_count = 0;
}

CacheConfig::CacheConfig(
            int memoryAddrLen,
            int blockSize, 
            int setAssociativity, 
            WritePolicy writePolicy, 
            ReplacementPolicy replacementPolicy
        ):
            _memoryAddrLen(memoryAddrLen),
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
    size_t cacheSize = sizeof(this->getData());
    size_t blockSize = this->_config->_blockSize;
    size_t numBlocks = cacheSize / blockSize;
    size_t setAssociativity = this->_config->_setAssociativity;

    size_t numBitsTotal = this->_config->_memoryAddrLen;
    size_t numBitIndex = int(log2(numBlocks / setAssociativity));
    size_t numBitByteOffset = int(log2(blockSize));
    size_t numBitTag = numBitsTotal - numBitIndex - numBitByteOffset;

    return true;
}

int extract(int , int skip, int) {
    // 
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