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
        // False indicating a read miss
        virtual bool read(char* dest, unsigned long address);
        virtual void write(char* src, unsigned long address);
        int _miss_read_count;
        int _read_count;
        int _miss_write_count;
        char* getData();
    private:
        char* _data;
};

class Cache: public RWObject {
    public:
        Cache(char* cache_data, CacheConfig* config, RWObject* lowerRW);
        void setConfig(CacheConfig* config);
        bool read(char* dest, unsigned long address);
        void write(char* src, unsigned long address);
    private:
        CacheConfig* _config;
        RWObject* _lower; // lower level of data object
};

class Memory: public RWObject {
    public:
        Memory(char* memory_data);
        bool read(char* dest, unsigned long address);
        void write(char* src, unsigned long address);
};
