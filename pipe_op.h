#pragma once

#include <stdint.h>
#include "pipe_pol.h"
#include "framebuffer.h"

#define POLYGON_DISCARD 1
#define POLYGON_CONTINUE 0

typedef struct PIPELINE_OPERATION
{
	/*linear interpolation functions family*/
	//This functions can be skipped by sending NULL;
	//Note! If at least one of this functions have NULL pointer then all of this functions never be called!(like they all have NULL pointer)
	struct lerp_func_family
	{
		/*lerp function that return delta between two vertexes*/
		void (*lerp_pol_data_get_delta_func)(const void *v0_data, const void *v1_data, void *out_delta);

		/*lerp function that multiplicate delta by 't' parameter*/
		void (*lerp_pol_data_delta_mul_t_func)(const void *delta, void *out_data, float t);

		/*lerp function that perform addition of two vertexes attributes*/
		void (*lerp_pol_data_add_func)(const void *v0_data, const void *v1_data, void *out_data);

		/*lerp function that perform full linear interpolation by one call*/
		void (*lerp_pol_data)(const void *v0_data, const void *v1_data, float t, void *out_data);

	} lerp_functions;
	/*~linear interpolation functions family*/

	/*polygon structure that specialize polygon for target pipeline operation*/
	//struct pipe_polygon_func_family
	//{
		/*pipe_polygon function that perform assign of all attributes of polygon*/
	//	void (*polygon_data_assign)(const pipe_polygon *src, pipe_polygon *dst);

		/*pipe_polygon function that copy attribute with 'num' offset to the out_data*/
	//	void (*vertex_data_assign)(const pipe_polygon *pol, int num, void *out_data);
	//} pipe_polygon_functions;

	/*polygon pipeline operation that get polygon, polygon data and loadable data(matrix for ex.)
	and return 'POLYGON_DISCARD' if we don't need to handle this polygon(fail cull test for ex.)
	or return 'POLYGON_CONTINUE' if we need to handle polygon and continue pipeline*/
	//pass NULL if you don't need to process each polygon.
	int (*polygon_pipe_op)(vec4 *v0, vec4 *v1, vec4 *v2, void *v0_data, void *v1_data, void *v2_data, void *loadable_data);

	/*post 3D culling pipeline operation that became after 3D planes culling.
	(May be useful for perspective division for ex.)*/
	//pass NULL if you don't need to manage polygons after 3D culling(for ex. you work with ortho projection)
	void (*post_3Dcull_func)(vec4 *v0, vec4 *v1, vec4 *v2, void *v0_data, void *v1_data, void *v2_data, void *loadable_data);

	/*per-pixel pipeline operation that make instructions for each pixel and list
	of separate pixels in the special framebuffer. You get pointer to the framebuffer,
	count of pixels that can be processe at one pixel pipeline operation call and pixel
	place in the framebuffer*/
	//Must be always!
	void (*pixel_pipe_op)(framebuffer *fb, size_t pixels_count, vec4 *start_pixel, float zstep, void *start_pixel_data, void *data_step);

	void *loadable_data;

	//size of one(!) vertex atrribute
	size_t data_ceil_size;
} pipe_op;


#warning MAKE REALIZATION!
//void* pixel_pipe_op_get_next_pixel_data(size_t i);

uint32_t get_pos(vec4 *pixel, framebuffer *fb);

float get_pixel_fb_z(uint32_t pos, framebuffer *fb);

void set_pixel_into_fb(uint32_t pos, framebuffer *fb, float z, uint32_t color);

