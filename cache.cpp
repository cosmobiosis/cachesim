#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "cache.h"

using namespace::std;

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

Cache::Cache(char* cacheData, CacheConfig* config, RWObject* lowerRW) : RWObject(cacheData) {
    setConfig(config);
    this->_lower = lowerRW;
}

void Cache::setConfig(CacheConfig* config) {
    this->_config = config;
    this->_metaData = (struct MetaRow*)malloc(this->_config->_numBlocks * sizeof(struct MetaRow));
}

Cache::~Cache() {}

size_t CacheConfig::parseTag(const unsigned long &address) {
    unsigned long tag = address >> (this->_numBitSetIndex + this->_numBitBlockOffset);
    return size_t(tag);
}

size_t CacheConfig::parseSetIndex(const unsigned long &address) {
    unsigned long shifted = address >> (this->_numBitBlockOffset);
    unsigned long setIndex = shifted & int(pow(2, this->_numBitSetIndex) - 1);
    return size_t(setIndex);
}

size_t CacheConfig::parseBlockOffset(const unsigned long &address) {
    return address & int(pow(2, this->_numBitBlockOffset) - 1);
}

char* RWObject::getData() {
    return this->_data;
}

char* Cache::getCacheBlock(size_t targetTag, size_t targetSetIndex) {
    size_t setSize = this->_config->_setAssociativity;
    size_t setOffset = targetSetIndex * setSize;
    for (int i = setOffset; i < setOffset + setSize; i++) {
        if (this->_metaData[i].valid && this->_metaData[i].tag == targetTag) {
            char* cacheHead = this->getData();
            return cacheHead + i * this->_config->_blockSize;
        }
    }
    return nullptr;
}

void Cache::read(char* dest, const unsigned long &address) {
    size_t tag = this->_config->parseTag(address);
    size_t setIndex = this->_config->parseSetIndex(address);

    char* cacheBlock = getCacheBlock(tag, setIndex);
    _read_count += 1;

    // Read Miss
    if (cacheBlock == nullptr) {
        _miss_read_count += 1;
        this->_lower->read(dest, address);
        return;
    }
    // Read Hit
    // read blocksize - offset
    size_t cacheOffset = this->_config->parseBlockOffset(address);
    size_t readSize = this->_config->_blockSize - cacheOffset;
    memcpy(dest, cacheBlock + cacheOffset, readSize);
}

void Cache::write(char* src, const unsigned long &address) {
    
}

Memory::~Memory() {}

Memory::Memory(char* memoryData): RWObject(memoryData) {

}

void Memory::read(char* dest, const unsigned long &address) {
    char* data = this->getData();
    memcpy(dest, &(data[address]), sizeof(dest));
}

void Memory::write(char* src, const unsigned long &address) {
    char* data = this->getData();
    memcpy(&(data[address]), src, sizeof(src));
}