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

size_t CacheConfig::parseBlockInternalOffset(const unsigned long &address) {
    return address & int(pow(2, this->_numBitBlockOffset) - 1);
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

void Cache::putNewBlock(size_t tag, size_t setIndex, char* newBlock) {
    size_t setSize = this->_config->_setAssociativity;
    size_t blockIndex = setIndex * setSize;
    size_t blockSize = this->_config->_blockSize;
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
    size_t tag = this->_config->parseTag(address);
    size_t setIndex = this->_config->parseSetIndex(address);
    size_t blockSize = this->_config->_blockSize;
    size_t numBitBlockOffset = this->_config->_numBitBlockOffset;

    char* cacheBlock = getCacheBlock(tag, setIndex);
    _read_count += 1;

    // Read Miss, bring the data to this level cache by querying the lower level
    if (cacheBlock == nullptr) {
        _miss_read_count += 1;
        // copy new block to this level
        char* newBlock = (char*)malloc(blockSize * sizeof(char));
        size_t blockHeadAddress = ((address >> numBitBlockOffset) << numBitBlockOffset);
        this->_lower->read(newBlock, blockSize, blockHeadAddress);
        putNewBlock(tag, setIndex, newBlock);
    }
    // Continue with read
    // read blocksize - offset
    size_t inBlockOffset = this->_config->parseBlockInternalOffset(address);
    size_t readSize = this->_config->_blockSize - inBlockOffset;
    memcpy(dest, cacheBlock + inBlockOffset, readSize);
}

void Cache::write(char* src, size_t srclen, const unsigned long &address) {
    size_t tag = this->_config->parseTag(address);
    size_t setIndex = this->_config->parseSetIndex(address);
    size_t blockSize = this->_config->_blockSize;
    size_t numBitBlockOffset = this->_config->_numBitBlockOffset;

    _write_count += 1;
    char* cacheBlock = getCacheBlock(tag, setIndex);
    // Write Miss
    if (cacheBlock == nullptr) {
        _miss_write_count += 1;
        // put empty block to evict old block
        char* newBlock = (char*)malloc(blockSize * sizeof(char));
        putNewBlock(tag, setIndex, newBlock);
    }

    // Continue with write
    size_t inBlockOffset = this->_config->parseBlockInternalOffset(address);
    size_t writeSize = this->_config->_blockSize - inBlockOffset;
    memcpy(cacheBlock + inBlockOffset, src, writeSize);

    // Write through
    // TODO: implement write buffer
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