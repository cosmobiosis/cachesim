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

CacheFactory::CacheFactory(CacheConfig* config) {
    this->_config = config;
}

void CacheFactory::setConfig(CacheConfig* config) {
    this->_config = config;
}

Cache* CacheFactory::makeCache() {
    char* data = (char*)malloc(this->_config->_cacheCapacity * sizeof(char));
    std::cout << (this->_config == nullptr) << std::endl;
    Cache* cache = new Cache(data, this->_config, nullptr);
    return cache;
}

CacheConfig::CacheConfig(
        int memoryAddrLen,
        size_t cacheCapacity,
        size_t blockSize,
        size_t setAssociativity, 
        WritePolicy writePolicy, 
        ReplacementPolicy replacementPolicy
    ):
    _memoryAddrLen(memoryAddrLen),
    _cacheCapacity(cacheCapacity),
    _blockSize(blockSize),
    _setAssociativity(setAssociativity),
    _writePolicy(writePolicy),
    _replacementPolicy(replacementPolicy) {
    this->_numBlocks = int(cacheCapacity / blockSize);

    _numBitSetIndex = int(log2(this->_numBlocks / setAssociativity));
    _numBitBlockOffset = int(log2(blockSize));
    _numBitTag = max(0, _memoryAddrLen - _numBitSetIndex - _numBitBlockOffset);
}

CacheConfig::~CacheConfig() {}

Cache::Cache(char* cacheData, CacheConfig* config, RWObject* lowerRW) : _config(config), RWObject(cacheData) {
    this->_metaData = (struct MetaRow*)malloc(this->_config->_numBlocks * sizeof(struct MetaRow));
    this->_lower = lowerRW;
}

Cache::~Cache() {}

size_t Cache::parseTag(const unsigned long &address) {
    unsigned long tag = address >> (this->_config->_numBitSetIndex + this->_config->_numBitBlockOffset);
    return size_t(tag);
}

size_t Cache::parseSetIndex(const unsigned long &address) {
    unsigned long shifted = address >> (this->_config->_numBitBlockOffset);
    unsigned long setIndex = shifted & int(pow(2, this->_config->_numBitSetIndex) - 1);
    return size_t(setIndex);
}

size_t Cache::parseBlockInternalOffset(const unsigned long &address) {
    return address & int(pow(2, this->_config->_numBitBlockOffset) - 1);
}

char* RWObject::getData() {
    return this->_data;
}

char* Cache::getCacheBlock(size_t targetTag, size_t targetSetIndex) {
    size_t setSize = this->_config->_setAssociativity;
    size_t blockIndex = targetSetIndex * setSize;
    for (int i = blockIndex; i < blockIndex + setSize; i++) {
        if (this->_metaData[i].valid && this->_metaData[i].tag == targetTag) {
            char* cacheHead = this->getData();
            return cacheHead + i * this->_config->_blockSize;
        }
    }
    return nullptr;
}

void Cache::setLowerRWObject(RWObject* lowerRW) {
    this->_lower = lowerRW;
}

void Cache::syncBlock(const unsigned long& address) {
    size_t tag = this->parseTag(address);
    size_t setIndex = this->parseSetIndex(address);

    size_t setSize = this->_config->_setAssociativity;
    size_t blockIndex = setIndex * setSize;
    size_t blockSize = this->_config->_blockSize;

    size_t numBitBlockOffset = this->_config->_numBitBlockOffset;
    size_t blockHeadAddress = ((address >> numBitBlockOffset) << numBitBlockOffset);

    char* newBlock = (char*)malloc(blockSize * sizeof(char));
    this->_lower->read(newBlock, blockSize, blockHeadAddress);


    char* cacheHead = this->getData();

    for (int i = blockIndex; i < blockIndex + setSize; i++) {
        if (!this->_metaData[i].valid) {
            char* blockHead = cacheHead + i * blockSize;
            // update index in metadata
            this->_metaData[i].tag = tag;
            this->_metaData[i].valid = true;
            // update data in blocks
            memcpy(blockHead, newBlock, blockSize);
            return;
        }
    }
    // else do eviction here
    // current eviction policy: first one
    // TODO: more advanced eviction policy

    // update index in metadata
    this->_metaData[blockIndex].tag = tag;
    this->_metaData[blockIndex].valid = true;
    // update data in blocks
    char* blockHead = cacheHead + blockIndex * blockSize;
    memcpy(blockHead, newBlock, blockSize);
}

void Cache::read(char* dest, size_t destlen, const unsigned long &address) {
    size_t tag = this->parseTag(address);
    size_t setIndex = this->parseSetIndex(address);
    size_t blockSize = this->_config->_blockSize;

    _read_count += 1;
    char* cacheBlock = getCacheBlock(tag, setIndex);

    // Read Miss, bring the data to this level cache by querying the lower level
    if (cacheBlock == nullptr) {
        _miss_read_count += 1;
        // copy new block to this level
        syncBlock(address);
    }
    // Continue with read
    // read blocksize - offset
    size_t inBlockOffset = this->parseBlockInternalOffset(address);
    size_t readSize = blockSize - inBlockOffset;
    memcpy(dest, cacheBlock + inBlockOffset, readSize);
}

void Cache::write(char* src, size_t srclen, const unsigned long &address) {
    size_t tag = this->parseTag(address);
    size_t setIndex = this->parseSetIndex(address);
    size_t blockSize = this->_config->_blockSize;

    _write_count += 1;
    char* cacheBlock = getCacheBlock(tag, setIndex);

    // Write Miss
    if (cacheBlock == nullptr) {
        _miss_write_count += 1;
        syncBlock(address);
    }

    // Continue with write
    size_t inBlockOffset = this->parseBlockInternalOffset(address);
    size_t writeSize = blockSize - inBlockOffset;
    memcpy(cacheBlock + inBlockOffset, src, writeSize);

    // Write through
    // TODO: implement write buffer for writeBack
    this->_lower->write(src, srclen, address);
}

Memory::~Memory() {

}

Memory::Memory(char* memoryData): RWObject(memoryData) {

}

void Memory::read(char* dest, size_t destlen, const unsigned long &address) {
    char* data = this->getData();
    memcpy(dest, &(data[address]), destlen);
}

void Memory::write(char* src, size_t srclen, const unsigned long &address) {
    char* data = this->getData();
    memcpy(&(data[address]), src, srclen);
}