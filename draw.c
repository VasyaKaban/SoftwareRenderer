#include "draw.h"
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "cull.h"

framebuffer* TargetFB = NULL;

void SetTargetFB(framebuffer* fb)
{
    TargetFB = fb;
}

/*void DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xs = 0, ys = 0;
    int hd;
    if(dx < 0) {xs = -1; dx = -dx;}
    else xs = 1;
    if(dy < 0) {ys = -1; dy = - dy;}
    else ys = 1;
    if(dx > dy)
    {
		hd = dy;
		while(x0 != x1 + xs)
		{
		    SetPixel(x0, y0, 1.0f, color);
	   	 	x0 += xs;
	    	dy += hd;
	    	if(dy >= dx)
	    	{
				dy -= dx;
				y0 += ys;
		    }
		}
    }
    else
    {
		hd = dx;
		while(y0 != y1 + ys)
		{
	    	SetPixel(x0, y0, 1.0f, color);
	    	y0 += ys;
	    	dx += hd;
	    	if(dx >= dy)
	    	{
				dx -= dy;
				x0 += xs;
	    	}
		}
    }
}*/

/*void DrawLineDepth(vec2i v0, vec2i v1)
{
    int dx = v1.x - v0.x;
    int dy = v1.y - v0.y;
    int xs = 0, ys = 0;
    int hd;
    if(dx < 0) {xs = -1; dx = -dx;}
    else xs = 1;
    if(dy < 0) {ys = -1; dy = - dy;}
    else ys = 1;

	float stepz;

    if(dx > dy)
    {
		stepz = (v1.z - v0.z) / dx;
		hd = dy;
		while(v0.x != v1.x)
		{
		    SetPixel(v0.x, v0.y, v0.z, tcolor);
	   	 	v0.x += xs;
			v0.z += stepz;
	    	dy += hd;
	    	if(dy >= dx)
	    	{
				dy -= dx;
				v0.y += ys;
		    }
		}
    }
    else
    {
		stepz = (v1.z - v0.z) / dy;
		hd = dx;
		while(v0.y != v1.y)
		{
	    	SetPixel(v0.x, v0.y, v0.z, tcolor);
	    	v0.y += ys;
			v0.z += stepz;
	    	dx += hd;
	    	if(dx >= dy)
	    	{
				dx -= dy;
				v0.x += xs;
	    	}
		}
    }
}*/

void Cull3D(polygon_list *l, float *pl, size_t pl_count, pipe_op *op)
{
	int plane_cull_state = 0;
	pipe_polygon *out0;
	pipe_polygon *out1;
	out0 = pipe_polygon_alloc_data(op->data_ceil_size);
	out1 = pipe_polygon_alloc_data(op->data_ceil_size);
	struct node *n;
	size_t pl_count_pass = 0;
	while(pl_count_pass != pl_count && l->start != NULL)
	{
		n = l->start;
		while(n != NULL)
		{
			plane_cull_state = polygon_intersect_plane_3D(n->data, &pl[pl_count_pass * 4], out0, out1, op);
			if(plane_cull_state == 0)//on side
			{
				//nothing!
				n = n->next;
			}
			else if(plane_cull_state == 4)//on another side
			{
				if(n->prev == NULL && n->next == NULL)
				{
					polygon_list_pop(l, n);
					n = NULL;
				}
				else
				{
					if(n->next != NULL)
					{
						n = n->next;
						polygon_list_pop(l, n->prev);
					}
					else
					{
						polygon_list_pop(l, n);
						n = NULL;
					}
				}
			}
			else if(plane_cull_state < 0)//out0 and out1 are used
			{
				*(pipe_polygon*)n->data = *out0;
				memcpy(&((pipe_polygon*)n->data)->v2 + 1, &out0->v2 + 1, op->data_ceil_size * 3);
				polygon_list_push_after_node(l, n, out1, op->data_ceil_size);
				n = n->next;
				n = n->next;
			}
			else//out0 only used
			{
				*(pipe_polygon*)n->data = *out0;
				memcpy(&((pipe_polygon*)n->data)->v2 + 1, &out0->v2 + 1, op->data_ceil_size * 3);
				n = n->next;
			}
		}
		pl_count_pass++;
	}

	pipe_polygon_free_alloc(out0);
	pipe_polygon_free_alloc(out1);
}

void pol_min_max(pipe_polygon *pol, vec4 *min, vec4 *max)
{
	if(pol->v0.x < min->x)
		min->x = pol->v0.x;
	if(pol->v0.y < min->y)
		min->y = pol->v0.y;
	if(pol->v1.x < min->x)
		min->x = pol->v1.x;
	if(pol->v1.y < min->y)
		min->y = pol->v1.y;
	if(pol->v2.x < min->x)
		min->x = pol->v2.x;
	if(pol->v2.y < min->y)
		min->y = pol->v2.y;

	if(pol->v0.x > max->x)
		max->x = pol->v0.x;
	if(pol->v0.y > max->y)
		max->y = pol->v0.y;
	if(pol->v1.x > max->x)
		max->x = pol->v1.x;
	if(pol->v1.y > max->y)
		max->y = pol->v1.y;
	if(pol->v2.x > max->x)
		max->x = pol->v2.x;
	if(pol->v2.y > max->y)
		max->y = pol->v2.y;
}

void Cull2D(polygon_list *l, vec4 *lh, vec4 *rl, pipe_op *op)
{
	int edge_cull_state = 0;
	pipe_polygon *out0;
	pipe_polygon *out1;
	out0 = pipe_polygon_alloc_data(op->data_ceil_size);
	out1 = pipe_polygon_alloc_data(op->data_ceil_size);
	struct node *n;
	int BB_status = 0;
	/*1 - left
	  2 - top
	  4 - right
	  8 - bottom */
	vec4 min, max;
	n = l->start;
	min.x = ((pipe_polygon*)n->data)->v0.x;
	min.y = ((pipe_polygon*)n->data)->v0.y;
	max.x = ((pipe_polygon*)n->data)->v0.x;
	max.y = ((pipe_polygon*)n->data)->v0.y;
	while(n != NULL)
	{
		pol_min_max(n->data, &min, &max);
		n = n->next;
	}

	if((min.x > rl->x && max.x > rl->x) || (min.x < lh->x && max.x < lh->x) || (min.y < rl->y && max.y < rl->y) || (min.y > lh->y && max.y > lh->y))
		BB_status = 0;
	else
	{
		if(min.x >= lh->x && max.x >= lh->x)
			BB_status += 1;
		if(min.y <= lh->y && max.y <= lh->y)
			BB_status += 2;
		if(min.x <= rl->x && max.x <= rl->x)
			BB_status += 4;
		if(min.y >= rl->y && max.y >= rl->y)
			BB_status += 8;
	}

	if(BB_status == 0)//all outisde
	{
		polygon_list_delete(l);
	}
	else if(BB_status == 15)//all inside
		return;

	int target_status = LEFT;

	while(BB_status != 15 && l->start != NULL)
	{
		if(!(BB_status & target_status))
		{
			n = l->start;
			while(n != NULL)
			{
				edge_cull_state = polygon_intersect_BB_edge(n->data, lh, rl, out0, out1, target_status, op);
				if(edge_cull_state == 0)//on side
				{
					//nothing!
					n = n->next;
				}
				else if(edge_cull_state == 4)//on another side
				{
					if(n->prev == NULL && n->next == NULL)
					{
						polygon_list_pop(l, n);
						n = NULL;
					}
					else
					{
						if(n->next != NULL)
						{
							n = n->next;
							polygon_list_pop(l, n->prev);
						}
						else
						{
							polygon_list_pop(l, n);
							n = NULL;
						}
					}
				}
				else if(edge_cull_state < 0)//out0 and out1 are used
				{
					*(pipe_polygon*)n->data = *out0;
					memcpy(&((pipe_polygon*)n->data)->v2 + 1, &out0->v2 + 1, op->data_ceil_size * 3);
					polygon_list_push_after_node(l, n, out1, op->data_ceil_size);
					n = n->next;
					n = n->next;
				}
				else//out0 only used
				{
					*(pipe_polygon*)n->data = *out0;
					memcpy(&((pipe_polygon*)n->data)->v2 + 1, &out0->v2 + 1, op->data_ceil_size * 3);
					n = n->next;
				}
			}
			BB_status = BB_status | target_status;
		}
		target_status = (target_status << 1);
	}

	pipe_polygon_free_alloc(out0);
	pipe_polygon_free_alloc(out1);
}

void Draw(const void *vertexes, size_t count, float *pl, size_t pl_count, vec4 *lh, vec4 *rl, pipe_op *op, void *attributes, int attributes_type)
{
	polygon_list list = {NULL, NULL};
	pipe_polygon *pol;
	pol = pipe_polygon_alloc_data(op->data_ceil_size);
	struct node *n;
	int polygon_pipe_op_result = POLYGON_CONTINUE;
    for(size_t i = 0; i < count; i+= 6)
    {
		pol->v0 = ((vec4*)vertexes)[i];
		pol->v1 = ((vec4*)vertexes)[i + 1];
		pol->v2 = ((vec4*)vertexes)[i + 2];
		memcpy(&pol->v2 + 1, &(((vec4*)vertexes)[i + 3]), op->data_ceil_size);
		memcpy(((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, &(((vec4*)vertexes)[i + 4]), op->data_ceil_size);
		memcpy(((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &(((vec4*)vertexes)[i + 5]), op->data_ceil_size);

		if(op->polygon_pipe_op != NULL)
		{
			polygon_pipe_op_result = op->polygon_pipe_op(&pol->v0, &pol->v1, &pol->v2, (void*)(&pol->v2 + 1), (void*)((uint8_t*)(&pol->v2 + 1) + op->data_ceil_size), (void*)((uint8_t*)(&pol->v2 + 1) + op->data_ceil_size * 2), op->loadable_data);
			if(polygon_pipe_op_result != POLYGON_DISCARD)
			{
				polygon_list_push_back(&list, pol, op->data_ceil_size);
			}
		}
		else
			polygon_list_push_back(&list, pol, op->data_ceil_size);

		#warning MAKE SURE THAT LEN OF LIST != 0

		Cull3D(&list, pl, pl_count, op);

		if(op->post_3Dcull_func != NULL)
		{
			n = list.start;
			while(n != NULL)
			{
				op->post_3Dcull_func(&((pipe_polygon*)n->data)->v0, &((pipe_polygon*)n->data)->v1, &((pipe_polygon*)n->data)->v2, &((pipe_polygon*)n->data)->v2 + 1, ((uint8_t*)(&((pipe_polygon*)n->data)->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&((pipe_polygon*)n->data)->v2 + 1)) + op->data_ceil_size * 2, op->loadable_data);
				n = n->next;
			}
		}

		if(list.start != NULL)
			Cull2D(&list, lh, rl, op);

		n = list.start;
		while(n != NULL)
		{
#warning MAKE THIS LIKE PIPELINE OPERATION + CONTEXT!
			ToScreenSpace(&((pipe_polygon*)n->data)->v0.x, &((pipe_polygon*)n->data)->v0.y);
			ToScreenSpace(&((pipe_polygon*)n->data)->v1.x, &((pipe_polygon*)n->data)->v1.y);
			ToScreenSpace(&((pipe_polygon*)n->data)->v2.x, &((pipe_polygon*)n->data)->v2.y);

			DrawPolygon(n->data, op);
			n = n->next;
		}
		polygon_list_delete(&list);
    }
	pipe_polygon_free_alloc(pol);
}

void ToScreenSpace(float *x, float *y)
{
    *x = (float)(TargetFB->w) / 2 * (*x) + (float)(TargetFB->w) / 2;
    *y = -(float)(TargetFB->h) / 2 * (*y) + (float)(TargetFB->h) / 2;
}

void DrawInterval(int32_t y, int_float left, int_float right, void *left_data, void *right_data, pipe_op *op/*int lint, int lnum, int ldsor, int rint, int rnum, int rdsor, int y, float lz, float rz*/)
{
	if(left.num > left.dsor / 2) left.integ++;
	if(right.num > right.dsor / 2) right.integ++;

	int32_t len = right.integ - left.integ;
	float dz = (right.z - left.z) * 0.5f / len;
	left.zstep = dz * 2;
	left.z += dz;

	void *target_data, *data_step;
	target_data = malloc(op->data_ceil_size);
	data_step = malloc(op->data_ceil_size);

	op->lerp_functions.lerp_pol_data_get_delta_func(left_data, right_data, data_step);
	op->lerp_functions.lerp_pol_data_delta_mul_t_func(data_step, target_data, 0.5f / len);
	op->lerp_functions.lerp_pol_data_add_func(target_data, target_data, data_step);
	op->lerp_functions.lerp_pol_data_add_func(target_data, left_data, target_data);

	vec4 start_pixel = {.xc = left.integ, .yc = y, .z = left.z, .w = 0.0f};

	op->pixel_pipe_op(TargetFB, right.integ - left.integ, &start_pixel, left.zstep, target_data, data_step);

	free(target_data);
	free(data_step);
}

void pmx(int_float *line, int32_t step)
{
	#warning maybe change to mul and div?
	if(step < 0)
	{
		while(line->num < 0)
		{
			line->num += line->dsor;
			line->integ--;
		}
	}
	else
	{
		while(line->num > line->dsor)
		{
			line->num -= line->dsor;
			line->integ++;
		}
	}
}

float get_t_param(const vec4 *dir, int_float *target_point, int32_t start_point)
{
	float len = sqrtf((float)(dir->xc * dir->xc + dir->yc * dir->yc)) / 2;
	int32_t num = (target_point->integ - start_point) * target_point->dsor + target_point->num;
	if(num < 0)
		num = -num;
	float x = (float)num / target_point->dsor;
	float tlen = sqrtf(x * x + 0.25f);
	return tlen / len;
}

int32_t LinePrepare(vec4 *st, vec4 *en, void *st_data, void *en_data, int_float *target, void *data, void *data_step, pipe_op *op)
{
	vec4 dir;
	dir.xc = (en->xc - st->xc) << 1;
	dir.yc = (en->yc - st->yc) << 1;

	target->integ = st->xc;
	target->num = dir.xc >> 1;
	target->dsor = dir.yc;
	pmx(target, dir.xc);

	float t = get_t_param(&dir, target, st->xc);
	target->z = (en->z - st->z) * t;
	target->zstep = target->z * 2;
	target->z += st->z;

	op->lerp_functions.lerp_pol_data_get_delta_func(st_data, en_data, data_step);
	op->lerp_functions.lerp_pol_data_delta_mul_t_func(data_step, data, t);
	op->lerp_functions.lerp_pol_data_add_func(data, data, data_step);
	op->lerp_functions.lerp_pol_data_add_func(data, st_data, data);

	return dir.xc;
}

void LineWork(int_float *target, void *data, void *data_step, int32_t step, pipe_op *op)
{
	target->z += target->zstep;
	op->lerp_functions.lerp_pol_data_add_func(data, data_step, data);
	target->num += step;
	pmx(target, step);
}

void DrawLeftRightLow(pipe_polygon *pol, pipe_op *op)
{
	//int lnum, ldsor, rnum, rdsor, lint, rint;
	int_float left, right;

	void *left_data, *right_data, *left_data_step, *right_data_step;
	left_data = malloc(op->data_ceil_size);
	right_data = malloc(op->data_ceil_size);
	left_data_step = malloc(op->data_ceil_size);
	right_data_step = malloc(op->data_ceil_size);

	int32_t left_dir, right_dir;
	left_dir = LinePrepare(&pol->top, &pol->low, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &left, left_data, left_data_step, op);
	right_dir = LinePrepare(&pol->top, &pol->med, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, &right, right_data, right_data_step, op);

	while(pol->top.yc < pol->low.yc)
	{
		DrawInterval(pol->top.yc, left, right, left_data, right_data, op);
		pol->top.yc++;
		LineWork(&left, left_data, left_data_step, left_dir, op);
		LineWork(&right, right_data, right_data_step, right_dir, op);
	}

	free(left_data);
	free(right_data);
	free(left_data_step);
	free(right_data_step);
}

void DrawLeftTopRight(pipe_polygon *pol, pipe_op *op)
{
	//int lnum, ldsor, rnum, rdsor, lint, rint;
	int_float left, right;

	void *left_data, *right_data, *left_data_step, *right_data_step;
	left_data = malloc(op->data_ceil_size);
	right_data = malloc(op->data_ceil_size);
	left_data_step = malloc(op->data_ceil_size);
	right_data_step = malloc(op->data_ceil_size);

	int32_t left_dir, right_dir;
	left_dir = LinePrepare(&pol->top, &pol->low, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &left, left_data, left_data_step, op);
	right_dir = LinePrepare(&pol->med, &pol->low, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &right, right_data, right_data_step, op);

	while(pol->top.yc < pol->low.yc)
	{
		DrawInterval(pol->top.yc, left, right, left_data, right_data, op);
		pol->top.yc++;
		LineWork(&left, left_data, left_data_step, left_dir, op);
		LineWork(&right, right_data, right_data_step, right_dir, op);
	}

	free(left_data);
	free(right_data);
	free(left_data_step);
	free(right_data_step);
}

void DrawLeftRightRight(pipe_polygon *pol, pipe_op *op)
{
	int_float left, right;
	void *left_data, *right_data, *left_data_step, *right_data_step;
	left_data = malloc(op->data_ceil_size);
	right_data = malloc(op->data_ceil_size);
	left_data_step = malloc(op->data_ceil_size);
	right_data_step = malloc(op->data_ceil_size);

	int32_t left_dir, right_dir;
	left_dir = LinePrepare(&pol->top, &pol->low, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &left, left_data, left_data_step, op);
	right_dir = LinePrepare(&pol->top, &pol->med, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, &right, right_data, right_data_step, op);

	while(pol->top.yc < pol->med.yc)
	{
		DrawInterval(pol->top.yc, left, right, left_data, right_data, op);
		pol->top.yc++;
		LineWork(&left, left_data, left_data_step, left_dir, op);
		LineWork(&right, right_data, right_data_step, right_dir, op);
	}

	right_dir = LinePrepare(&pol->med, &pol->low, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &right, right_data, right_data_step, op);

	while(pol->med.yc < pol->low.yc)
	{
		DrawInterval(pol->med.yc, left, right, left_data, right_data, op);
		pol->med.yc++;
		LineWork(&left, left_data, left_data_step, left_dir, op);
		LineWork(&right, right_data, right_data_step, right_dir, op);
	}

	free(left_data);
	free(right_data);
	free(left_data_step);
	free(right_data_step);
}

void DrawLeftLeftRight(pipe_polygon *pol, pipe_op *op)
{
	int_float left, right;
	void *left_data, *right_data, *left_data_step, *right_data_step;
	left_data = malloc(op->data_ceil_size);
	right_data = malloc(op->data_ceil_size);
	left_data_step = malloc(op->data_ceil_size);
	right_data_step = malloc(op->data_ceil_size);

	int32_t left_dir, right_dir;
	left_dir = LinePrepare(&pol->top, &pol->med, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, &left, left_data, left_data_step, op);
	right_dir = LinePrepare(&pol->top, &pol->low, &pol->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &right, right_data, right_data_step, op);

	while(pol->top.yc < pol->med.yc)
	{
		DrawInterval(pol->top.yc, left, right, left_data, right_data, op);
		pol->top.yc++;
		LineWork(&left, left_data, left_data_step, left_dir, op);
		LineWork(&right, right_data, right_data_step, right_dir, op);
	}

	left_dir = LinePrepare(&pol->med, &pol->low, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &left, left_data, left_data_step, op);

	while(pol->med.yc < pol->low.yc)
	{
		DrawInterval(pol->med.yc, left, right, left_data, right_data, op);
		pol->med.yc++;
		LineWork(&left, left_data, left_data_step, left_dir, op);
		LineWork(&right, right_data, right_data_step, right_dir, op);
	}

	free(left_data);
	free(right_data);
	free(left_data_step);
	free(right_data_step);
}

#define TOP 1
#define LOW 10
#define LEFT 100
#define RIGHT 1000

/*define that v0 = top, v1 = med, v2 = low*/

int g = 0;

/*make EPS - the minimal range of the target (0.5f) where we need
to make floor */
#warning MAYBE FIND THE FORMULA WHERE WE CAN USE 'width' and 'height' like EPS-param!
#define EPS 0.0001f

void DrawPolygon(pipe_polygon *pol, pipe_op *op)
{
	int casenum = 0;

	pipe_polygon *sorted = NULL;

	float remainder;

	remainder = pol->v0.x - floor(pol->v0.x);
	if(remainder > (0.5f - EPS) && remainder < (0.5f + EPS)) pol->v0.xc = (int32_t)pol->v0.x;
	else pol->v0.xc = (int32_t)roundf(pol->v0.x);
	remainder = pol->v0.y - floor(pol->v0.y);
	if(remainder > (0.5f - EPS) && remainder < (0.5f + EPS)) pol->v0.yc = (int32_t)pol->v0.y;
	else pol->v0.yc = (int32_t)roundf(pol->v0.y);

	remainder = pol->v1.x - floor(pol->v1.x);
	if(remainder > (0.5f - EPS) && remainder < (0.5f + EPS)) pol->v1.xc = (int32_t)pol->v1.x;
	else pol->v1.xc = (int32_t)roundf(pol->v1.x);
	remainder = pol->v1.y - floor(pol->v1.y);
	if(remainder > (0.5f - EPS) && remainder < (0.5f + EPS)) pol->v1.yc = (int32_t)pol->v1.y;
	else pol->v1.yc = (int32_t)roundf(pol->v1.y);

	remainder = pol->v2.x - floor(pol->v2.x);
	if(remainder > (0.5f - EPS) && remainder < (0.5f + EPS)) pol->v2.xc = (int32_t)pol->v2.x;
	else pol->v2.xc = (int32_t)roundf(pol->v2.x);
	remainder = pol->v2.y - floor(pol->v2.y);
	if(remainder > (0.5f - EPS) && remainder < (0.5f + EPS)) pol->v2.yc = (int32_t)pol->v2.y;
	else pol->v2.yc = (int32_t)roundf(pol->v2.y);


	if((pol->v0.xc == pol->v1.xc && pol->v0.xc == pol->v2.xc) || (pol->v0.yc == pol->v1.yc && pol->v0.yc == pol->v2.yc)) return;

	sorted = pipe_polygon_alloc_data(op->data_ceil_size);

	if(pol->v0.yc == pol->v1.yc)
	{
		if(pol->v2.yc < pol->v0.yc)
		{
			casenum += LOW;
			sorted->top = pol->v2;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 0);
			if(pol->v0.xc < pol->v1.xc)
			{
				sorted->low = pol->v0;
				sorted->med = pol->v1;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 2);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 1);
			}
			else
			{
				sorted->low = pol->v1;
				sorted->med = pol->v0;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 2);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 1);
			}
		}
		else
		{
			casenum += TOP;
			sorted->low = pol->v2;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 2);
			if(pol->v0.xc < pol->v1.xc)
			{
				sorted->top = pol->v0;
				sorted->med = pol->v1;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 0);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 1);
			}
			else
			{
				sorted->top = pol->v1;
				sorted->med = pol->v0;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 0);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 1);
			}
		}
	}
	else if(pol->v0.yc == pol->v2.yc)
	{
		if(pol->v1.yc < pol->v0.yc)
		{
			casenum += LOW;
			sorted->top = pol->v1;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 0);
			if(pol->v0.xc < pol->v2.xc)
			{
				sorted->low = pol->v0;
				sorted->med = pol->v2;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 2);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 1);
			}
			else
			{
				sorted->low = pol->v2;
				sorted->med = pol->v0;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 2);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 1);
			}
		}
		else
		{
			casenum += TOP;
			sorted->low = pol->v1;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 2);
			if(pol->v0.xc < pol->v2.xc)
			{
				sorted->top = pol->v0;
				sorted->med = pol->v2;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 0);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 1);
			}
			else
			{
				sorted->top = pol->v2;
				sorted->med = pol->v0;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 0);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 1);
			}
		}
	}
	else if(pol->v2.yc == pol->v1.yc)
	{
		if(pol->v0.yc < pol->v1.yc)
		{
			casenum += LOW;
			sorted->top = pol->v0;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 0);
			if(pol->v2.xc < pol->v1.xc)
			{
				sorted->low = pol->v2;
				sorted->med = pol->v1;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 2);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 1);
			}
			else
			{
				sorted->low = pol->v1;
				sorted->med = pol->v2;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 2);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 1);
			}
		}
		else
		{
			casenum += TOP;
			sorted->low = pol->v0;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 2);
			if(pol->v2.xc < pol->v1.xc)
			{
				sorted->top = pol->v2;
				sorted->med = pol->v1;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 0);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 1);
			}
			else
			{
				sorted->top = pol->v1;
				sorted->med = pol->v2;
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 0);
				pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 1);
			}
		}
	}
	else if(pol->v0.yc < pol->v1.yc && pol->v0.yc < pol->v2.yc)
	{
		casenum += RIGHT + LEFT;
		sorted->top = pol->v0;
		pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 0);
		if(pol->v1.yc < pol->v2.yc)
		{
			sorted->med = pol->v1;
			sorted->low = pol->v2;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 1);
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 2);
		}
		else
		{
			sorted->med = pol->v2;
			sorted->low = pol->v1;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 1);
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 2);
		}
	}
	else if(pol->v1.yc < pol->v0.yc && pol->v1.yc < pol->v2.yc)
	{
		casenum += RIGHT + LEFT;
		sorted->top = pol->v1;
		pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 0);
		if(pol->v0.yc < pol->v2.yc)
		{
			sorted->med = pol->v0;
			sorted->low = pol->v2;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 1);
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 2);
		}
		else
		{
			sorted->med = pol->v2;
			sorted->low = pol->v0;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 1);
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 2);
		}
	}
	else
	{
		casenum += RIGHT + LEFT;
		sorted->top = pol->v2;
		pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 2, 0);
		if(pol->v0.yc < pol->v1.yc)
		{
			sorted->med = pol->v0;
			sorted->low = pol->v1;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 1);
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 2);
		}
		else
		{
			sorted->med = pol->v1;
			sorted->low = pol->v0;
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 1, 1);
			pipe_polygon_copy_attr(pol, sorted, op->data_ceil_size, 0, 2);
		}
	}

	switch(casenum)
	{
		case TOP: DrawLeftTopRight(sorted, op);
		break;
		case LOW: DrawLeftRightLow(sorted, op);
		break;
		case RIGHT + LEFT:
		int32_t x_xcd, x_ycd, y_xcd, y_ycd;
		int32_t z_dir;

		x_xcd = (sorted->med.xc - sorted->top.xc);
		x_ycd = (sorted->med.yc - sorted->top.yc);
		y_xcd = (sorted->low.xc - sorted->med.xc);
		y_ycd = (sorted->low.yc - sorted->med.yc);

		/*find like z-component of cross product
		of tm_dir(like x screenspace vector) and
		ml_dir(like y screenspace vector).
		And if we 'rotate' 'x' to 'y' we see
		on direction of rotation.
		Component z > 0 if we rotate with negative angle!*/
		z_dir = x_xcd * y_ycd - y_xcd * x_ycd;
		if(z_dir > 0)
			DrawLeftRightRight(sorted, op);
		else
			DrawLeftLeftRight(sorted, op);
		break;
	}

	pipe_polygon_free_alloc(sorted);
}
