#include <string>
#include <map>
#include <iostream>
#include <sstream>

enum WritePolicy { Through, Back };
enum ReplacementPolicy { LRU, FIFO };

class CacheConfig {
    public:
        CacheConfig(
            int blockSize, 
            int setAssociativity, 
            WritePolicy writePolicy, 
            ReplacementPolicy replacementPolicy
        );
        int _blockSize;
        int _setAssociativity;
        WritePolicy _writePolicy;
        ReplacementPolicy _replacementPolicy;
};

class RWObject {
    public:
        RWObject(char* data);
        bool read(char* dest, unsigned long address);
        void write(char* src, unsigned long address);
        int _miss_read_count;
        int _read_count;
        int _miss_write_count;
        char* getData();
    private:
        char* _data;
};

class Cache: public RWObject {
    public:
        Cache(char* data, CacheConfig* config, RWObject* lowerRW);
        void setConfig(CacheConfig* config);
        void setLowerRW(RWObject* _lower);
        // False indicating a read miss
        bool read(char* dest, unsigned long address);
        void write(char* src, unsigned long address);
    private:
        CacheConfig* _config;
        RWObject* _lower; // lower level of memory
};