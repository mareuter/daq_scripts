import numpy as np
from astropy.io import fits

filehead = "SCI_REB"
# Amp segment size
shape = (2048, 576)

p_hdr = fits.Header()
detsize = "[1:{},1:{}]".format(shape[1] * 8 * 3, shape[0] * 2 * 3)
print(detsize)
p_hdr["DETSIZE"] = detsize

hdu1 = fits.PrimaryHDU(header=p_hdr)
hdu_list = [hdu1]

for i in [2, 1, 0]:
    for j in range(3):
        toprow = []
        botrow = []

        for k in range(0, 16):
            filename = "{}{}_CCD{}_segment.{:02d}".format(filehead, i, j, k)
            #print(filename)
            pixels = np.fromfile(filename, dtype=np.int32).reshape(shape)
            pixels = pixels[:2000, 10:522]
            if k < 8:
                pixels = pixels[::-1, ::-1]
                toprow.append(pixels)
            else:
                botrow.append(pixels)

        toprow.reverse()
        top = np.hstack(toprow)
        botrow.reverse()
        bot = np.hstack(botrow)
        ccd = np.vstack((bot, top))

        i_hdr = fits.Header()
        detsec = "[{}:{},{}:{}]".format(1 + j * shape[1] * 8, (j + 1) * shape[1] * 8,
                                        1 + (2 - i) * shape[0] * 2, (2 - i + 1) * shape[0] * 2)
        print(detsec)
        i_hdr["DETSEC"] = detsec
        hdu_list.append(fits.ImageHDU(ccd, header=i_hdr))

final_hdu = fits.HDUList(hdu_list)
final_hdu.writeto("SCI_RAFT.fits", overwrite=True)
