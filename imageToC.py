#!/bin/env python3

import sys
import os
from PIL import Image
from datetime import datetime

# tried bzip2 but I couldn't get it to either compress or decompress right, I'm not sure which
import lzma

def main():
	filePath = sys.argv[1]

	name = os.path.basename(filePath).split(".")[0]

	if not os.path.exists(filePath):
		return 1

	img = Image.open(filePath)
	imgRGBA = img.convert("RGBA")
	imgBytes = imgRGBA.tobytes()

	compressedBytes = lzma.compress(imgBytes)

	dataFile = ""
	dataFile += "/*\n"
	dataFile += f"\tgenerated on {datetime.today().strftime('%Y-%m-%d %H:%M:%S')}\n"
	dataFile += f"\t\"{' '.join(sys.argv)}\"\n"
	dataFile += "*/\n"
	dataFile += "#include <stdint.h>\n"
	dataFile += f"uint16_t {name}ImgW = {img.size[0]};\n"
	dataFile += f"uint16_t {name}ImgH = {img.size[1]};\n"
	dataFile += f"uint8_t {name}ImgLZMA[] = {{\n"
	for b in compressedBytes:
		dataFile += hex(b)
		dataFile += ","
	dataFile += "\n};\n"

	with open(f"src/generated/{name}.c", "w") as f:
		f.write(dataFile)


	headerFile = ""
	headerFile += "/*\n"
	headerFile += f"\tgenerated on {datetime.today().strftime('%Y-%m-%d %H:%M:%S')}\n"
	headerFile += f"\t\"{' '.join(sys.argv)}\"\n"
	headerFile += "*/\n"
	headerFile += "#include <stdint.h>\n"
	headerFile += f"extern uint16_t {name}ImgW;\n"
	headerFile += f"extern uint16_t {name}ImgH;\n"
	headerFile += f"extern uint8_t {name}ImgLZMA[];\n"
	headerFile += f"#define {name}ImgLZMALen {len(compressedBytes)}\n"

	with open(f"src/generated/{name}.h", "w") as f:
		f.write(headerFile)
	
	return 0


if __name__=="__main__":
	exit(main())
