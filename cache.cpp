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

RWObject::~RWObject() {}

CacheConfig::CacheConfig(
        char* cache,
        int memoryAddrLen,
        int blockSize, 
        int setAssociativity, 
        WritePolicy writePolicy, 
        ReplacementPolicy replacementPolicy
    ):
    _cacheSize(sizeof(cache)),
    _memoryAddrLen(memoryAddrLen),
    _blockSize(blockSize),
    _setAssociativity(setAssociativity),
    _writePolicy(writePolicy),
    _replacementPolicy(replacementPolicy) {
    if (sizeof(cache) == 0){
        throw std::invalid_argument("cache cannot be empty");
    }
    size_t cacheSize = sizeof(this->_cacheSize);
    size_t numBlocks = cacheSize / blockSize;

    size_t numBitsTotal = this->_memoryAddrLen;
    size_t numBitIndex = int(log2(numBlocks / setAssociativity));
    size_t numBitByteOffset = int(log2(blockSize));

    size_t numBitTag = 0;
    if (numBitsTotal > numBitIndex + numBitByteOffset) {
        numBitTag = numBitsTotal - numBitIndex - numBitByteOffset;
    }
}

CacheConfig::~CacheConfig() {}

Cache::Cache(char* cache_data, CacheConfig* config, RWObject* lowerRW) : RWObject(cache_data) {
    this->_config = config;
    this->_lower = lowerRW;
}

Cache::~Cache() {}

size_t CacheConfig::parseTag(const unsigned long &address) {
    // unsigned long shiftedAddr = address >> this->_blockSize;
    return 0;
}

size_t CacheConfig::parseSetIndex(const unsigned long &address) {
    
    return 0;
}

size_t CacheConfig::parseCacheOffset(const unsigned long &address) {
    
    return 0;
}

char* RWObject::getData() {
    return this->_data;
}

void Cache::setConfig(CacheConfig* config) {
    this->_config = config;
}

bool Cache::read(char* dest, const unsigned long &address) {


    size_t tag = this->_config->parseTag(address);
    size_t setIndex = this->_config->parseSetIndex(address);
    size_t cacheOffset = this->_config->parseCacheOffset(address);

    

    return true;
}

void Cache::write(char* src, const unsigned long &address) {
    return;
}

Memory::~Memory() {}

Memory::Memory(char* memory_data): RWObject(memory_data) {
}

bool Memory::read(char* dest, const unsigned long &address) {
    return true;
}

void Memory::write(char* src, const unsigned long &address) {
    return;
}