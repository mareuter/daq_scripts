import numpy as np
from astropy.io import fits
import sys

filehead = sys.argv[1]
shape = (2222, 500)

toprow = []
botrow = []

for i in range(0, 16):
    filename = "{}.{:02d}".format(filehead, i)
    print(filename)
    pixels = np.fromfile(filename, dtype=np.int32).reshape(shape)
    if i < 8:
        toprow.append(pixels)
    else:
        botrow.append(pixels)

top = np.hstack(toprow)
bot = np.hstack(botrow)
ccd = np.vstack((bot, top))

print(ccd.shape)

hdu = fits.PrimaryHDU(ccd)
fitsname = filehead.rstrip("_segment")
hdu.writeto("{}.fits".format(fitsname))
