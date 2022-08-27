#pragma once

#include "vec.h"
#include <stdint.h>
#include <stddef.h>

/*polygon structure that specialize polygon for target pipeline operation*/
typedef struct
{
	//coordinates of each vertex in 4-component format
	union {vec4 v0; vec4 top;};
	union {vec4 v1; vec4 med;};
	union {vec4 v2; vec4 low;};

	/*Warning! If you want to put attrubutes together with vertexes
	then make like this scheme:
	v0, v1, v2, attr0, attr1, attr2, ...
	or use separate buffers*/
	#warning MAKE USE OF SEPARATE BUFFERS!
} pipe_polygon;

pipe_polygon* pipe_polygon_alloc_data(size_t size_of_data);

void pipe_polygon_free_alloc(pipe_polygon *pol);

void pipe_polygon_copy_attr(pipe_polygon *src, pipe_polygon *dst, size_t size_of_data, uint8_t srci, uint8_t dsti);
