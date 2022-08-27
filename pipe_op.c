#include "pipe_op.h"
#include "framebuffer.h"

uint32_t get_pos(vec4 *pixel, framebuffer *fb)
{
	return (pixel->yc * fb->w + pixel->xc);
}

float get_pixel_fb_z(uint32_t pos, framebuffer *fb)
{
	return (fb->depthb[pos]);
}

void set_pixel_into_fb(uint32_t pos, framebuffer *fb, float z, uint32_t color)
{
	fb->depthb[pos] = z;
	fb->colorb[pos] = color;
}

