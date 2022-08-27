#include "vec.h"
#include <math.h>

//vec4
void add_vec4(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v)
{
	out_v->x = add_v0->x + add_v1->x;
	out_v->y = add_v0->y + add_v1->y;
	out_v->z = add_v0->z + add_v1->z;
	out_v->w = add_v0->w + add_v1->w;
}

void sub_vec4(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v)
{
	out_v->x = min_v->x - sub_v->x;
	out_v->y = min_v->y - sub_v->y;
	out_v->z = min_v->z - sub_v->z;
	out_v->w = min_v->w - sub_v->w;
}

//vec3
void add_vec3(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v)
{
	out_v->x = add_v0->x + add_v1->x;
	out_v->y = add_v0->y + add_v1->y;
	out_v->z = add_v0->z + add_v1->z;
}

void sub_vec3(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v)
{
	out_v->x = min_v->x - sub_v->x;
	out_v->y = min_v->y - sub_v->y;
	out_v->z = min_v->z - sub_v->z;
}

float dot_vec3(const vec4 *v0, const vec4 *v1)
{
	return (v0->x * v1->x + v0->y * v1->y + v0->z * v1->z);
}

float len_vec3(const vec4 *v)
{
	return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

void norm_vec3(vec4 *v)
{
	float inv_len = 1.0f / sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x *= inv_len;
	v->y *= inv_len;
	v->z *= inv_len;
}

void cross_vec3(const vec4 *a, const vec4 *b, vec4 *out_v)
{
	vec4 v;
	v.x = a->y * b->z - b->y * a->z;
	v.y = b->x * a->z - a->x * b->z;
	v.z = a->x * b->y - b->x * a->y;
	*out_v = v;
}

//vec2
void add_vec2(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v)
{
	out_v->x = add_v0->x + add_v1->x;
	out_v->y = add_v0->y + add_v1->y;
}

void sub_vec2(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v)
{
	out_v->x = min_v->x - sub_v->x;
	out_v->y = min_v->y - sub_v->y;
}

float len_vec2(const vec4 *v)
{
	return sqrtf(v->x * v->x + v->y * v->y);
}

//vec2i
void add_vec2i(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v)
{
	out_v->xc = add_v0->xc + add_v1->xc;
	out_v->yc = add_v0->yc + add_v1->yc;
}

void sub_vec2i(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v)
{
	out_v->xc = min_v->xc - sub_v->xc;
	out_v->yc = min_v->yc - sub_v->yc;
}

//vec2iz
void add_vec2iz(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v)
{
	out_v->xc = add_v0->xc + add_v1->xc;
	out_v->yc = add_v0->yc + add_v1->yc;
	out_v->z = add_v0->z + add_v1->z;
}

void sub_vec2iz(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v)
{
	out_v->xc = min_v->xc - sub_v->xc;
	out_v->yc = min_v->yc - sub_v->yc;
	out_v->z = min_v->z - sub_v->z;
}

#include <stdio.h>
//debug
void debug_print_vec4(const vec4 *v)
{
	printf("x = %f y = %f z = %f w = %f xc = %d yc = %d\n", v->x, v->y, v->z, v->w, v->xc, v->yc);
}
