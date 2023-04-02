# -----------
# Simple python script to
# create icon header files for Inkplate 6COLOR
# Arduino sketches
#
# Takes all files from /icons and saves them to /binary_Icons
#
# -----------

from PIL import Image
import os, sys

if not os.path.isdir("./binary_Icons"):
    os.mkdir(os.path.abspath(os.getcwd()) + "/binary_Icons")

for file in os.listdir("./icons"):
    im = Image.open("./icons/" + file)
    im = im.resize((48, 48))
    alp = im.split()[-1]
    s = [0 for x in range(48 * 48)]
    for y in range(48):
        for x in range(48):
            # print(im.getpixel((x, y)))
            if alp.getpixel((x, y)) > 128:
                s[(x + 48 * y) // 8] |= 1 << (7 - (x + 48 * y) % 8)

    with open("./binary_Icons/icon_s_" + file[:-4] + ".h", "w") as f:
        print("const uint8_t icon_s_" + file[:-4] + "[] PROGMEM = {", file=f)
        print(",".join(list(map(hex, s))), file=f)
        print("};", file=f)
