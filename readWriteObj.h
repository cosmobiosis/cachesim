#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>

using namespace::std;

namespace MemoryCacheSim
{

class RWObject {
    public:
        RWObject(char* data);
        virtual ~RWObject() = 0;
        // False indicating a read miss
        virtual void read(char* dest, const unsigned long &address) = 0;
        virtual void write(char* src, const unsigned long &address) = 0;
        int _read_count;
        int _miss_read_count;
        int _write_count;
        int _miss_write_count;
        char* getData();
    private:
        char* _data;
};

}
