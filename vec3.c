#include "vec3.h"
#include "math.h"

void add_v(const vec3* v0, const vec3* v1, vec3* v)
{
    v->x = v0->x + v1->x;
    v->y = v0->y + v1->y;
    v->z = v0->z + v1->z;
}

void sub_v(const vec3* mv, const vec3* sv, vec3* v)
{
    v->x = mv->x - sv->x;
    v->y = mv->y - sv->y;
    v->z = mv->z - sv->z;
}

float dot(const vec3* v0, const vec3* v1)
{
    return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

float cos_bet(const vec3* v0, const vec3* v1)
{
    return (v0->x * v1->x + v0->y * v1->y + v0->z * v1->z) / (get_len(v0) * get_len(v1));
}

float get_len(const vec3* v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

float get_len_xy(const vec3* v)
{
    return sqrtf(v->x * v->x + v->y * v->y);
}

void normalize(vec3* v)
{
    float inv_len = 1.0f / get_len(v);
    v->x *= inv_len;
    v->y *= inv_len;
    v->z *= inv_len;
}

void cross(const vec3* a, const vec3* b, vec3* c)
{
    c->x = a->y * b->z - a->z * b->y;
    c->y = -a->x * b->z + a->z * b->x;
    c->z = a->x * b->y - a->y * b->x;
}

