#include <iostream>
#include <string>
#include "cache.h"

int main(int argc, char* argv[])
{
  std::cout << "Factory init..." << std::endl;
  CacheFactory* factory = new CacheFactory(nullptr);

  std::cout << "Config init..." << std::endl;
  CacheConfig* l1_config = new CacheConfig(
    6, 16, 4, 2, WritePolicy::THROUGH, ReplacementPolicy::DUMMY
  );
  std::cout << "Setting config..." << std::endl;
  factory->setConfig(l1_config);

  std::cout << "Making cache..." << std::endl;
  Cache* l1_cache = factory->makeCache();

  std::cout << "Mem data init..." << std::endl;
  char* mem_data = (char*)malloc(258 * sizeof(char));
  Memory* memory = new Memory(mem_data);

  std::cout << "Setting mem data..." << std::endl;
  l1_cache->setLowerRWObject(memory);

  std::cout << "Mallocing src..." << std::endl;
  // 110100 -> 52
  char* src = (char*)malloc(4 * sizeof(char));
  src[0] = 'a';
  src[1] = 'b';
  src[2] = 'c';
  src[3] = '\0';

  std::cout << "Writing src..." << std::endl;
  l1_cache->write(src, 4, 52);

  char* dest = (char*)malloc(4 * sizeof(char));
  l1_cache->read(dest, 4, 52);

  printf("%s\n", dest);
  return 0;
}
