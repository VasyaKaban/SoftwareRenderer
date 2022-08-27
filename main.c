#include <SDL2/SDL.h>
#include <stdio.h>
#include "framebuffer.h"
#include "draw.h"
#include "vec.h"
#include <time.h>
#include "pipe_op.h"

int w = 800;
int h = 600;

int win_is_close = 0;

int mx = 400, my = 300;

float r_angle = 0.0f;

int is_rot = 0;

vec4 w_axis = {1.0f / sqrtf(2.0f), 1.0f / sqrtf(2.0f), 0.0f, 0.0f};

vec4 translate = {0.0f, 0.0f, 0.0f, 0.0f};

//pipe_op
struct color_op_loadable_data
{
	float near, far, top, right;
};

struct color_op_loadable_data persp_param;

void color_lerp_get_delta(const void *c0, const void *c1, void *out_c)
{
	sub_vec4((vec4*)c1, (vec4*)c0, (vec4*)out_c);
}

void color_lerp_delta_t_mul(const void *dc, void *out_c, float t)
{
	((vec4*)out_c)->r = ((vec4*)dc)->r * t;
	((vec4*)out_c)->g = ((vec4*)dc)->g * t;
	((vec4*)out_c)->b = ((vec4*)dc)->b * t;
	((vec4*)out_c)->a = ((vec4*)dc)->a * t;
}

void color_lerp_add(const void *c0, const void *c1, void *out_c)
{
	add_vec4((vec4*)c0, (vec4*)c1, (vec4*)out_c);
}

void color_lerp(const void *c0, const void *c1, float t, void *out_c)
{
	vec4 dc;
	sub_vec4((vec4*)c1, (vec4*)c0, &dc);
	dc.r *= t;
	dc.g *= t;
	dc.b *= t;
	dc.a *= t;
	add_vec4(&dc, (vec4*)c0, (vec4*)out_c);
}

void rotate(vec4 axis, float angle, vec4 *v)
{
	float proj = dot_vec3(v, &axis);
	vec4 a = {axis.x * proj, axis.y * proj, axis.z * proj, 0.0f};
	vec4 b = {v->x - a.x, v->y - a.y, v->z - a.z, 0.0f};
	cross_vec3(&b, &axis, &b);
	v->x = v->x * cosf(M_PI / 180.0f * angle) + b.x * sinf(M_PI / 180.0f * angle) + (1.0f - cosf(M_PI / 180.0f * angle)) * a.x;
	v->y = v->y * cosf(M_PI / 180.0f * angle) + b.y * sinf(M_PI / 180.0f * angle) + (1.0f - cosf(M_PI / 180.0f * angle)) * a.y;
	v->z = v->z * cosf(M_PI / 180.0f * angle) + b.z * sinf(M_PI / 180.0f * angle) + (1.0f - cosf(M_PI / 180.0f * angle)) * a.z;
}

int color_polygon_op(vec4 *v0, vec4 *v1, vec4 *v2, void *v0_data, void *v1_data, void *v2_data, void *loadable_data)
{
	rotate(w_axis, r_angle, v0);
	rotate(w_axis, r_angle, v1);
	rotate(w_axis, r_angle, v2);

	v0->x += translate.x;
	v0->y += translate.y;
	v0->z += translate.z;

	v1->x += translate.x;
	v1->y += translate.y;
	v1->z += translate.z;

	v2->x += translate.x;
	v2->y += translate.y;
	v2->z += translate.z;

	return POLYGON_CONTINUE;
}

void color_post_cull(vec4 *v0, vec4 *v1, vec4 *v2, void *v0_data, void *v1_data, void *v2_data, void *load_data)
{
	struct color_op_loadable_data *ptr = load_data;

	v0->x = (v0->x * ptr->near / ptr->right) / v0->z;
	v0->y = (v0->y * ptr->near / ptr->top) / v0->z;
	v0->z = ptr->near / v0->z;

	v1->x = (v1->x * ptr->near / ptr->right) / v1->z;
	v1->y = (v1->y * ptr->near / ptr->top) / v1->z;
	v1->z = ptr->near / v1->z;

	v2->x = (v2->x * ptr->near / ptr->right) / v2->z;
	v2->y = (v2->y * ptr->near / ptr->top) / v2->z;
	v2->z = ptr->near / v2->z;
	#warning ADD COLOR MUL AND DIV!
}

void color_pixel_op(framebuffer *fb, size_t pixels_count, vec4 *start_pixel, float zstep, void *start_data, void *data_step)
{
	vec4 *colori = start_data;
	uint32_t color;
	uint32_t pos = get_pos(start_pixel, fb);
	for(uint32_t i = 0; i < pixels_count; i++)
	{
		if(get_pixel_fb_z(pos, fb) < start_pixel->z)
		{
			color = ((uint32_t)colori->a) << 24 | ((uint32_t)colori->r) << 16 | ((uint32_t)colori->g) << 8 | ((uint32_t)colori->b);
			set_pixel_into_fb(pos, fb, start_pixel->z, color);
		}

		start_pixel->z += zstep;
		start_pixel->xc++;
		add_vec4(colori, data_step, colori);
		pos++;
	}
}

pipe_op operation_color;
//~pipe_op


vec4 vertexes_colors[] = {
	-1.0f, 1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,

	1.0f, -1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,

	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,
	255.0f, 255.0f, 255.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,

	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	255.0f, 255.0f, 255.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,
	128.0f, 0.0f, 128.0f, 0.0f,

	-1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	128.0f, 0.0f, 128.0f, 0.0f,
	255.0f, 165.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 255.0f, 0.0f,

	-1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	255.0f, 165.0f, 0.0f, 0.0f,
	128.0f, 0.0f, 128.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,

	-1.0f, -1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, -1.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,
	128.0f, 0.0f, 128.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,

	-1.0f, 1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,
	128.0f, 0.0f, 128.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,

	1.0f, 1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	128.0f, 0.0f, 128.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,

	-1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 0.0f,
	255.0f, 165.0f, 0.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,

	1.0f, -1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 255.0f, 0.0f,
	255.0f, 165.0f, 0.0f, 0.0f,

//one color per side
	/*-1.0f, 1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,

	1.0f, 1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,
	255.0f, 0.0f, 0.0f, 0.0f,

	1.0f, -1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,

	1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,
	0.0f, 255.0f, 0.0f, 0.0f,

	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,

	-1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,
	0.0f, 0.0f, 255.0f, 0.0f,

	-1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	255.0f, 0.0f, 255.0f, 0.0f,
	255.0f, 0.0f, 255.0f, 0.0f,
	255.0f, 0.0f, 255.0f, 0.0f,

	-1.0f, -1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, -1.0f, 0.0f,
	255.0f, 0.0f, 255.0f, 0.0f,
	255.0f, 0.0f, 255.0f, 0.0f,
	255.0f, 0.0f, 255.0f, 0.0f,

	-1.0f, 1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, -1.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,

	1.0f, 1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,
	255.0f, 255.0f, 0.0f, 0.0f,

	-1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, -1.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,

	1.0f, -1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 1.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f,
	0.0f, 255.0f, 255.0f, 0.0f*/
};


SDL_Window *window;
SDL_Surface *surface;
framebuffer fb;

//near, far
float frustum[8];
float g_top, g_right;
vec4 lh, rl;

void UpdateFrustum(float fov, float near, float far, float aspect)
{
	g_top = near * tan((fov / 2)  * M_PI / 180);
	g_right = aspect * g_top;
	//near
	frustum[0] = 0.0f;
	frustum[1] = 0.0f;
	frustum[2] = 1.0f;
	frustum[3] = -near;
	//far
	frustum[4] = 0.0f;
	frustum[5] = 0.0f;
	frustum[6] = -1.0f;
	frustum[7] = far;
}

void SetPerspective(float far, float near, float aspect, float fov)
{
	persp_param.top = near * tan((fov / 2) * M_PI / 180);
	persp_param.right = persp_param.top * aspect;
	persp_param.near = near;
	persp_param.far = far;
}

void EventPoll()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_CLOSE)
                    win_is_close = 1;
		if(event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
		    SDL_GetWindowSize(window, &w, &h);
			surface = SDL_GetWindowSurface(window);
		    ResizeFrameBuffer(&fb, w, h);
			memcpy(surface->pixels, fb.colorb, fb.w * fb.h * sizeof(uint32_t));
		    SetPerspective(100.0f, 2.0f, (float)w / h, 45.0f);
			UpdateFrustum(45.0f, 2.0f, 100.0f, (float)w / h);
		}
                break;
            case SDL_MOUSEMOTION:
                //SDL_GetMouseState(&mx, &my);
				//printf("x = %d y = %d\n", mx, my);
				//printf("a = %d r = %d g = %d b = %d\n", fb.colorb[my * w + mx] >> 24, (fb.colorb[my * w + mx] << 8) >> 24, (fb.colorb[my * w + mx] << 16) >> 24, (fb.colorb[my * w + mx] << 24) >> 24);
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_w) translate.y += 0.01f;
				else if(event.key.keysym.sym == SDLK_s) translate.y -= 0.01f;
				else if(event.key.keysym.sym == SDLK_d) translate.x += 0.01f;
				else if(event.key.keysym.sym == SDLK_a) translate.x -= 0.01f;
				else if(event.key.keysym.sym == SDLK_u) translate.z += 0.01f;
				else if(event.key.keysym.sym == SDLK_l) translate.z -= 0.01f;
				else if(event.key.keysym.sym == SDLK_SPACE) is_rot = (is_rot ? 0 : 1);
				else if(event.key.keysym.sym == SDLK_q) win_is_close = 1;
				break;
        }
    }
}

int main(int argc, char** argv)
{
	operation_color.lerp_functions.lerp_pol_data_get_delta_func = color_lerp_get_delta;
	operation_color.lerp_functions.lerp_pol_data_delta_mul_t_func = color_lerp_delta_t_mul;
	operation_color.lerp_functions.lerp_pol_data_add_func = color_lerp_add;
	operation_color.lerp_functions.lerp_pol_data = color_lerp;
	operation_color.polygon_pipe_op = color_polygon_op;
	operation_color.post_3Dcull_func = color_post_cull;
	operation_color.pixel_pipe_op = color_pixel_op;
	operation_color.data_ceil_size = sizeof(vec4);
#warning MAYBE MAKE LIKE COPY TO THE IN-LIB BUFER?
	operation_color.loadable_data = &persp_param;

	norm_vec3(&w_axis);

	translate.z += 4.5f;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0)
    {
        printf("ERROR! SDL not init!");
        return 0;
    }

    window = SDL_CreateWindow("pipe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_RESIZABLE);

	surface = SDL_GetWindowSurface(window);

    InitFramebuffer(&fb, w, h);
    ClearColorBuffer(&fb, 0x00000000);
    SetTargetFB(&fb);
    ClearDepthBuffer(&fb, 2.0f / 100.0f);
    SetPerspective(100.0f, 2.0f, (float)w / h, 45.0f);
	UpdateFrustum(45.0f, 2.0f, 100.0f, (float)w / h);

	/*lh.x = -0.5f;
	lh.y = 0.5f;
	rl.x = 0.5f;
	rl.y = -0.5f;*/

	lh.x = -1.0f;
	lh.y = 1.0f;
	rl.x = 1.0f;
	rl.y = -1.0f;


	clock_t st, en;
	double fps = 0.0;
	double sec_count = 0.0;
	double fps_sum = 0.0;
	int count = 0;
	double sec = 0.0;

    while(!win_is_close)
    {
		st = clock();
        ClearColorBuffer(&fb, 0x00000000);
	    ClearDepthBuffer(&fb, 2.0f / 100.0f);
        Draw(vertexes_colors, sizeof(vertexes_colors) / sizeof(vec4), frustum, sizeof(frustum) / (sizeof(float) * 4), &lh, &rl, &operation_color, NULL, ATTRIBUTES_OVERLAP_AFTER_BUFFER);
		memcpy(surface->pixels, fb.colorb, fb.w * fb.h * sizeof(uint32_t));
		SDL_UpdateWindowSurface(window);
        EventPoll();
		en = clock();
		sec = ((double)(en - st) / CLOCKS_PER_SEC);
		sec_count += sec;
		fps = 1.0 / sec;

		if(is_rot)
			r_angle += 100.f / fps;

		if(sec_count >= 1.0)
		{
			sec_count = 0.0;
			printf("avg_fps = %lf\n", fps_sum / count);
		}
		else
		{
			fps_sum += fps;
			++count;
		}
    }

    DeleteFramebuffer(&fb);
    SDL_DestroyWindow(window);

    return 0;
}
