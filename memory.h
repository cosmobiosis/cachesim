#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

// namespace MemoryCacheSim {

class Memory {
    friend class Cache;
    public:
        Memory(char* memoryData);
        ~Memory();
        char* _data;
        char* getData();
        void read(char* dest, const unsigned long &address);
        void write(char* src, const unsigned long &address);
};

// }
