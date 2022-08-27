#pragma once

#include <stdint.h>

typedef struct
{
	union {float x; float r; int32_t xc;};
	union {float y; float g; int32_t yc;};
	union {float z; float b;};
	union {float w; float a;};
} vec4;

//vec4: float xyzw or rgba eq.
//vec3: float xyz or rgb eq.
//vec2: float xy or rg eq.
//vec2i: integer xc and xy
//vec2iz: integer xc and xy + float z

//vec4
void add_vec4(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v);

void sub_vec4(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v);

//vec3
void add_vec3(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v);

void sub_vec3(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v);

float dot_vec3(const vec4 *v0, const vec4 *v1);

float len_vec3(const vec4 *v);

void norm_vec3(vec4 *v);

void cross_vec3(const vec4 *a, const vec4 *b, vec4 *out_v);

//vec2
void add_vec2(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v);

void sub_vec2(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v);

float len_vec2(const vec4 *v);

//vec2i
void add_vec2i(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v);

void sub_vec2i(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v);

//vec2iz
void add_vec2iz(const vec4 *add_v0, const vec4 *add_v1, vec4 *out_v);

void sub_vec2iz(const vec4 *min_v, const vec4 *sub_v, vec4 *out_v);

//debug
void debug_print_vec4(const vec4 *v);
