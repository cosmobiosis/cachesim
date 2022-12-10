import random
import matplotlib.pyplot as plt
import math

from cache import Cache, int_to_binary

colorEnums = ["#746ab0", "#ffce30", "#e83845", "#e389b9", "#800080", "#FF7F50", "#800080", "#FF7F50", "#800080", "#FF7F50"]
colorInd = 0

##############################
#  Memory and Cache Parameters
##############################
MEMORY_SIZE = 256 * 1024  # 256KB
CACHE_SIZE = 8 * 1024     # 8KB
CACHE_BLOCK_SIZE = 2      # 2B

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
plt.savefig("visual/blocksize_{}_random".format(CACHE_BLOCK_SIZE))

# ##############################
# #  Memory and Cache Parameters
# ##############################
# MEMORY_SIZE = 256 * 1024  # 256KB
# CACHE_SIZE = 8 * 1024     # 8KB
# CACHE_BLOCK_SIZE = 2      # 2B

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
# plt.savefig("visual/blocksize_{}_linear".format(CACHE_BLOCK_SIZE))
