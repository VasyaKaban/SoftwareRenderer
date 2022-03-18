#pragma once

typedef struct vec3_object
{
    float x, y, z;
} vec3;

void add_v(const vec3* v0, const vec3* v1, vec3* v);

void sub_v(const vec3* mv, const vec3* sv, vec3* v);

float dot(const vec3* v0, const vec3* v1);

float cos_bet(const vec3* v0, const vec3* v1);

float get_len(const vec3* v);

float get_len_xy(const vec3* v);

void normalize(vec3* v);

void cross(const vec3* a, const vec3* b, vec3* c);

