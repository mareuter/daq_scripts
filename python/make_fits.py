import numpy as np
from astropy.io import fits
import sys

filehead = sys.argv[1]
shape = (2048, 576)
#shape = (2222, 500)

toprow = []
botrow = []

for i in range(0, 16):
    filename = "{}.{:02d}".format(filehead, i)
    print(filename)
    pixels = np.fromfile(filename, dtype=np.int32).reshape(shape)
    pixels = pixels[:2000, 10:522]
    if i < 8:
        pixels = pixels[::-1, ::-1]
        toprow.append(pixels)
    else:
        botrow.append(pixels)

toprow.reverse()
top = np.hstack(toprow)
botrow.reverse()
bot = np.hstack(botrow)
ccd = np.vstack((bot, top))

print(ccd.shape)

hdu = fits.PrimaryHDU(ccd)
fitsname = filehead.rstrip("_segment")
hdu.writeto("{}.fits".format(fitsname))
