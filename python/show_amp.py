import matplotlib.pyplot as plt
import numpy as np
import sys

filename = sys.argv[1]

#shape = (489, 142)
#shape = (500, 2222)
shape = (2048, 576)

pixels = np.fromfile(filename, dtype=np.int32).reshape(shape)
#print(pixels)
pixels = pixels[1:2001, 11:521]
#pixels = pixels[::-1, ::-1]
print(pixels.shape)
plt.imshow(pixels)
plt.show()
