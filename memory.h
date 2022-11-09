#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace MemoryCacheSim {

class Memory: public RWObject {
    friend class Cache;
    public:
        Memory(char* memoryData);
        ~Memory();
        void read(char* dest, const unsigned long &address);
        void write(char* src, const unsigned long &address);
};

}