#pragma once
#include "framebuffer.h"
#include "vec.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "pipe_op.h"
#include "pipe_pol.h"
#include "cull.h"

typedef struct
{
	int32_t integ, num, dsor;
	float z, zstep;
} int_float;

void SetTargetFB(framebuffer* fb);

//void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);

//void DrawLIneDepth(vec2i v0, vec2i v2);

#define ATTRIBUTES_SEPARATE_BUFFER 1
#define ATTRIBUTES_OVERLAP_AFTER_BUFFER 2
#define ATTRIBUTES_OVERLAP_BETWEEN_BUFFER 3

#warning ADD OTHER DEFS!
#warning ADD SEPARATE HEADER FOR PIPE_POLYGON!

void Draw(const void *vertexes, size_t count, float *pl, size_t pl_count, vec4 *lh, vec4 *rl, pipe_op *op, void *attributes, int attributes_type);

//Draw pipeline fucntions
static void cull3D(polygon_list *l, float *pl, size_t pl_count, pipe_op *op);

static void pol_min_max(pipe_polygon *pol, vec4 *min, vec4 *max);

static void cull2D(polygon_list *l, vec4 *lh, vec4 *rl, pipe_op *op);

static void ToScreenSpace(float *x, float *y);

static void pmx(int_float *line, int32_t step);

static float get_t_param(const vec4 *dir, int_float *target_point, int32_t start_point);

static int32_t LinePrepare(vec4 *st, vec4 *en, void *st_data, void *en_data, int_float *target, void *data, void *data_step, pipe_op *op);

static void LineWork(int_float *target, void *data, void *data_step, int32_t step, pipe_op *op);

//functions of choosed from DrawPolygon type of polygon
static void DrawLeftRightLow(pipe_polygon *pol, pipe_op *op);

static void DrawLeftTopRight(pipe_polygon *pol, pipe_op *op);

static void DrawLeftRightRight(pipe_polygon *pol, pipe_op *op);

static void DrawLeftLeftRight(pipe_polygon *pol, pipe_op *op);

static void DrawPolygon(pipe_polygon *pol, pipe_op *op);
