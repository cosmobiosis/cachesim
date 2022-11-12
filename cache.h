#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

// namespace MemoryCacheSim
// {

enum WritePolicy { Through, Back };
enum ReplacementPolicy { 
    LRU, 
    FIFO, 
    Clock
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
        Cache(char* cacheData, CacheConfig* config);
        ~Cache();
        void setConfig(CacheConfig* config);
        bool isValid(char* targetSet);

        int _read_count;
        int _miss_read_count;
        int _write_count;
        int _miss_write_count;

        char* getData();
        size_t parseTag(const unsigned long &address);
        size_t parseSetIndex(const unsigned long &address);
        size_t parseBlockOffset(const unsigned long &address);
        char* getCacheBlock(size_t targetTag, size_t targetSetIndex);

        // read maximum amounts: block size
        void read(char* dest, const unsigned long &address);
        void write(char* src, const unsigned long &address);

    private:
        CacheConfig* _config;
        struct MetaRow* _metaData;
        Cache* _lower; // lower level of cache object
        Cache* _upper; // upper level of cache object
        char* _data;
};

// }
