#!/bin/env python3

# could probably compress the images, especially with something like the font being just two colors

import sys
import os
from PIL import Image
from datetime import datetime

def main():
	if len(sys.argv) != 3:
		return 1

	filePath = sys.argv[1]

	mode = sys.argv[2]

	name = os.path.basename(filePath).split(".")[0]

	if not os.path.exists(filePath):
		return 1

	img = Image.open(filePath)
	imgRGBA = img.convert("RGBA")
	pixels = imgRGBA.load()


	if(mode[0] == 'c'):
		print("/*")
		print(f"\tgenerated on {datetime.today().strftime('%Y-%m-%d %H:%M:%S')}")
		print(f"\t\"{' '.join(sys.argv)}\"")
		print("*/")
		print("#include <stdint.h>")
		print(f"uint16_t {name}ImgW = {img.size[0]};")
		print(f"uint16_t {name}ImgH = {img.size[1]};")
		print(f"uint8_t {name}Img[] = {{")
		for y in range(img.size[1]):
			for x in range(img.size[0]):
				print(pixels[x,y][0], end=',')
				print(pixels[x,y][1], end=',')
				print(pixels[x,y][2], end=',')
				print(pixels[x,y][3], end=',')
		print("\n};")
	elif(mode[0] == 'h'):
		print("/*")
		print(f"\tgenerated on {datetime.today().strftime('%Y-%m-%d %H:%M:%S')}")
		print(f"\t\"{' '.join(sys.argv)}\"")
		print("*/")
		print("#include <stdint.h>")
		print(f"extern uint16_t {name}ImgW;")
		print(f"extern uint16_t {name}ImgH;")
		print(f"extern uint8_t {name}Img[];")
	
	return 0


if __name__=="__main__":
	exit(main())
