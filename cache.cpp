#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "cache.h"

#include "memory.h"

using namespace::std;

// namespace MemoryCacheSim
//{

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
    _replacementPolicy(replacementPolicy) {
    size_t cacheSize = sizeof(this->_cacheSize);
    this->_numBlocks = cacheSize / blockSize;

    _numBitSetIndex = int(log2(this->_numBlocks / setAssociativity));
    _numBitBlockOffset = int(log2(blockSize));
    _numBitTag = max(0, _memoryAddrLen - _numBitSetIndex - _numBitBlockOffset);
}

CacheConfig::~CacheConfig() {}



Cache::Cache(char* cacheData, CacheConfig* config) {

    if (sizeof(cacheData) == 0){
        throw std::invalid_argument("data size cannot be empty");
    }
    this->_read_count = 0;
    this->_miss_read_count = 0;
    this->_write_count = 0;
    this->_miss_write_count = 0;

    // Set config
    setConfig(config);
    // this->_lower = lowerCache;
}

void Cache::setConfig(CacheConfig* config) {
    this->_config = config;
    this->_metaData = (struct MetaRow*)malloc(this->_config->_numBlocks * sizeof(struct MetaRow));
}

Cache::~Cache() {}



//--------------------------------------------------
// Implementation of the cache replacement mechanism
//--------------------------------------------------
// void Cache::LRUCacheReplacement() {
// }

// void Cache::FIFOCacheReplacement() {
// }

// void Cache::ClockCacheReplacement() {
// }

//--------------------------------------------------
// Implementation of cache miss handling
//--------------------------------------------------
// void Cache::handleReadMiss() {
//      _miss_read_count += 1;
//     // this->_lower->read(dest, address);
// }

// void Cache::handleWriteMiss() {}

// //--------------------------------------------------
// // Implementation of cache mishits handling
// //--------------------------------------------------
// void Cache::handleReadHit() {
//     // Read Hit
//     // read blocksize - offset
//     size_t cacheOffset = this->parseBlockOffset(address);
//     size_t readSize = this->_config->_blockSize - cacheOffset;
//     memcpy(dest, cacheBlock + cacheOffset, readSize);
// }

// void Cache::handleWriteHit() {}


//--------------------------------------------------
// Implementation of parsing
//--------------------------------------------------
char* Cache::getData() {
    return this->_data;
}

size_t Cache::parseTag(const unsigned long &address) {
    unsigned long tag = address >> (this->_config->_numBitSetIndex + this->_config->_numBitBlockOffset);
    return size_t(tag);
}

size_t Cache::parseSetIndex(const unsigned long &address) {
    unsigned long shifted = address >> (this->_config->_numBitBlockOffset);
    unsigned long setIndex = shifted & int(pow(2, this->_config->_numBitSetIndex) - 1);
    return size_t(setIndex);
}

size_t Cache::parseBlockOffset(const unsigned long &address) {
    return address & int(pow(2, this->_config->_numBitBlockOffset) - 1);
}


char* Cache::getCacheBlock(size_t targetTag, size_t targetSetIndex) {
    size_t setSize = this->_config->_setAssociativity;
    size_t setOffset = targetSetIndex * setSize;
    for (int i = setOffset; i < setOffset + setSize; i++) {
        // Is it correct to compare the tag directly? 
        if (this->_metaData[i].valid && this->_metaData[i].tag == targetTag) {
            char* cacheHead = this->getData();
            return cacheHead + i * this->_config->_blockSize;
        }
    }
    return nullptr;
}


//--------------------------------------------------
// Implementation of cache read and write
//--------------------------------------------------
void Cache::read(char* dest, const unsigned long &memoryAddress) {

    size_t tag = parseTag(memoryAddress);
    size_t setIndex = parseSetIndex(memoryAddress);

    char* cacheBlock = getCacheBlock(tag, setIndex);
    _read_count += 1;

    if (cacheBlock == nullptr) { // Read Miss
        // handleReadMiss();
        return;
    }
    // handleReadHit(dest);
    return;
}


void Cache::write(char* src, const unsigned long &address) {

}

// }
