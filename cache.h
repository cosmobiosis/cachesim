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
            char* cache,
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
        WritePolicy _writePolicy;
        ReplacementPolicy _replacementPolicy;

        // calculated values
        int numBitsTotal;
        int numBitIndex;
        int numBitByteOffset;

        // functions
        size_t parseTag(const unsigned long &address);
        size_t parseSetIndex(const unsigned long &address);
        size_t parseCacheOffset(const unsigned long &address);
};

class RWObject {
    public:
        RWObject(char* data);
        ~RWObject();
        // False indicating a read miss
        virtual bool read(char* dest, const unsigned long &address) = 0;
        virtual void write(char* src, const unsigned long &address) = 0;
        int _read_count;
        int _miss_read_count;
        int _write_count;
        int _miss_write_count;
        char* getData();
    private:
        char* _data;
};

class Cache: public RWObject {
    public:
        Cache(char* cache_data, CacheConfig* config, RWObject* lowerRW);
        ~Cache();
        void setConfig(CacheConfig* config);
        bool read(char* dest, const unsigned long &address);
        void write(char* src, const unsigned long &address);
    private:
        CacheConfig* _config;
        RWObject* _lower; // lower level of data object
};

class Memory: public RWObject {
    public:
        Memory(char* memory_data);
        ~Memory();
        bool read(char* dest, const unsigned long &address);
        void write(char* src, const unsigned long &address);
};
