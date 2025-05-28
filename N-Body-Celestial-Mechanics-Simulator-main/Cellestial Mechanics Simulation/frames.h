#ifndef FRAMES_H
#define FRAMES_H

#include <stdint.h>

typedef struct {
	uint8_t* img;
	size_t height;
	size_t width;
} frame;

void newFrame(frame** frm, unsigned height, unsigned width);
void writeFrame(frame* frm, char* frmname);
void setPixel(frame* frm, int y, int x, int r, int g, int b);
void drawBlankFrame(frame* frm);
void frm2png(frame* frm, char* frmname);
void destroyFrame(frame* frm, char* frmname);
void makeVideo(int frmRT, char* output);

#endif