#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

struct MetaRow;
class CacheConfig;
class CacheFactory;
class RWObject;
class Cache;
class Memory;

enum WritePolicy { THROUGH, BACK };
enum ReplacementPolicy { DUMMY, LRU, FIFO };

class CacheConfig {
    public:
        CacheConfig(
            int memoryAddrLen,
            size_t cacheCapacity,
            size_t blockSize,
            size_t setAssociativity, 
            WritePolicy writePolicy, 
            ReplacementPolicy replacementPolicy
        );
        ~CacheConfig();
        int _memoryAddrLen;
        size_t _cacheCapacity;
        size_t _blockSize;
        size_t _setAssociativity;
        size_t _numBlocks;
        WritePolicy _writePolicy;
        ReplacementPolicy _replacementPolicy;

        // calculated values
        int _numBitTag;
        int _numBitSetIndex;
        int _numBitBlockOffset;
};

class RWObject {
    public:
        RWObject(char* data);
        virtual ~RWObject() = 0;
        // False indicating a read miss
        virtual void read(char* dest, size_t destlen, const unsigned long &address) = 0;
        virtual void write(char* src, size_t srclen, const unsigned long &address) = 0;
        int _read_count;
        int _miss_read_count;
        int _write_count;
        int _miss_write_count;
        char* getData();
    private:
        char* _data;
};

struct MetaRow {
    size_t tag;
    bool valid;
};

class CacheFactory {
    public:
        CacheFactory(CacheConfig* config);
        void setConfig(CacheConfig* config);
        Cache* makeCache();
    private:
        CacheConfig* _config;
};

class Cache: public RWObject {
    public:
        Cache(char* cacheData, CacheConfig* config, RWObject* lowerRW);
        ~Cache();
        // functions
        size_t parseTag(const unsigned long &address);
        size_t parseSetIndex(const unsigned long &address);
        size_t parseBlockInternalOffset(const unsigned long &address);

        bool isValid(char* targetSet);

        // read write maximum amounts: block size
        // read / write one block
        void read(char* dest, size_t destlen, const unsigned long &address);
        void write(char* src, size_t srclen, const unsigned long &address);

        void getCacheBlock(char** blockHead, size_t targetTag, size_t targetSetIndex);
        void syncBlock(const unsigned long& address);

        void setLowerRWObject(RWObject* lowerRW);
    private:
        CacheConfig* _config;
        struct MetaRow* _metaData;
        RWObject* _lower; // lower level of data object
};

class Memory: public RWObject {
    public:
        Memory(char* memoryData);
        ~Memory();
        void read(char* dest, size_t destlen, const unsigned long &address);
        void write(char* src, size_t srclen, const unsigned long &address);
};
