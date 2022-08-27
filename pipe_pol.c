#include "pipe_pol.h"
#include <malloc.h>
#include <string.h>

pipe_polygon* pipe_polygon_alloc_data(size_t size_of_data)
{
	return malloc(size_of_data * 3 + sizeof(pipe_polygon));
}

void pipe_polygon_free_alloc(pipe_polygon *pol)
{
	free(pol);
}

void pipe_polygon_copy_attr(pipe_polygon *src, pipe_polygon *dst, size_t size_of_data, uint8_t srci, uint8_t dsti)
{
	if(size_of_data != 0)
	{
		memcpy(((uint8_t*)(&dst->v2 + 1)) + size_of_data * dsti, ((uint8_t*)(&src->v2 + 1)) + size_of_data * srci, size_of_data);
	}
}
