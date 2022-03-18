#include "framebuffer.h"
#include <malloc.h>

void InitFramebuffer(framebuffer* fb, int w, int h)
{
    fb->colorb = malloc(w * h * sizeof(uint32_t));
    fb->depthb = malloc(w * h * sizeof(float));
    fb->w = w;
    fb->h = h;
}

void ClearColorBuffer(framebuffer* fb, uint32_t color)
{
    uint32_t* ptr = fb->colorb;
    while(ptr < fb->colorb + fb->w * fb->h)
    {
        *ptr = color;
        ++ptr;
    }
}

void ClearDepthBuffer(framebuffer* fb, float far)
{
    float* ptr = fb->depthb;
    while(ptr < fb->depthb + fb->w * fb->h)
    {
	*ptr = far;
	++ptr;
    }
}

void ResizeFrameBuffer(framebuffer* fb, int w, int h)
{
    free(fb->colorb);
    free(fb->depthb);
    fb->colorb = malloc(w * h * sizeof(uint32_t));
    fb->depthb = malloc(w * h * sizeof(float));
    fb->w = w;
    fb->h = h;
}

void DeleteFramebuffer(framebuffer* fb)
{
    free(fb->colorb);
    free(fb->depthb);
    fb->colorb = NULL;
    fb->depthb = NULL;
    fb->w = fb->h = 0;
}
