import random
import matplotlib.pyplot as plt
import math

class Cache:
    def __init__(self, num_set, set_associavity, blockSize=1, child=None):
        """ 
        num_set:    Number of sets in the cache
        set_associavity:    Set associavity
        blockSize:  Cache Block size

        <LRU-based Replacement>
        Cache Repalcement Policy: LRU 
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
        self.cacheBlockInit()

        # Data Structure for LRU Cache
        self.ageBitTable = [[1 for _ in range(set_associavity)] for _ in range(num_set)]

    def cacheBlockInit(self):
        self.cacheBlockTable = []
        for _ in range(self.num_set):
            curSet = []
            for _ in range(self.set_associavity):
                emptyBlock = [0] * self.blockSize
                curSet.append(emptyBlock)
            self.cacheBlockTable.append(curSet)

    def lru_eviction(self):
        max_age = 0 
        idx_to_evict = None
        for set_idx in range(self.num_set):
            for block_idx in range(self.set_associavity):
                if max_age < self.ageBitTable[set_idx][block_idx]:
                    max_age = self.ageBitTable[set_idx][block_idx]
                    idx_to_evict = (set_idx, block_idx)
        return idx_to_evict

    def lru_lazy_cleanup(self):
        """ Avoid the lifetime becomes too large """ 
        pass

    def lru_increment_lifetime(self, cur_set_idx, cur_block_idx):
        """ 
        Increment the lifetime of all other cache lines except for the one
        at (cur_set_idx, cur_block_idx)
        """
        for set_idx in range(self.num_set):
            for block_idx in range(self.set_associavity):
                if cur_block_idx == set_idx and cur_block_idx == block_idx:
                    continue
                self.ageBitTable[set_idx][block_idx] += 1

    def findEmptyCacheLine(self):
        for set_idx in range(self.num_set):
            for block_idx in range(self.set_associavity):
                if self.cache[set_idx][block_idx] == -1:
                    return (set_idx, block_idx)
        return None

    def readr(self, set_index, tag):
        self.read += 1
        setBlock = self.cache[set_index]
        for block_iter in range(self.set_associavity):
            if self.validBitTable[set_index][block_iter] == 1 and setBlock[block_iter] == tag:
                self.read_hit += 1
                self.lru_increment_lifetime(set_index, block_iter)
                return
        ### Handle Read Miss ### 
        self.read_miss += 1
        if self.child is None:
            self.mem_read += 1
        else:
            self.child.readr(set_index, tag)
        ### Perform LRU Eviction ### 
        tup = self.findEmptyCacheLine()
        if not tup:
            tup = self.lru_eviction()
        self.cache[tup[0]][tup[1]] = tag
        self.ageBitTable[tup[0]][tup[1]] = 1
        # Increment the lifetime of all other cache lines 
        self.lru_increment_lifetime(tup[0], tup[1])

    def writer(self, set_index, tag):
        self.write += 1
        setBlock = self.cache[set_index]
        for block_iter in range(self.set_associavity):
            if self.validBitTable[set_index][block_iter] == 1 and setBlock[block_iter] == tag:
                self.write_hit += 1
                self.lru_increment_lifetime(set_index, block_iter)
                return
        ### Handle Write Miss ###
        self.write_miss += 1
        self.readr(set_index, tag)

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


############################
#   Configurable parameters 
############################
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
    c1 = Cache(num_of_set, set_associavity, CACHE_BLOCK_SIZE)
    c2 = Cache(2 * num_of_set, 2 * set_associavity, CACHE_BLOCK_SIZE)
    c1.child = c2

    # Address traces are randomly generated 
    for k in range(10000): 
        byte_address = random.choice(list(range(MEMORY_SIZE)))
        (tag_bit, set_index_bit, _) = int_to_binary(byte_address, num_tag_bit, num_index_bits)
        c1.writer(set_index_bit, tag_bit)

    y2_miss.append(c2.read_miss)
    plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(num_tag_bit))
    colorInd += 1
    config_string = "physical address bits: {}, index bits: {}, tab bits: {}, {}-way set associative".format(num_physical_address_bits, 
    num_index_bits, num_tag_bit, set_associavity)
    print(config_string)

title_string = "Read miss (LRU-cache)".format(CACHE_SIZE//1024)
plt.title(title_string)
plt.xlabel("Set Associativity")
plt.ylabel("Counts of Read Miss")
plt.savefig("visual/lru_random_{}.png".format(10000))


# ############################
# #   Configurable parameters 
# ############################
# MEMORY_SIZE = 256 * 1024  # 256KB
# CACHE_SIZE = 8 * 1024     # 8KB
# CACHE_BLOCK_SIZE = 8      # 8B

# num_cache_block = CACHE_SIZE / CACHE_BLOCK_SIZE
# num_physical_address_bits = int(math.log2(MEMORY_SIZE))
# physical_address_bits = int(math.log2(MEMORY_SIZE))
# num_byteoffset_bits = int(math.log2(CACHE_BLOCK_SIZE))

# set_associavity_array = [1, 2, 4, 8]
# x_set = []
# y2_miss = []
# for set_ass in set_associavity_array:
#     x_set.append(set_ass)
#     num_of_set = int(num_cache_block / set_ass)
#     num_index_bit = int(math.log2(num_of_set))
#     num_tag_bit = num_physical_address_bits - num_index_bit - num_byteoffset_bits
#     c1 = Cache(num_of_set, set_ass)
#     c2 = Cache(2 * num_of_set, 2 * set_ass)
#     c1.child = c2

#     # Address traces are linearly generated
#     for k in range(10000):
#         (tag_bit, set_index_bit, _) = int_to_binary(k, num_tag_bit, num_index_bit)
#         c1.writer(set_index_bit, tag_bit)

#     # cnt = 10000
#     # k = 1
#     # while cnt > 0:
#     #     cnt -= 1 
#     #     (tag_bit, set_index_bit, _) = int_to_binary(k, num_tag_bit, num_index_bit)
#     #     c1.writer(set_index_bit, tag_bit)
#     #     k += 8

#     y2_miss.append(c2.read_miss)
#     plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(num_tag_bit))
#     colorInd += 1
#     config_string = "physical address bits: {}, index bits: {}, tab bits: {}, {}-way set associative".format(num_physical_address_bits, 
#     num_index_bit, num_tag_bit, set_ass)
#     print(config_string)

# title_string = "Read miss and hits with L1 cache size {} KB versus L2 cache size {} KB".format(CACHE_SIZE//1024, CACHE_SIZE//512)
# plt.title(title_string)
# plt.xlabel("Set Associativity")
# plt.ylabel("Counts of Read Miss")
# plt.savefig("visual/lru_linear_{}.png".format(10000))
