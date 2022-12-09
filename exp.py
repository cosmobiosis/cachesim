import random
import matplotlib.pyplot as plt
import math

class Cache:
    def __init__(self, num_set, set_ass, blockSize=1, child=None):
        self.num_set = num_set
        self.set_associavity = set_ass
        self.blockSize = blockSize
        self.read = 0
        self.read_hit = 0
        self.read_miss = 0
        self.mem_read = 0
        self.write = 0
        self.write_hit = 0
        self.write_miss = 0
        self.mem_write = 0
        self.child = child
        self.cache = [[-1 for _ in range(set_ass)] for _ in range(num_set)]
        # ValidBitTable: we initialize all the valid bits to 1 initially  
        self.validBitTable = [[1 for _ in range(set_ass)] for _ in range(num_set)]
        # CacheBlock: 3D Array
        self.cacheBlockInit()

    def cacheBlockInit(self):
        self.cacheBlockTable = []
        for _ in range(self.num_set):
            curSet = []
            for _ in range(self.set_associavity):
                emptyBlock = [0] * self.blockSize
                curSet.append(emptyBlock)
            self.cacheBlockTable.append(curSet)

    def readr(self, set_index, tag):
        # read from memory
        self.read += 1
        setBlock = self.cache[set_index]
        for i in range(len(setBlock)):
            if setBlock[i] == tag:
                self.read_hit += 1
                return
        self.read_miss += 1
        if self.child is None:
            self.mem_read += 1
        else:
            self.child.readr(set_index, tag)
        # Random Eviction 
        choose = [i for i in range(len(setBlock))]
        setBlock[random.choice(choose)] = tag
        
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


colorEnums = ["#746ab0", "#ffce30", "#e83845", "#e389b9", "#800080", "#FF7F50", "#800080", "#FF7F50", "#800080", "#FF7F50"]
colorInd = 0

############################
#   Configurable parameters 
############################
MEMORY_SIZE = 256 * 1024  # 256KB
CACHE_SIZE = 8 * 1024     # 8KB
CACHE_BLOCK_SIZE = 8      # 8B

num_cache_block = CACHE_SIZE / CACHE_BLOCK_SIZE
num_physical_address_bits = int(math.log2(MEMORY_SIZE))
physical_address_bits = int(math.log2(MEMORY_SIZE))
num_byteoffset_bits = int(math.log2(CACHE_BLOCK_SIZE))

# set_associavity_array = [1, 2, 4, 8, 16, 32, 64, 128]
set_associavity_array = [1, 2, 4, 8, 16]
x_set = []
y2_hit = []
y2_miss = []
for set_ass in set_associavity_array:
    x_set.append(set_ass)
    num_of_set = int(num_cache_block / set_ass)
    index_bits = int(math.log2(num_of_set))
    tag_bit = num_physical_address_bits - index_bits - num_byteoffset_bits
    c1 = Cache(num_of_set, set_ass)
    c2 = Cache(2 * num_of_set, 2 * set_ass)
    c1.child = c2
    tag_upper = 2 ** tag_bit
    print(tag_upper)
    for k in range(10000):
        rand_set_index = random.choice(list(range(num_of_set)))   
        tag = random.choice([x for x in range( 2 ** tag_bit )])
        c1.writer(rand_set_index, tag)
    y2_hit.append(c2.read_hit)
    y2_miss.append(c2.read_miss)
    # plt.plot(x_set, y2_hit, '.-', color = colorEnums[colorInd], label="L2 cache hit tag: " + str(tagUpper))
    plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(tag_bit))
    colorInd += 1
    config_string = "physical address bits: {}, index bits: {}, tab bits: {}, {}-way set associative".format(num_physical_address_bits, 
    index_bits, tag_bit, set_ass)
    print(config_string)

title_string = "Read miss and hits with L1 cache size {} KB versus L2 cache size {} KB".format(CACHE_SIZE//1024, CACHE_SIZE//512)
plt.title(title_string)
plt.xlabel("Set Associativity")
plt.ylabel("Counts of Read Miss")
# plt.show()
plt.savefig("random.png")


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

# # set_associavity_array = [1, 2, 4, 8, 16, 32, 64, 128]
# set_associavity_array = [1, 2, 4, 8, 16]
# x_set = []
# y2_hit = []
# y2_miss = []
# for set_ass in set_associavity_array:
#     x_set.append(set_ass)
#     num_of_set = int(num_cache_block / set_ass)
#     index_bits = int(math.log2(num_of_set))
#     tag_bit = num_physical_address_bits - index_bits - num_byteoffset_bits
#     c1 = Cache(num_of_set, set_ass)
#     c2 = Cache(2 * num_of_set, 2 * set_ass)
#     c1.child = c2
#     tag_upper = 2 ** tag_bit
#     cc = 0
#     print(tag_upper)
#     for k in range(10000):
#         rand_set_index = random.choice(list(range(num_of_set)))   
#         tag = random.choice([x for x in range( 2 ** tag_bit )])
#         tag = (cc + 1) % tag_upper
#         c1.writer(rand_set_index, tag)
#     y2_hit.append(c2.read_hit)
#     y2_miss.append(c2.read_miss)
#     # plt.plot(x_set, y2_hit, '.-', color = colorEnums[colorInd], label="L2 cache hit tag: " + str(tagUpper))
#     plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(tag_bit))
#     colorInd += 1
#     config_string = "physical address bits: {}, index bits: {}, tab bits: {}, {}-way set associative".format(num_physical_address_bits, 
#     index_bits, tag_bit, set_ass)
#     print(config_string)

# title_string = "Read miss and hits with L1 cache size {} KB versus L2 cache size {} KB".format(CACHE_SIZE//1024, CACHE_SIZE//512)
# plt.title(title_string)
# plt.xlabel("Set Associativity")
# plt.ylabel("Counts of Read Miss")
# # plt.show()
# plt.savefig("random_linear.png")

