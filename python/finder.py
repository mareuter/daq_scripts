import sys

array_size = int(sys.argv[1])

for i in range(2, array_size - 1):
    if array_size % i == 0:
        print("Factor: {}".format(i))
