#pragma once
#include <stdint.h>

typedef struct framebuffer_object
{
    uint32_t* colorb;
    float* depthb;
    int w, h;
} framebuffer;

void InitFramebuffer(framebuffer* fb, int w, int h);

void ClearColorBuffer(framebuffer* fb, uint32_t color);

void ClearDepthBuffer(framebuffer* fb, float far);

void ResizeFrameBuffer(framebuffer* fb, int w, int h);

void DeleteFramebuffer(framebuffer* fb);
