#pragma once
#include "vec.h"
#include "stdint.h"
#include "pipe_pol.h"
#include "pipe_op.h"

typedef struct
{
	struct node
	{
		struct node *next, *prev;
		void *data;
	} *start, *end;
} polygon_list;

void polygon_list_push_back(polygon_list *l, pipe_polygon *pol, size_t data_ceil_size);

void polygon_list_push_after_node(polygon_list *l, struct node *n, pipe_polygon *pol, size_t data_ceil_size);

void polygon_list_pop(polygon_list *l, struct node *n);

void polygon_list_delete(polygon_list *l);


int line_intersect_plane_3D(vec4 *st, void *st_data, vec4 *en, void *en_data, float *pl, vec4 *out, void *out_data, pipe_op *op);

/*
not intersect{
//ret 0 if vertexes on side of plane normal
//ret 4 if vertexes on another side of plane normal
}
intersect{
ret 1 if v0 on side and v1 and v2 not at side
ret 2 if v1 on side and v0 and v2 not at side
ret 3 if v2 on side and v1 and v0 not at side
ret -1 if v1 and v2 on side and v0 not at side
ret -2 if v0 and v2 on side and v1 not at side
ret -3 if v1 and v0 on side and v2 not at side
}
*/
int polygon_intersect_plane_3D(pipe_polygon *pol, float *pl, pipe_polygon *out0, pipe_polygon *out1, pipe_op *op);

enum bb_edge_type
{
	LEFT = 1,
	TOP = 2,
	RIGHT = 4,
	BOTTOM = 8
};

/*
not intersect{
ret 0 if inside BB
ret 2 if outside BB
}
intersect{
ret 1, if en outside BB
ret -1 if st outside BB
}
*/
int line_intersect_BB_edge(vec4 *st, void *st_data, vec4 *en, void *en_data, vec4 *lh, vec4 *rl, vec4 *out, void *out_data,  enum bb_edge_type bt, pipe_op *op);

/*
not intersect{
//ret 0 if vertexes inside BB
//ret 4 if vertexes outside BB
}
intersect{
ret 1 if v0 inside and v1 and v2 outside
ret 2 if v1 inside and v0 and v2 outside
ret 3 if v2 inside and v1 and v0 outside
ret -1 if v1 and v2 inside and v0 outside
ret -2 if v0 and v2 inside and v1 outside
ret -3 if v1 and v0 inside and v2 outside
}
*/
int polygon_intersect_BB_edge(pipe_polygon *pol, vec4 *lh, vec4 *rl, pipe_polygon *out0, pipe_polygon *out1, enum bb_edge_type bt, pipe_op *op);
