#include <SDL2/SDL.h>
#include <stdio.h>
#include "framebuffer.h"
#include "draw.h"
#include "vec3.h"
#include <time.h>

int w = 1600;
int h = 900;

int win_is_close = 0;

int mx = 400, my = 300;


vec3 vertexes[] = {
//    1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
//	-1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f,
////    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
////    1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
//'		-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
//'		1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 2.0f, 2.0f, -1.0f
//		-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
//		1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 3.0f, 1.0f, -1.0f,
//		3.0f, 1.0f, -1.0f, 5.0f, -1.0f, -1.0f ,1.0f, -1.0f, -1.0f
//    1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
  //  -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
   // -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
   // -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
//    -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
  //  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
  //  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
   // 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f*/
//	0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
//	0.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, -1.0f, 1.0f

	/*-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, -1.0f
	0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f
	-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f
	0.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f
	-2.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, -1.0f,
	0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -2.0f, -1.0f, -1.0f,
	-2.0f, 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, -2.0f, -1.0f, -1.0f,
	0.0f, 1.0f, -1.0f, 2.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f,
	1.0f, 0.0f, -1.0f, 1.0f, -1.0f, -1.0f, -2.0f, -1.0f, -1.0f*/

/*	0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f,
	1.0f, 0.0f, -1.0f,3.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
	0.0f, 1.0f, -1.0f, 3.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f,
	-2.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 0.0f, -2.0f, -1.0f,
	3.0f, -1.0f, -1.0f, 4.0f, -1.5f, -1.0f, 0.0f, -2.0f, -1.0f,
	1.0f, 0.0f, -1.0f, 3.0f, 1.0f, -1.0f, 3.0f, -1.0f, -1.0f,
//	3.0f, 1.0f, -1.0f, 4.0f, -1.5f, -1.0f, 3.0f, -1.0f, -1.0f*/
//	-0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f,
//	-2.0f, 0.0f, 2.0f, 0.0f, 2.0f, 2.0f, 2.0f, 0.0f, 2.0f
//	-1.0f, -1.5f, 2.0f, 0.0f, 0.5f, 2.0f, 1.0f, -1.5f, 2.0f,
//	-1.0f, 0.0f, 3.0f, 1.0f, 0.0f, 3.0f, 0.0f, -1.0f, 1.0f

	-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,//*
	1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f,//*
	-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f

//	-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -2.0f, 1.0f, -1.0f, -1.0f
};

uint32_t colors[] = {
	0xFF000000, 0x00FF0000, 0x0000FF00, 0xFFFF0000,
	0xFF00FF00, 0x00FFFF00, 0xFFFFFF00, 0x76A0B700,
	0xFF8C0000, 0x6A5ACD00, 0xBC8F8F00, 0x00FA9A00
};

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;
framebuffer fb;

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
		    SDL_DestroyTexture(texture);
		    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
		    ResizeFrameBuffer(&fb, w, h);
		    SDL_UpdateTexture(texture, NULL, fb.colorb, w * sizeof(uint32_t));
		    SetPerspective(10.0f, 2.0f, (float)w / h, 90.0f);
		}
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&mx, &my);
				break;
			//case SDL_MOUSEBUTTONDOWN:
			//	for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].z += 0.1f;
				//printf("%f\n", vertexes[0].z);
				//printf("%f\n", TargetFB->depthb[my * w + mx]);
			//	break;
			case SDL_MOUSEWHEEL:
				if(event.wheel.y > 0) for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].z += 0.1f;
				if(event.wheel.y < 0) for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].z -= 0.1f;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_w) for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].y += 0.1f;
				else if(event.key.keysym.sym == SDLK_s) for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].y -= 0.1f;
				else if(event.key.keysym.sym == SDLK_d) for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].x += 0.1f;
				else if(event.key.keysym.sym == SDLK_a) for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++) vertexes[i].x -= 0.1f;
				break;
        }
    }
}

int main(int argc, char** argv)
{
    for(int i = 0; i < sizeof(vertexes) / sizeof(vec3); i++)
    {
        vertexes[i].z += 5;
//		vertexes[i].x -= 3;
//		vertexes[i].y -= 2;
    }

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0)
    {
        printf("ERROR! SDL not init!");
        return 0;
    }

    window = SDL_CreateWindow("pipe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_RESIZABLE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);

    InitFramebuffer(&fb, w, h);
    ClearColorBuffer(&fb, 0x00000000);
    SetTargetFB(&fb);
    ClearDepthBuffer(&fb, 10.0f);
    SetPerspective(10.0f, 2.0f, (float)w / h, 90.0f);

	clock_t st;

    while(!win_is_close)
    {
		st = clock();
        ClearColorBuffer(&fb, 0x00000000);
	    ClearDepthBuffer(&fb, 10.0f);
        Draw(vertexes, sizeof(vertexes) / sizeof(vec3), colors);
        SDL_UpdateTexture(texture, NULL, fb.colorb, w * sizeof(uint32_t));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        EventPoll();
//		printf("%lld\n", clock() - st);
    }

    DeleteFramebuffer(&fb);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
