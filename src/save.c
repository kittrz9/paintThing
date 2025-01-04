#include "save.h"

#include <stdio.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool endsWith(char* str1, char* str2) {
	return (strcmp(str1+strlen(str1)-strlen(str2), str2) == 0);
}

void SDLCALL saveCanvasCallback(void* userdata, const char* const* fileList, int filter) {
	if(fileList == NULL || fileList[0] == NULL) { 
		*((saveDialogUserdata*)userdata)->savingFlag = false; // was set to true in main.c
	}

	saveDialogUserdata canvasData = *(saveDialogUserdata*)userdata;

	printf("%s\n", fileList[0]);

	if(!endsWith(fileList[0], ".png")) {
		printf("image formats other than png are not currently supported\n");
		*((saveDialogUserdata*)userdata)->savingFlag = false; // was set to true in main.c
		return;
	}

	// remove the pitch used in SDL's textures so it can be saved by stb_image_write
	uint32_t* rawPixels = malloc(sizeof(uint32_t) * canvasData.savedCanvasW * canvasData.savedCanvasH);


	uint32_t* p1 = rawPixels;
	uint32_t* p2 = canvasData.savedCanvasPixels;
	for(uint16_t y = 0; y < canvasData.savedCanvasH; ++y) {
		memcpy(p1, p2, sizeof(uint32_t) * canvasData.savedCanvasW);
		p1 += canvasData.savedCanvasW;
		p2 = (uint32_t*)((uint8_t*)p2 + canvasData.savedCanvasPitch);
	}

	// endianness coming back to haunt me once again
	for(uint32_t i = 0; i < canvasData.savedCanvasW * canvasData.savedCanvasH; ++i) {
		rawPixels[i] = ((rawPixels[i] >> 24)&0xff) | ((rawPixels[i] >> 8)&0xff00) | ((rawPixels[i] << 8)&0xff0000) | ((rawPixels[i] << 24)&0xff000000);
	}

	stbi_write_png(fileList[0], canvasData.savedCanvasW, canvasData.savedCanvasH, 4, rawPixels, 0);


	*((saveDialogUserdata*)userdata)->savingFlag = false; // was set to true in main.c

	free(rawPixels);

}
