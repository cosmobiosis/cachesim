#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "cache.h"

using namespace::std;

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

Cache::Cache(char* cacheData, CacheConfig* config, CacheLayer layer, void* lower_bank) {

    if (sizeof(cacheData) == 0){
        throw std::invalid_argument("data size cannot be empty");
    }
    this->_read_count = 0;
    this->_miss_read_count = 0;
    this->_write_count = 0;
    this->_miss_write_count = 0;
    this->_layer = layer;
    // Set lower-level
    if (layer < 2) {
        this->_lowerCache = (Cache *) (lower_bank);
    } else {
        this->_lowerMemory = (Memory *) (lower_bank);
    }
    setConfig(config);
}

/* set config */
void Cache::setConfig(CacheConfig* config) {
    this->_config = config;
    this->_metaData = (struct MetaRow*)malloc(this->_config->_numBlocks * sizeof(struct MetaRow));
}

// /*  Set lower level of cache for L1 or L2 cache */
// void Cache::setLowerCache(Cache* lower_bank) {
//     // Cache* this->_lower = static_cast<Cache*>(lower_bank);
//     this->_lower = static_cast<Cache*>(lower_bank);
// }

// /*  Set lower level of cache for L3 cache */
// void Cache::setLowerMemory(Memory* lower_bank) {
//     // Memory* this->_lower = static_cast<Memory*>(lower_bank);
//     // Memory* this->_lower = reinterpret_cast<Memory*>(lower_bank);
//     this->_lower = static_cast<Memory*>(lower_bank);
// }

// /* Set connection between cache and memory */
// void Cache::setMemory(Memory* memory_object) {
//     this->_lower = memory_object;
// }

Cache::~Cache() {}


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
        cacheBlock = getCacheBlock( tag, setIndex);;
    }
    // Continue with read
    // read blocksize - offset
    size_t inBlockOffset = this->parseBlockOffset(address);
    size_t readSize = blockSize - inBlockOffset;
    memcpy(dest, cacheBlock + inBlockOffset, readSize);
}


/*
    Invoked by Cache::read call 
*/
void Cache::syncBlock(const unsigned long& address) {
    size_t tag = this->parseTag(address);
    size_t setIndex = this->parseSetIndex(address);

    size_t setSize = this->_config->_setAssociativity;
    size_t blockIndex = setIndex * setSize;
    size_t blockSize = this->_config->_blockSize;

    size_t numBitBlockOffset = this->_config->_numBitBlockOffset;
    size_t blockHeadAddress = ((address >> numBitBlockOffset) << numBitBlockOffset);
    char* newBlock = (char*)malloc(blockSize * sizeof(char));
    if (this->_layer < 2) {
        this->_lowerCache->read(newBlock, blockSize, blockHeadAddress);
    } else {
        this->_lowerMemory->read(newBlock, blockSize, blockHeadAddress);
    }
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

/*
    1. If write hit
        Update L1
        Update L2
        Update L3
        Update Memory
    2. If write miss
        Update Memory
*/
void Cache::write(char* src, size_t srclen, const unsigned long &address) {
    size_t tag = this->parseTag(address);
    size_t setIndex = this->parseSetIndex(address);
    size_t blockSize = this->_config->_blockSize;

    _write_count += 1;
    char* cacheBlock = getCacheBlock(tag, setIndex);

    // Write miss
    if (cacheBlock == nullptr) {
        _miss_write_count += 1;
        // TODO: update the value in main memory
        return;
    }

    // Write hit
    size_t inBlockOffset = this->parseBlockOffset(address);
    size_t writeSize = blockSize - inBlockOffset;
    memcpy(cacheBlock + inBlockOffset, src, writeSize);
    // TODO: implement write buffer for writeBack
    if (this->_layer < 2) {
        this->_lowerCache->write(src, srclen, address);
    } else {
        this->_lowerMemory->write(src, srclen, address);
    }
    return;
}
