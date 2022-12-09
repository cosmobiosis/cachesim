#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

enum WritePolicy { Through, Back };
enum ReplacementPolicy { LRU, FIFO };

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

        // functions
        size_t parseTag(const unsigned long &address);
        size_t parseSetIndex(const unsigned long &address);
        size_t parseBlockOffset(const unsigned long &address);
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

class Cache: public RWObject {
    public:
        Cache(char* cacheData, CacheConfig* config, RWObject* lowerRW);
        ~Cache();
        void setConfig(CacheConfig* config);
        bool isValid(char* targetSet);

        // read maximum amounts: block size
        void read(char* dest, size_t destlen, const unsigned long &address);
        void write(char* src, size_t srclen, const unsigned long &address);

        char* getCacheBlock(size_t targetTag, size_t targetSetIndex);
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

// class CacheClient : public  {

// }