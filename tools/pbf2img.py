#!/usr/bin/env python3

import os
import sys
from PIL import Image, ImageDraw

filename = "my_pixel_buffer.pbf"

if len(sys.argv) > 1:
    filename = sys.argv[1]

# Read .pbf image
with open(filename, "rb") as binaryfile :
    pbf = bytearray(binaryfile.read())

# Read header information
# pbf[0] = 'p'
# pbf[1] = 'b'
lowidth = pbf[2]
hiwidth = pbf[3]
loheight = pbf[4]
hiheight = pbf[5]
bitdepth = pbf[6]
# pbf[7] = ':'

# Little endian
width = (hiwidth << 8) | (lowidth & 0xFF)
height = (hiheight << 8) | (loheight & 0xFF)

# The rest of the bytes are the pixels
pixels = pbf[8:]

print("width:    ", width)
print("height:   ", height)
print("bitdepth: ", bitdepth)
# print(pixels)

if bitdepth == 8:
	mode = 'L'
if bitdepth == 16:
	mode = 'LA'
if bitdepth == 24:
	mode = 'RGB'
if bitdepth == 32:
	mode = 'RGBA'

print("#pixels:  ", int( len(pixels) / (bitdepth/8)) )

img = Image.new(mode, (width, height), color = 'white')
imgpixels = img.load()

start = 0
for y in range(height):
	for x in range(width):
		if bitdepth == 8:
			color = ( pixels[start + 0] )
		if bitdepth == 16:
			color = ( pixels[start + 0], pixels[start + 1] )
		if bitdepth == 24:
			color = ( pixels[start + 0], pixels[start + 1], pixels[start + 2] )
		if bitdepth == 32:
			color = ( pixels[start + 0], pixels[start + 1], pixels[start + 2], pixels[start + 3] )

		imgpixels[x, y] = color
		start += int(bitdepth / 8)

no_extension = os.path.splitext(filename)[0]

if len(sys.argv) > 2:
	imgtype = sys.argv[2]
	if imgtype == 'tga':
		img.save( no_extension + '.tga', 'TGA' )
	elif imgtype == 'png':
		img.save( no_extension + '.png', 'PNG' )
	elif imgtype == 'gif':
		img.save( no_extension + '.gif', 'GIF' )
	elif imgtype == 'jpg':
		img.save( no_extension + '.jpg', 'JPEG' )
	elif imgtype == 'jpeg':
		img.save( no_extension + '.jpeg', 'JPEG' )
	elif imgtype == 'bmp':
		img.save( no_extension + '.bmp', 'BMP' )
	else:
		print("Unknown type, save as PNG (default)")
		img.save( no_extension + '.png', 'PNG' )
else:
	print("No type, save as PNG (default)")
	img.save( no_extension + '.png', 'PNG' )
