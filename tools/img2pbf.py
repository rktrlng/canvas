#!/usr/bin/env python3

import os
import sys
from PIL import Image

def convert(filename):
    # Read image
    im = Image.open( filename )

    width, height = im.size
    pixels = im.load()

    bitdepth = 0
    if im.mode == 'L':
        bitdepth = 1
    elif im.mode == 'LA':
        bitdepth = 2
    elif im.mode == 'RGB':
        bitdepth = 3
    elif im.mode == 'RGBA':
        bitdepth = 4
    else:
        bitdepth = 0

    print("converting: ", filename, width, height, bitdepth)

    hiwidth = ((width >> 8) & 0xFF)
    lowidth = (width & 0xFF)
    hiheight = ((height >> 8) & 0xFF)
    loheight = (height & 0xFF)

    allBytes = bytearray([0x70, 0x62, lowidth, hiwidth, loheight, hiheight, bitdepth, 0x3A])

    # grayscale
    if bitdepth == 1:
        for y in range(height):
            for x in range(width):
                cpixel = pixels[x, y]
                allBytes.append(cpixel)

    # 2, 3, or 4
    if bitdepth > 1:
        for y in range(height):
            for x in range(width):
                cpixel = pixels[x, y]
                for b in cpixel:
                    allBytes.append(b)

    no_extension = os.path.splitext(filename)[0]

    saveFile = open(no_extension + ".pbf", "wb")
    saveFile.write(allBytes)


# main
if len(sys.argv) > 1:
    filename = sys.argv[1]
    convert(filename)
else:
    validfiles = [ '.png', '.tga', '.gif', '.bmp']
    for name in os.listdir('.'):
        ext = os.path.splitext(name)[1]
        if ext in validfiles:
            convert(name)
