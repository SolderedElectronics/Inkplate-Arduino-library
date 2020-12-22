from PIL import Image
from os import listdir

iname = "test.bmp"
oname = "test.bmp"

for x in listdir("./jpg"):
    if x[0] == ".":
        continue
    img = Image.open("./jpg/" + x)
    bmp1 = img.convert("1")
    bmp4 = img.convert(mode="P", colors=16)
    bmp8 = img.convert(mode="P", colors=256)

    print(x)

    bmp1.save("./bmp1bit/" + x[:-4] + ".bmp")
    bmp4.save("./bmp4bit/" + x[:-4] + ".bmp")
    bmp8.save("./bmp8bit/" + x[:-4] + ".bmp")
    # bmp16 use ffmpeg
    img.save("./bmp24bit/" + x[:-4] + ".bmp")
    # bmp32 sava as png and then as bmp in convert
