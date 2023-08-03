# -----------
# Simple python script to
# create icon header files for Inkplate 6
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
    if file == ".DS_Store":
        continue
    im = Image.open("./icons/" + file)
    im = im.resize((152, 152))
    alp = im.split()[-1]
    s = [0 for x in range(152 * 152)]
    for y in range(152):
        for x in range(152):
            # print(im.getpixel((x, y)))
            if alp.getpixel((x, y)) > 128:
                s[(x + 152 * y) // 8] |= 1 << (7 - (x + 152 * y) % 8)

    with open("./binary_Icons/icon_" + file[:-4] + ".h", "w") as f:
        print("const uint8_t icon_" + file[:-4] + "[] PROGMEM = {", file=f)
        print(",".join(list(map(hex, s))), file=f)
        print("};", file=f)
