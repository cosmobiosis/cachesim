#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Import memory file
#include "memory.h"

enum WritePolicy { Through, Back };

enum ReplacementPolicy { 
    LRU, 
    FIFO, 
    Clock
};

enum CacheLayer { 
    LayerOne, 
    LayerTwo, 
    LayerThree
};

struct MetaRow {
    size_t tag;
    bool valid;
};

class CacheConfig {
    public:
        CacheConfig(
            int memoryAddrLen,
            int blockSize,
            int setAssociativity, 
            WritePolicy writePolicy, 
            ReplacementPolicy replacementPolicy
        );
        ~CacheConfig();
        int _cacheSize;
        int _memoryAddrLen;
        int _blockSize;
        int _setAssociativity;
        int _numBlocks;
        WritePolicy _writePolicy;
        ReplacementPolicy _replacementPolicy;

        // calculated values
        int _numBitTag;
        int _numBitSetIndex;
        int _numBitBlockOffset;
};

class Cache {
    public:
        Cache(char* cacheData, CacheConfig* config, CacheLayer layer, void* lowerObject);
        ~Cache();
        void setConfig(CacheConfig* config);
        bool isValid(char* targetSet);

        int _read_count;
        int _miss_read_count;
        int _write_count;
        int _miss_write_count;
        CacheLayer _layer;

        char* getData();
        size_t parseTag(const unsigned long &address);
        size_t parseSetIndex(const unsigned long &address);
        size_t parseBlockOffset(const unsigned long &address);
        char* getCacheBlock(size_t targetTag, size_t targetSetIndex);
        void syncBlock(const unsigned long& address);
        // void handleReadHit(char* dest, char* cacheBlock, const unsigned long &address);

        // read maximum amounts: block size
        void read(char* dest, size_t destlen, const unsigned long &address);
        void write(char* src, size_t srclen, const unsigned long &address);
        // void setLowerCache(Cache* lower_bank);
        // void setLowerMemory(Memory* lower_bank);
        // void setLowerBank(void* lower_bank);
        // void setLowerCache(Cache* cache_object);
        // void setMemory(Memory* memory_object);

    private:
        CacheConfig* _config;
        struct MetaRow* _metaData;
        Memory* _lowerMemory;
        Cache* _lowerCache;
        char* _data;
};
