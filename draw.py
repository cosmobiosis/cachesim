import random
import matplotlib.pyplot as plt

class Cache:
	def __init__(self, num_set, set_ass, child=None):
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

	def readr(self, set_index, tag):
		# read from memory
		self.read += 1
		jerk = self.cache[set_index]
		for i in range(len(jerk)):
			if jerk[i] == tag:
				self.read_hit += 1
				return
		self.read_miss += 1
		if self.child is None:
			self.mem_read += 1
		else:
			self.child.readr(set_index, tag)
		choose = [i for i in range(len(jerk))]
		jerk[random.choice(choose)] = tag

	def writer(self, set_index, tag):
		self.write += 1
		jerk = self.cache[set_index]
		find_flag = False
		for i in range(len(jerk)):
			if jerk[i] == tag:
				self.write_hit += 1
				find_flag = True

		if not find_flag:
			self.write_miss += 1
			self.readr(set_index, tag)

colorEnums = ["#746ab0", "#ffce30", "#e83845", "#e389b9", "#800080", "#FF7F50", "#HotPink"]
colorInd = 0

for blocks in [256]:
	for tagUpper in [4]:
		x_set = []
		y2_hit = []
		y2_miss = []
		for j in [1, 2, 4, 8]:
			x_set.append(j)
			i = int(blocks / j)
			c1 = Cache(i, j)
			c2 = Cache(2 * i, 2 * j)
			c1.child = c2
			for k in range(50000):
				rand_set_index = random.choice([x for x in range(i)])
				tag = random.choice([x for x in range(tagUpper)])
				c1.writer(rand_set_index, tag)
			y2_hit.append(c2.read_hit)
			y2_miss.append(c2.read_miss)

		print(tagUpper, y2_miss)

		plt.plot(x_set, y2_hit, '.-', color = colorEnums[colorInd], label="L2 cache hit tag: " + str(tagUpper))
		plt.plot(x_set, y2_miss, 'x-', color = colorEnums[colorInd], label="L2 cache miss tag: " + str(tagUpper))
		colorInd += 1

	plt.title("Read miss and hits with L1 cache size 16 KB versus L2 cache size 32 KB")
	plt.xlabel("Set Associativity")
	plt.ylabel("Counts of events")
	plt.legend(loc="best")
	plt.show()

# 1 [256, 128, 64, 32]
# 2 [5142, 376, 161, 72]
# 4 [7549, 5058, 523, 167]
# 8 [8741, 7601, 5209, 666]