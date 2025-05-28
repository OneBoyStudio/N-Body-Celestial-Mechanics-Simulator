#include "frames.h"
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void newFrame(frame** frm, size_t height, size_t width) {

	*frm = (frame*)malloc(sizeof(frame));
	
	(*frm)->height = height;
	(*frm)->width = width;
	(*frm)->img = (uint8_t*)malloc(width * height * 4);
}

void writeBytes(FILE* f, int n) {

	uint8_t* bytes = (uint8_t*)malloc(2);

	*bytes = (uint8_t)(n && 0xFFFF) << 8;
	*(bytes + 1) = (uint8_t)(n && 0xFF);

	fwrite(bytes, 1, 2, f);
}

void writeFrame(frame* frm, char* frmname) {

	char* filedir = "frames/";
	char* suffix = ".bin";

	char* filename = strcat(strcat(filedir, frmname), suffix);

	FILE* f = fopen(filename, "wb");

	if (1);

	writeBytes(f, frm->height);
	writeBytes(f, frm->width);

	fwrite(frm->img, 1, frm->height * frm->width * 4, f);
}

void setPixel(frame* frm, int y, int x, int r, int g, int b) {

	frm->img[4 * (y * (frm->width) + x) + 0] = r;
	frm->img[4 * (y * (frm->width) + x) + 1] = g;
	frm->img[4 * (y * (frm->width) + x) + 2] = b;
	frm->img[4 * (y * (frm->width) + x) + 3] = 255;
}

void drawBlankFrame(frame* frm) {

	for (int y = 0; y < frm->height; y++) {

		for (int x = 0; x < frm->width; x++) {

			frm->img[4 * (y * (frm->width) + x) + 0] = 15;
			frm->img[4 * (y * (frm->width) + x) + 1] = 0;
			frm->img[4 * (y * (frm->width) + x) + 2] = 25;
			frm->img[4 * (y * (frm->width) + x) + 3] = 255;
		}
	}
}

void frm2png(frame* frm, char* frmname) {

	char filename[22];

	snprintf(filename, sizeof(filename), "%s.png", frmname);
	printf("%s\n", filename);

	lodepng_encode32_file(filename, frm->img, frm->width, frm->height);

}

void destroyFrame(frame* frm, char* frmname) {

	free(frm->img);
	free(frm);
}

void makeVideo(int frmRT, char* output) {

	char params[100];
	char* frames = "frame%05d.png";
	
	char cwd[MAX_PATH];
	if (GetCurrentDirectoryA(sizeof(cwd), cwd)) {
		printf("Video written to: %s\\out.mp4\n", cwd);
	}
	else {
		printf("Error retrieving working directory.\n");
	}

	snprintf(params, sizeof(params), "ffmpeg -framerate %d -i %s -c:v libx264 -pix_fmt yuv420p %s.mp4", frmRT, frames, output);
	system(params);
}