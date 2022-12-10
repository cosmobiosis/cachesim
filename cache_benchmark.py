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


# Plotting params 
colorEnums = ["#746ab0", "#ffce30", "#e83845", "#e389b9", "#800080", "#FF7F50", "#800080", "#FF7F50", "#800080", "#FF7F50"]
colorInd = 0

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


##############################
#  Memory and Cache Parameters
##############################
MEMORY_SIZE = 256 * 1024  # 256KB
CACHE_SIZE = 8 * 1024     # 8KB
CACHE_BLOCK_SIZE = 8      # 8B

num_cache_block = CACHE_SIZE / CACHE_BLOCK_SIZE
num_physical_address_bits = int(math.log2(MEMORY_SIZE))
num_byteoffset_bits = int(math.log2(CACHE_BLOCK_SIZE))
set_associavity_array = [1, 2, 4, 8]

x_set = []
y2_miss = []
for set_associavity in set_associavity_array:
    x_set.append(set_associavity)
    num_of_set = int(num_cache_block / set_associavity)
    num_index_bits = int(math.log2(num_of_set))
    num_tag_bit = num_physical_address_bits - num_index_bits - num_byteoffset_bits

    ### Cache object creation ### 
    c1 = Cache(num_of_set, set_associavity, CACHE_BLOCK_SIZE)
    c2 = Cache(2 * num_of_set, 2 * set_associavity, CACHE_BLOCK_SIZE)
    c1.child = c2

    # Address traces are randomly generated 
    for k in range(10000):
        # Sample a memory byte address
        byte_address = random.choice(list(range(MEMORY_SIZE)))
        (tag_bit, set_index_bit, _) = int_to_binary(byte_address, num_tag_bit, num_index_bits)
        c1.writer(set_index_bit, tag_bit)

    y2_miss.append(c2.read_miss)

    plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(num_tag_bit))
    colorInd += 1
    config_string = "Physical address bits: {}, index bits: {}, tab bits: {}, {}-way set associative".format(num_physical_address_bits, 
    num_index_bits, num_tag_bit, set_associavity)
    print(config_string)

title_string = "Read miss (Random)".format(CACHE_SIZE//1024)
plt.title(title_string)
plt.xlabel("Set Associativity")
plt.ylabel("Counts of Read Miss")
plt.savefig("visual/benchmark_random_{}.png".format(10000))


# ##############################
# #  Memory and Cache Parameters
# ##############################
# MEMORY_SIZE = 256 * 1024  # 256KB
# CACHE_SIZE = 8 * 1024     # 8KB
# CACHE_BLOCK_SIZE = 8      # 8B

# num_cache_block = CACHE_SIZE / CACHE_BLOCK_SIZE
# num_physical_address_bits = int(math.log2(MEMORY_SIZE))
# num_byteoffset_bits = int(math.log2(CACHE_BLOCK_SIZE))
# set_associavity_array = [1, 2, 4, 8]

# x_set = []
# y2_miss = []
# for set_associavity in set_associavity_array:
#     x_set.append(set_associavity)
#     num_of_set = int(num_cache_block / set_associavity)
#     num_index_bits = int(math.log2(num_of_set))
#     num_tag_bit = num_physical_address_bits - num_index_bits - num_byteoffset_bits

#     ### Cache object creation ### 
#     c1 = Cache(num_of_set, set_associavity, CACHE_BLOCK_SIZE)
#     c2 = Cache(2 * num_of_set, 2 * set_associavity, CACHE_BLOCK_SIZE)
#     c1.child = c2

#     # Address traces are linearly generated
#     for k in range(10000): 
#         (tag_bit, set_index_bit, _) = int_to_binary(k, num_tag_bit, num_index_bits)
#         c1.writer(set_index_bit, tag_bit)
#     y2_miss.append(c2.read_miss)

#     plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(num_tag_bit))
#     colorInd += 1
#     config_string = "Physical address bits: {}, index bits: {}, tab bits: {}, {}-way set associative".format(num_physical_address_bits, 
#     num_index_bits, num_tag_bit, set_associavity)
#     print(config_string)

# title_string = "Read miss versus set associativity".format(CACHE_SIZE//1024)
# plt.title(title_string)
# plt.xlabel("Set Associativity")
# plt.ylabel("Counts of Read Miss")
# plt.savefig("visual/benchmark_linear_{}.png".format(10000))
