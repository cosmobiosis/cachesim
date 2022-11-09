#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "memory.h"

namespace MemoryCacheSim {

// Constructor goes here

Memory::~Memory() {}


Memory::Memory(char* memoryData): RWObject(memoryData) {
}


void Memory::read(char* dest, const unsigned long &address) {
    char* data = this->getData();
    memcpy(dest, &(data[address]), sizeof(dest));
}

void Memory::write(char* src, const unsigned long &address) {
    char* data = this->getData();
    memcpy(&(data[address]), src, sizeof(src));
}

}
