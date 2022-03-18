#pragma once
#include "framebuffer.h"
#include "vec3.h"
#include "stddef.h"

//extern framebuffer* TargetFB;

//extern float PerspX, PerspY;

void SetPixel(int x, int y, float z, uint32_t color);

void SetTargetFB(framebuffer* fb);

void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);

void SetPerspective(float far, float near, float aspect, float fov);

void Draw(const vec3* vertexes, size_t count, uint32_t* color);

void ToScreenSpace(float *x, float *y);

void DrawPolygon(const vec3* v0, const vec3* v1, const vec3* v2);
