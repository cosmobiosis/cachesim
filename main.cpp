#include <iostream>
#include <string>
#include "cache.h"

int main(int argc, char* argv[])
{
  CacheFactory* factory = new CacheFactory(nullptr);

  CacheConfig* l1_config = new CacheConfig(
    6, 16, 4, 2, WritePolicy::THROUGH, ReplacementPolicy::DUMMY
  );
  factory->setConfig(l1_config);

  Cache* l1_cache = factory->makeCache();

  char* mem_data = (char*)malloc(258 * sizeof(char));
  Memory* memory = new Memory(mem_data);

  l1_cache->setLowerRWObject(memory);

  // 110100 -> 52
  char* src = (char*)malloc(4 * sizeof(char));
  src[0] = 'a';
  src[1] = 'b';
  src[2] = 'c';
  src[3] = '\0';

  l1_cache->write(src, 4, 52);

  char* dest = (char*)malloc(4 * sizeof(char));
  l1_cache->read(dest, 4, 52);

  printf("Succesfully read written data: %s\n", dest);

  // 000100 -> 4
  char* overwrite = (char*)malloc(4 * sizeof(char));
  overwrite[0] = 'x';
  overwrite[1] = 'y';
  overwrite[2] = 'z';
  overwrite[3] = '\0';

  l1_cache->write(overwrite, 4, 4);
  // 010100 -> 20
  l1_cache->write(overwrite, 4, 20);

  l1_cache->read(dest, 4, 4);
  printf("Succesfully read written data: %s\n", dest);

  std::cout << l1_cache->_read_count << std::endl;
  std::cout << l1_cache->_miss_read_count << std::endl;

  return 0;
}
