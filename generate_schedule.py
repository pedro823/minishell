import random
import sys

def print_usage():
    print("Usage:", sys.argv[0], "<size>")
    sys.exit(1)

if len(sys.argv) != 2:
    print_usage()

size = int(sys.argv[1])
f = open("schedule_auto_" + sys.argv[1], "w")

for i in range(size):
    init_time = round(random.random() * size / 2, 1)
    dt = round(random.random() * 4, 1)
    deadline = round(init_time + dt + random.random() * 4 * (size/25), 1)
    f.write(str(init_time) + " " + str(dt) + " " + str(deadline) + " processo" + str(i) + "\n")
