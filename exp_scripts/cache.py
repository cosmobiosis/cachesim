import math
import random
import matplotlib.pyplot as plt

class Cache:
    def __init__(self, num_set, set_associavity, blockSize, child=None):
        """ 
        num_set:    Number of sets in the cache
        set_associavity:    Set associavity
        blockSize:  Cache Block size

        <Benchmark>
        Cache Repalcement Policy: Random 
        """
        self.num_set = num_set
        self.set_associavity = set_associavity
        self.blockSize = blockSize

        #### Read ####
        self.read = 0
        self.read_hit = 0
        self.read_miss = 0
        self.mem_read = 0

        #### Write #### 
        self.write = 0
        self.write_hit = 0
        self.write_miss = 0
        self.mem_write = 0

        #### data structure ####
        self.child = child
        self.cache = [[-1 for _ in range(set_associavity)] for _ in range(num_set)]
        self.validBitTable = [[1 for _ in range(set_associavity)] for _ in range(num_set)]
        self.cacheBlockInit() # CacheBlock: 3D Array

    def cacheBlockInit(self):
        """ Initialize the cache block data structure. 3-D array""" 
        self.cacheBlockTable = []
        for _ in range(self.num_set):
            curSet = []
            for _ in range(self.set_associavity):
                emptyBlock = [0] * self.blockSize
                curSet.append(emptyBlock)
            self.cacheBlockTable.append(curSet)

    def readr(self, set_index, tag):
        """
        CPU issues a read request, the input is a physical memory address. 
        The address contains set_index and tag.
        """
        self.read += 1
        setBlock = self.cache[set_index]
        for block_iter in range(self.set_associavity):
            # Need to meet two conditions
            # 1. Valid bit is 1
            # 2. Tag bit matches
            if self.validBitTable[set_index][block_iter] == 1 and setBlock[block_iter] == tag:
                self.read_hit += 1
                return
        self.read_miss += 1
        if self.child is None:
            self.mem_read += 1
        else:
            self.child.readr(set_index, tag)
        ### Perform Random Eviction ### 
        rand_block_idx = random.choice(list(range(self.set_associavity)))
        setBlock[rand_block_idx] = tag

    def writer(self, set_index, tag):
        self.write += 1
        setBlock = self.cache[set_index]
        find_flag = False
        for i in range(len(setBlock)):
            if setBlock[i] == tag:
                self.write_hit += 1
                find_flag = True
        if not find_flag:
            self.write_miss += 1
            self.readr(set_index, tag)

def int_to_binary(byte_address_int, num_tag_bit, num_index_bit):
    """ 
    Extract 
    1) tag bit
    2) index bit
    3) byte offset bit
    """ 
    binary_string = format(byte_address_int, '#020b')[2:]
    tag_bit = binary_string[:num_tag_bit]
    index_bit = binary_string[num_tag_bit: num_tag_bit+num_index_bit]
    byte_offset_bit = binary_string[num_tag_bit+num_index_bit:]
    return (tag_bit, int(index_bit, 2), byte_offset_bit)
