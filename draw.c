#include "draw.h"
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//#define FLOAT_LINE

typedef struct
{
    int x, y;
    float z;
} vec2i;

uint32_t tcolor = 0x0;

framebuffer* TargetFB = NULL;

float PerspX = 0.0f, PerspY = 0.0f;

float Pfar = 100.0f, Pnear = 1.0f;

void SetPixel(int x, int y, float z, uint32_t color)
{
    //if(x >= 0 && x < TargetFB->w && y >= 0 && y < TargetFB->h)
	//{
		if(/*z > 1.0f &&*/ z < TargetFB->depthb[y * TargetFB->w + x])
		{
			TargetFB->colorb[y * TargetFB->w + x] = color;
			TargetFB->depthb[y * TargetFB->w + x] = z;
		}
		//else printf("%f\n", z);
		/*if(TargetFB->colorb[y * TargetFB->w + x] != 0x0)
		{
			color = 0xFFFFFFFF;
			TargetFB->colorb[y * TargetFB->w + x] = color;
		}*/
	//}
	//else printf("Outside!\n");
}

void SetTargetFB(framebuffer* fb)
{
    TargetFB = fb;
}

void DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
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
}

void DrawLineDepth(vec2i v0, vec2i v1)
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
}

void SetPerspective(float far, float near, float aspect, float fov)
{
    float top = near * tan((fov / 2) * M_PI / 180);
    float right = top * aspect;
    PerspX = near / right;
    PerspY = near / top;
	Pfar = far;
	Pnear = near;
}

void PlaneTest(vec3 v0, vec3 v1, vec3 v2, uint8_t tnum);

void Draw(const vec3* vertexes, size_t count, uint32_t* color)
{
    //vec3 vec0, vec1, vec2/*, norm, v1, v2, point*/;
    for(size_t i = 0; i < count; i+= 3)
    {
        //vec0 = vertexes[i];
		//vec1 = vertexes[i + 1];
		//vec2 = vertexes[i + 2];
		/*sub_v(&vec1, &vec0, &v1);
		sub_v(&vec2, &vec0, &v2);
		cross(&v1, &v2, &norm);
		if(cos_bet(&vec0, &norm) >= 0.0f)
		{
			v1 = vec1;
			vec1 = vec2;
			vec2 = v1;
		}*/
		//if(vec0.z < 1.0f || vec1.z < 1.0f || vec2.z < 1.0f) continue;
        //vec0.x = vec0.x * PerspX / vec0.z;
        //vec0.y = vec0.y * PerspY / vec0.z;
		//if(vec0.x < -1.0f || vec0.x > 1.0f || vec0.y < -1.0f || vec0.y > 1.0f) continue;
        //ToScreenSpace(&vec0.x, &vec0.y);
        //vec1.x = vec1.x * PerspX / vec1.z;
        //vec1.y = vec1.y * PerspY / vec1.z;
		//if(vec1.x < -1.0f || vec1.x > 1.0f || vec1.y < -1.0f || vec1.y > 1.0f) continue;
        //ToScreenSpace(&vec1.x, &vec1.y);
        //vec2.x = vec2.x *  PerspX / vec2.z;
        //vec2.y = vec2.y * PerspY / vec2.z;
		//if(vec2.x < -1.0f || vec2.x > 1.0f || vec2.y < -1.0f || vec2.y > 1.0f) continue;
        //ToScreenSpace(&vec2.x, &vec2.y);
		tcolor = color[i / 3];
		PlaneTest(vertexes[i], vertexes[i + 1], vertexes[i + 2], 0);
        //DrawPolygon(&vec0, &vec1, &vec2);
    }
}

#define X 1
#define Y 2
#define Z 3

void LineIntersect(vec3 *st, vec3 *en, float p, int mode, vec3* out)
{
	float t;
	vec3 dt;
	sub_v(en, st, &dt);
	switch(mode)
	{
		case X:
			t = (p - st->x) / dt.x;
			out->x = p;
			out->y = dt.y * t + st->y;
			out->z = dt.z * t + st->z;
			break;
		case Y:
			t = (p - st->y) / dt.y;
			out->x = dt.x * t + st->x;;
			out->y = p;
			out->z = dt.z * t + st->z;
			break;
		case Z:
			t = (p - st->z) / dt.z;
			out->x = dt.x * t + st->x;
			out->y = dt.y * t + st->y;
			out->z = p;
			break;
	}
}

#define IN_1 1
#define IN_2 2

void PlaneIntersect(vec3 out, vec3 outin, vec3 in, uint8_t tnum, int mode)
{
	if(tnum == 0)//far
	{
		tnum++;
		if(mode == IN_1)
		{
			LineIntersect(&in, &out, Pfar, Z, &out);
			LineIntersect(&in, &outin, Pfar, Z, &outin);
			/*t = (Pfar - in.z) / (outin.z - in.z);
			sub_v(&outin, &in, &outin);
			outin.x *= t;
			outin.y *= t;
			outin.z *= t;
			add_v(&outin, &in, &outin);
			t = (Pfar - in.z) / (out.z - in.z);
			sub_v(&out, &in, &out);
			out.x *= t;
			out.y *= t;
			out.z *= t;
			add_v(&out, &in, &out);*/
			PlaneTest(in, outin, out, tnum);
		}
		else
		{
			vec3 hv, lv;
			LineIntersect(&in, &out, Pfar, Z, &hv);
			LineIntersect(&outin, &out, Pfar, Z, &lv);
			/*t = (Pfar - out.z) / (in.z - out.z);
			sub_v(&in, &out, &hv);
			hv.x *= t;
			hv.y *= t;
			hv.z *= t;
			add_v(&hv, &out, &hv);
			t = (Pfar - out.z) / (outin.z - out.z);
			sub_v(&outin, &out, &lv);
			lv.x *= t;
			lv.y *= t;
			lv.z *= t;
			add_v(&lv, &out, &lv);*/
			PlaneTest(outin, in, hv, tnum);
			PlaneTest(outin, hv, lv, tnum);
		}
	}
	else if(tnum == 1)//near
	{
		tnum += 2;
		if(mode == IN_1)
		{
			LineIntersect(&in, &out, Pnear, Z, &out);
			LineIntersect(&in, &outin, Pnear, Z, &outin);
			/*t = (Pnear - in.z) / (outin.z - in.z);
			sub_v(&outin, &in, &outin);
			outin.x *= t;
			outin.y *= t;
			outin.z *= t;
			add_v(&outin, &in, &outin);
			t = (Pnear - in.z) / (out.z - in.z);
			sub_v(&out, &in, &out);
			out.x *= t;
			out.y *= t;
			out.z *= t;
			add_v(&out, &in, &out);*/
			PlaneTest(in, outin, out, tnum);
		}
		else
		{
			vec3 hv, lv;
			LineIntersect(&in, &out, Pnear, Z, &hv);
			LineIntersect(&outin, &out, Pnear, Z, &lv);
			/*t = (Pnear - out.z) / (in.z - out.z);
			sub_v(&in, &out, &hv);
			hv.x *= t;
			hv.y *= t;
			hv.z *= t;
			add_v(&hv, &out, &hv);
			t = (Pnear - out.z) / (outin.z - out.z);
			sub_v(&outin, &out, &lv);
			lv.x *= t;
			lv.y *= t;
			lv.z *= t;
			add_v(&lv, &out, &lv);*/
			PlaneTest(outin, in, hv, tnum);
			PlaneTest(outin, hv, lv, tnum);
		}
	}
	else if((tnum & 4) == 0)//left test
	{
		tnum |= 4;
		if(mode == IN_1)
		{
			LineIntersect(&in, &out, -1.0f, X, &out);
			LineIntersect(&in, &outin, -1.0f, X, &outin);
			PlaneTest(in, outin, out, tnum);
		}
		else
		{
			vec3 hv, lv;
			LineIntersect(&in, &out, -1.0f, X, &hv);
			LineIntersect(&outin, &out, -1.0f, X, &lv);
			PlaneTest(outin, in, hv, tnum);
			PlaneTest(outin, hv, lv, tnum);
		}
	}
	else if((tnum & 8) == 0)//top test
	{
		tnum |= 8;
		if(mode == IN_1)
		{
			LineIntersect(&in, &out, 1.0f, Y, &out);
			LineIntersect(&in, &outin, 1.0f, Y, &outin);
			PlaneTest(in, outin, out, tnum);
		}
		else
		{
			vec3 hv, lv;
			LineIntersect(&in, &out, 1.0f, Y, &hv);
			LineIntersect(&outin, &out, 1.0f, Y, &lv);
			PlaneTest(outin, in, hv, tnum);
			PlaneTest(outin, hv, lv, tnum);
		}
	}
	else if((tnum & 16) == 0)//right test
	{
		tnum += 16;
		if(mode == IN_1)
		{
			LineIntersect(&in, &out, 1.0f, X, &out);
			LineIntersect(&in, &outin, 1.0f, X, &outin);
			PlaneTest(in, outin, out, tnum);
		}
		else
		{
			vec3 hv, lv;
			LineIntersect(&in, &out, 1.0f, X, &hv);
			LineIntersect(&outin, &out, 1.0f, X, &lv);
			PlaneTest(outin, in, hv, tnum);
			PlaneTest(outin, hv, lv, tnum);
		}
	}
	else if((tnum & 32) == 0)//low test
	{
		tnum |= 32;
		if(mode == IN_1)
		{
			LineIntersect(&in, &out, -1.0f, Y, &out);
			LineIntersect(&in, &outin, -1.0f, Y, &outin);
			PlaneTest(in, outin, out, tnum);
		}
		else
		{
			vec3 hv, lv;
			LineIntersect(&in, &out, -1.0f, Y, &hv);
			LineIntersect(&outin, &out, -1.0f, Y, &lv);
			PlaneTest(outin, in, hv, tnum);
			PlaneTest(outin, hv, lv, tnum);
		}
	}
}

void PlaneTest(vec3 v0, vec3 v1, vec3 v2, uint8_t tnum)
{
	if(tnum == 0)//far
	{
		if(v0.z > Pfar && v1.z > Pfar && v2.z > Pfar) return;
		else if(v0.z < Pfar && v1.z < Pfar && v2.z < Pfar)
		{
			tnum++;
			PlaneTest(v0, v1, v2, tnum);
		}
		else if(v0.z < Pfar && v1.z > Pfar && v2.z > Pfar)
		{
			PlaneIntersect(v1, v2, v0, tnum, IN_1);
		}
		else if(v2.z < Pfar && v1.z > Pfar && v0.z > Pfar)
		{
			PlaneIntersect(v1, v0, v2, tnum, IN_1);
		}
		else if(v1.z < Pfar && v0.z > Pfar && v2.z > Pfar)
		{
			PlaneIntersect(v0, v2, v1, tnum, IN_1);
		}
		else if(v0.z > Pfar && v1.z < Pfar && v2.z < Pfar)
		{
			PlaneIntersect(v0, v1, v2, tnum, IN_2);
		}
		else if(v2.z > Pfar && v1.z < Pfar && v0.z < Pfar)
		{
			PlaneIntersect(v2, v0, v1, tnum, IN_2);
		}
		else /*if(v1.z > Pfar && v2.z < Pfar && v0.z < Pfar)*/
		{
			PlaneIntersect(v1, v0, v2, tnum, IN_2);
		}
	}
	else if(tnum == 1)//near
	{
		if(v0.z < Pnear && v1.z < Pnear && v2.z < Pnear) return;
		else if(v0.z > Pnear && v1.z > Pnear && v2.z > Pnear)
		{
			tnum += 2;
			PlaneTest(v0, v1, v2, tnum);
		}
		else if(v0.z > Pnear && v1.z < Pnear && v2.z < Pnear)
		{
			PlaneIntersect(v1, v2, v0, tnum, IN_1);
		}
		else if(v2.z > Pnear && v1.z < Pnear && v0.z < Pnear)
		{
			PlaneIntersect(v1, v0, v2, tnum, IN_1);
		}
		else if(v1.z > Pnear && v0.z < Pnear && v2.z < Pnear)
		{
			PlaneIntersect(v0, v2, v1, tnum, IN_1);
		}
		else if(v0.z < Pnear && v1.z > Pnear && v2.z > Pnear)
		{
			PlaneIntersect(v0, v1, v2, tnum, IN_2);
		}
		else if(v2.z < Pnear && v1.z > Pnear && v0.z > Pnear)
		{
			PlaneIntersect(v2, v0, v1, tnum, IN_2);
		}
		else /*if(v1.z > Pfar && v2.z < Pfar && v0.z < Pfar)*/
		{
			PlaneIntersect(v1, v0, v2, tnum, IN_2);
		}
	}
	else//BB test
	{
		if(tnum == 3)
		{
			v0.x = v0.x * PerspX / v0.z;
			v0.y = v0.y * PerspY / v0.z;
			//ToScreenSpace(&v0.x, &v0.y);
			v1.x = v1.x * PerspX / v1.z;
			v1.y = v1.y * PerspY / v1.z;
			//ToScreenSpace(&v1.x, &v1.y);
			v2.x = v2.x * PerspX / v2.z;
			v2.y = v2.y * PerspY / v2.z;
			//ToScreenSpace(&v2.x, &v2.y);
		}
		//DrawPolygon(&v0, &v1, &v2);
		float minX, minY, maxX, maxY;
		minX = maxX = v0.x;
		minY = maxY = v0.y;
		if(minX > v1.x) minX = v1.x;
		else if(maxX < v1.x) maxX = v1.x;
		if(minY > v1.y) minY = v1.y;
		else if(maxY < v1.y) maxY = v1.y;

		if(minX > v2.x) minX = v2.x;
		else if(maxX < v2.x) maxX = v2.x;
		if(minY > v2.y) minY = v2.y;
		else if(maxY < v2.y) maxY = v2.y;

		if(minX > 1.0f || maxX < -1.0f || minY > 1.0f || maxY < -1.0f) return;

		if(tnum == 3)
		{
			if(minX > -1.0f) tnum |= 4;
			if(maxY < 1.0f) tnum |= 8;
			if(maxX < 1.0f) tnum |= 16;
			if(minY > -1.0f) tnum |= 32;
		}

		if(minX >= -1.0f && maxX <= 1.0f && minY >= -1.0f && maxY <= 1.0f)
		{
			ToScreenSpace(&v0.x, &v0.y);
			ToScreenSpace(&v1.x, &v1.y);
			ToScreenSpace(&v2.x, &v2.y);
			DrawPolygon(&v0, &v1, &v2);
		}
		else if(minX < -1.0f)//left
		{
			if(v0.x > -1.0f && v1.x < -1.0f && v2.x < -1.0f)
				PlaneIntersect(v1, v2, v0, tnum, IN_1);
			else if(v1.x > -1.0f && v0.x < -1.0f && v2.x < -1.0f)
				PlaneIntersect(v0, v2, v1, tnum, IN_1);
			else if(v2.x > -1.0f && v0.x < -1.0f && v1.x < -1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_1);
			else if(v0.x < -1.0f && v1.x > -1.0f && v2.x > -1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_2);
			else if(v1.x < -1.0f && v0.x > -1.0f && v2.x > -1.0f)
				PlaneIntersect(v1, v0, v2, tnum, IN_2);
			else
				PlaneIntersect(v2, v0, v1, tnum, IN_2);
		}
		else if(maxY > 1.0f)//top
		{
			tnum = tnum | 4;
			if(v0.y < 1.0f && v1.y > 1.0f && v2.y > 1.0f)
				PlaneIntersect(v1, v2, v0, tnum, IN_1);
			else if(v1.y < 1.0f && v0.y > 1.0f && v2.y > 1.0f)
				PlaneIntersect(v0, v2, v1, tnum, IN_1);
			else if(v2.y < 1.0f && v0.y > 1.0f && v1.y > 1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_1);
			else if(v0.y > 1.0f && v1.y < 1.0f && v2.y < 1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_2);
			else if(v1.y > 1.0f && v0.y < 1.0f && v2.y < 1.0f)
				PlaneIntersect(v1, v0, v2, tnum, IN_2);
			else
				PlaneIntersect(v2, v0, v1, tnum, IN_2);
		}
		else if(maxX > 1.0f)//right
		{
			tnum = tnum | 8;
			if(v0.x < 1.0f && v1.x > 1.0f && v2.x > 1.0f)
				PlaneIntersect(v1, v2, v0, tnum, IN_1);
			else if(v1.x < 1.0f && v0.x > 1.0f && v2.x > 1.0f)
				PlaneIntersect(v0, v2, v1, tnum, IN_1);
			else if(v2.x < 1.0f && v0.x > 1.0f && v1.x > 1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_1);
			else if(v0.x > 1.0f && v1.x < 1.0f && v2.x < 1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_2);
			else if(v1.x > 1.0f && v0.x < 1.0f && v2.x < 1.0f)
				PlaneIntersect(v1, v0, v2, tnum, IN_2);
			else
				PlaneIntersect(v2, v0, v1, tnum, IN_2);

		}
		else if(minY < -1.0f)//low
		{
			tnum = tnum | 16;
			if(v0.y > -1.0f && v1.y < -1.0f && v2.y < -1.0f)
				PlaneIntersect(v1, v2, v0, tnum, IN_1);
			else if(v1.y > -1.0f && v0.y < -1.0f && v2.y < -1.0f)
				PlaneIntersect(v0, v2, v1, tnum, IN_1);
			else if(v2.y > -1.0f && v0.y < -1.0f && v1.y < -1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_1);
			else if(v0.y < -1.0f && v1.y > -1.0f && v2.y > -1.0f)
				PlaneIntersect(v0, v1, v2, tnum, IN_2);
			else if(v1.y < -1.0f && v0.y > -1.0f && v2.y > -1.0f)
				PlaneIntersect(v1, v0, v2, tnum, IN_2);
			else
				PlaneIntersect(v2, v0, v1, tnum, IN_2);
		}
	}
}

void ToScreenSpace(float *x, float *y)
{
    *x = (float)(TargetFB->w) / 2 * (*x) + (float)(TargetFB->w) / 2;
    *y = -(float)(TargetFB->h) / 2 * (*y) + (float)(TargetFB->h) / 2;
}

void DrawInterval(int lint, int lnum, int ldsor, int rint, int rnum, int rdsor, int y, float lz, float rz)
{
	if(lnum > ldsor / 2) lint++;
	//lnum = ldsor / 2;
	if(rnum > rdsor / 2) rint++;
	//rnum = 0;
	//rdsor = ldsor;
	int len = rint - lint;
	float tz = (rz - lz) * 0.5f / len;
	float stepz = tz * 2;
	tz += lz;
	while(lint < rint /*&& lnum < rnum*/)
	{
		SetPixel(lint, y, tz, tcolor);
		lint++;
		tz += stepz;
	}
//	if(rz > tz) printf("%f\t%f\n", tz, rz);
	//if((lint == rint) && ((rdsor / 2) < rnum)) SetPixel(lint, y, 0x00BBCCFF);
}

void pmx(int *inte, int* num, int dsor, int step)
{
	if(step < 0)
	{
		while(*num < 0)
		{
			*num += dsor;
			(*inte)--;
		}
	}
	else
	{
		while(*num > dsor)
		{
			*num -= dsor;
			(*inte)++;
		}
	}
}

float getZ(const vec2i* dir, int inte, int num, int dsor, float dz)
{
	float len = sqrtf(dir->x * dir->x + dir->y * dir->y) / 2;
	num = inte * dsor + num;
	float x = (float)abs(num) / dsor;
	float tlen = sqrtf(x * x + 0.25f);
	return dz * tlen / len;
}

void DrawLeftRightLow(vec2i top, vec2i med, vec2i low)
{
#ifndef FLOAT_LINE
	/*vec2i topi, medi, lowi;
	if(top.x - floor(top.x) == 0.5f) topi.x = (int)top.x;
	else topi.x = (int)roundf(top.x);
	if(top.x - floor(top.y) == 0.5f) topi.y = (int)top.y;
	else topi.y = (int)roundf(top.y);

	if(med.x - floor(med.x) == 0.5f) medi.x = (int)med.x;
	else medi.x = (int)roundf(med.x);
	if(med.x - floor(med.y) == 0.5f) medi.y = (int)med.y;
	else medi.y = (int)roundf(med.y);

	if(low.x - floor(low.x) == 0.5f) lowi.x = (int)low.x;
	else lowi.x = (int)roundf(low.x);
	if(low.x - floor(low.y) == 0.5f) lowi.y = (int)low.y;
	else lowi.y = (int)roundf(low.y);

	topi.z = top.z;
	medi.z = med.z;
	lowi.z  = low.z;*/
	/*topi.x = floor(top.x);
	topi.y = floor(top.y);
	medi.x = floor(med.x);
	medi.y = floor(med.y);
	lowi.x = floor(low.x);
	lowi.y = floor(low.y);*/

	vec2i tldir, tmdir;
	tldir.x = (low.x - top.x) * 2;
	tldir.y = (low.y - top.y) * 2;

	tmdir.x = (med.x - top.x) * 2;
	tmdir.y = (med.y - top.y) * 2;

	int lnum, ldsor, rnum, rdsor, lint, rint;

	rnum = tmdir.x / 2;
	rdsor = tmdir.y;
	rint = top.x;
/*	while(rnum > rdsor)
	{
		rint++;
		rnum -= rdsor;
	}*/
	pmx(&rint, &rnum, rdsor, tmdir.x);

	lint = top.x;
	ldsor = tldir.y;
	lnum = tldir.x / 2;
/*	while(lnum < 0)
	{
		lnum += ldsor;
		lint--;
	}*/

	pmx(&lint, &lnum, ldsor, tldir.x);

	float lz = getZ(&tldir, lint - top.x, lnum, ldsor, low.z - top.z);
	float lstepz = lz * 2;
	lz += top.z;

	float rz = getZ(&tmdir, rint - top.x, rnum, rdsor, med.z - top.z);
	float rstepz = rz * 2;
	rz += top.z;

	while(top.y < low.y)
	{
		DrawInterval(lint, lnum, ldsor, rint, rnum, rdsor, top.y, lz, rz);
		lz += lstepz;
		rz += rstepz;
		top.y++;
		rnum += tmdir.x;
		/*while(rnum > rdsor)
		{
			rnum -= rdsor;
			rint++;
		}*/
		pmx(&rint, &rnum, rdsor, tmdir.x);

		lnum += tldir.x;
		/*while(lnum < 0)
		{
			lnum += ldsor;
			lint--;
		}*/
		pmx(&lint, &lnum, ldsor, tldir.x);
	}

#else

	if(top.x - floor(top.x) == 0.5f) top.x = floor(top.x);
	else top.x = roundf(top.x);
	if(top.y - floor(top.y) == 0.5f) top.y = floor(top.y);
	else top.y = roundf(top.y);

	if(low.x - floor(low.x) == 0.5f) low.x = floor(low.x);
	else low.x = roundf(low.x);
	if(low.y - floor(low.y) == 0.5f) low.y = floor(low.y);
	else low.y = roundf(low.y);

	if(med.x - floor(med.x) == 0.5f) med.x = floor(med.x);
	else med.x = roundf(med.x);
	if(med.y - floor(med.y) == 0.5f) med.y = floor(med.y);
	else med.y = roundf(med.y);

	vec3 tldir, tmdir;
	sub_v(&low, &top, &tldir);
	sub_v(&med, &top, &tmdir);

	vec3 targetP;
	//targetP.x = top.x + 0.5f;
	targetP.y = top.y + 0.5f;

	targetP.x = top.x + (targetP.y - top.y) * (tldir.x / tldir.y);
	if(targetP.x - floor(targetP.x) != 0.5f) targetP.x = roundf(targetP.x) + 0.5f;

	vec3 intvec;
	intvec.y = targetP.y;
	intvec.x = top.x + (intvec.y - top.y) * (tmdir.x / tmdir.y);

	while(targetP.y < low.y)
	{
		while(targetP.x < intvec.x)
		{
			SetPixel(targetP.x, targetP.y, 0x00BBCCFF);
			targetP.x++;
		}
		targetP.y++;
		targetP.x = top.x + (targetP.y - top.y) * (tldir.x / tldir.y);
		intvec.y = targetP.y;
		intvec.x = top.x + (intvec.y - top.y) * (tmdir.x / tmdir.y);
		if(targetP.x - floor(targetP.x) != 0.5f) targetP.x = roundf(targetP.x) + 0.5f;
	}

#endif
}

void DrawLeftTopRight(vec2i top, vec2i med, vec2i low)
{
#ifndef FLOAT_LINE
	/*vec2i topi, medi, lowi;
	if(top.x - floor(top.x) == 0.5f) topi.x = (int)top.x;
	else topi.x = (int)roundf(top.x);
	if(top.x - floor(top.y) == 0.5f) topi.y = (int)top.y;
	else topi.y = (int)roundf(top.y);

	if(med.x - floor(med.x) == 0.5f) medi.x = (int)med.x;
	else medi.x = (int)roundf(med.x);
	if(med.x - floor(med.y) == 0.5f) medi.y = (int)med.y;
	else medi.y = (int)roundf(med.y);

	if(low.x - floor(low.x) == 0.5f) lowi.x = (int)low.x;
	else lowi.x = (int)roundf(low.x);
	if(low.x - floor(low.y) == 0.5f) lowi.y = (int)low.y;
	else lowi.y = (int)roundf(low.y);

	topi.z = top.z;
	medi.z = med.z;
	lowi.z = low.z;*/
	//printf("%d %d %d %d %d %d\n", topi.x, topi.y, medi.x, medi.y, lowi.x, lowi.y);
	/*topi.x = floor(top.x);
	topi.y = floor(top.y);
	medi.x = floor(med.x);
	medi.y = floor(med.y);
	lowi.x = floor(low.x);
	lowi.y = floor(low.y);*/

	vec2i tldir, mldir;
	tldir.x = (low.x - top.x) * 2;
	tldir.y = (low.y - top.y) * 2;

	mldir.x = (low.x - med.x) * 2;
	mldir.y = (low.y - med.y) * 2;

	int lnum, ldsor, rnum, rdsor, lint, rint;

	ldsor = tldir.y;
	lnum = tldir.x / 2;
	lint = top.x;
/*	while(lnum > ldsor)
	{
		lint++;
		lnum -= ldsor;
	}*/
	pmx(&lint, &lnum, ldsor, tldir.x);

	rdsor = mldir.y;
	rnum = mldir.x / 2;
	rint = med.x;
/*	while(rnum < 0)
	{
		rint--;
		rnum += rdsor;
	}*/
	pmx(&rint, &rnum, rdsor, mldir.x);

	float lz = getZ(&tldir, lint - top.x, lnum, ldsor, low.z - top.z);
	float lstepz = lz * 2;
	lz += top.z;

	float rz = getZ(&mldir, rint - med.x, rnum, rdsor, low.z - med.z);
	float rstepz = rz * 2;
	rz += med.z;
//	printf("%d %d\t%d %d\n", lnum, ldsor, rnum, rdsor);
	while(top.y < low.y)
	{
		DrawInterval(lint, lnum, ldsor, rint, rnum, rdsor, top.y, lz, rz);
		lz += lstepz;
		rz += rstepz;
		top.y++;
		lnum += tldir.x;
		/*while(lnum > ldsor)
		{
			lint++;
			lnum -= ldsor;
		}*/
		pmx(&lint, &lnum, ldsor, tldir.x);

		rnum += mldir.x;
		/*while(rnum < 0)
		{
			rint--;
			rnum += rdsor;
		}*/
		pmx(&rint, &rnum, rdsor, mldir.x);
	}
//	printf("%f\t%f\n", lz, rz);

#else
	if(top.x - floor(top.x) == 0.5f) top.x = floor(top.x);
	else top.x = roundf(top.x);
	if(top.y - floor(top.y) == 0.5f) top.y = floor(top.y);
	else top.y = roundf(top.y);

	if(low.x - floor(low.x) == 0.5f) low.x = floor(low.x);
	else low.x = roundf(low.x);
	if(low.y - floor(low.y) == 0.5f) low.y = floor(low.y);
	else low.y = roundf(low.y);

	if(med.x - floor(med.x) == 0.5f) med.x = floor(med.x);
	else med.x = roundf(med.x);
	if(med.y - floor(med.y) == 0.5f) med.y = floor(med.y);
	else med.y = roundf(med.y);

	vec3 tldir, mldir;
	sub_v(&low, &top, &tldir);
	sub_v(&med, &low, &mldir);

	vec3 targetP;
	//targetP.x = top.x + 0.5f;
	targetP.y = top.y + 0.5f;

	targetP.x = top.x + (targetP.y - top.y) * (tldir.x / tldir.y);
	if(targetP.x - floor(targetP.x) != 0.5f) targetP.x = roundf(targetP.x) + 0.5f;

	vec3 intvec;
	intvec.y = targetP.y;
	intvec.x = med.x + (intvec.y - med.y) * (mldir.x / mldir.y);

	while(targetP.y < low.y)
	{
		while(targetP.x < intvec.x)
		{
			SetPixel(targetP.x, targetP.y, 0x00BBCCFF);
			targetP.x++;
		}
		targetP.y++;
		targetP.x = top.x + (targetP.y - top.y) * (tldir.x / tldir.y);
		intvec.y = targetP.y;
		intvec.x = med.x + (intvec.y - med.y) * (mldir.x / mldir.y);
		if(targetP.x - floor(targetP.x) != 0.5f) targetP.x = roundf(targetP.x) + 0.5f;
	}
#endif
}

void DrawLeftRightRight(vec2i top, vec2i med, vec2i low)
{
	/*vec2i topi, medi, lowi;
	if(top.x - floor(top.x) == 0.5f) topi.x = (int)top.x;
	else topi.x = (int)roundf(top.x);
	if(top.x - floor(top.y) == 0.5f) topi.y = (int)top.y;
	else topi.y = (int)roundf(top.y);

	if(med.x - floor(med.x) == 0.5f) medi.x = (int)med.x;
	else medi.x = (int)roundf(med.x);
	if(med.x - floor(med.y) == 0.5f) medi.y = (int)med.y;
	else medi.y = (int)roundf(med.y);

	if(low.x - floor(low.x) == 0.5f) lowi.x = (int)low.x;
	else lowi.x = (int)roundf(low.x);
	if(low.x - floor(low.y) == 0.5f) lowi.y = (int)low.y;
	else lowi.y = (int)roundf(low.y);

	topi.z = top.z;
	medi.z = med.z;
	lowi.z = low.z;*/

	/*topi.x = floor(top.x);
	topi.y = floor(top.y);
	medi.x = floor(med.x);
	medi.y = floor(med.y);
	lowi.x = floor(low.x);
	lowi.y = floor(low.y);*/

	vec2i tldir, tmdir;
	tldir.x = (low.x - top.x) * 2;
	tldir.y = (low.y - top.y) * 2;

	tmdir.x = (med.x - top.x) * 2;
	tmdir.y = (med.y - top.y) * 2;

	int lnum, ldsor, rnum, rdsor, lint, rint;

	ldsor = tldir.y;
	lnum = tldir.x / 2;
	lint = top.x;
	/*while(lnum < 0)
	{
		lnum += ldsor;
		lint--;
	}*/
	pmx(&lint, &lnum, ldsor, tldir.x);

	rdsor = tmdir.y;
	rnum = tmdir.x / 2;
	rint = top.x;
	/*while(rnum > rdsor)
	{
		rnum -= rdsor;
		rint++;
	}*/
	pmx(&rint, &rnum, rdsor, tmdir.x);

	float lz = getZ(&tldir, lint - top.x, lnum, ldsor, low.z - top.z);
	float lstepz = lz * 2;
	lz += top.z;

	float rz = getZ(&tmdir, rint - top.x, rnum, rdsor, med.z - top.z);
	float rstepz = rz * 2;
	rz += top.z;

	while(top.y < med.y)
	{
		DrawInterval(lint, lnum, ldsor, rint, rnum, rdsor, top.y, lz, rz);
		lz += lstepz;
		rz += rstepz;
		top.y++;
		lnum += tldir.x;
		/*while(lnum < 0)
		{
			lnum += ldsor;
			lint--;
		}*/
		pmx(&lint, &lnum, ldsor, tldir.x);

		rnum += tmdir.x;
		/*while(rnum > rdsor)
		{
			rnum -= rdsor;
			rint++;
		}*/
		pmx(&rint, &rnum, rdsor, tmdir.x);
	}

	vec2i mldir;
	mldir.x = (low.x - med.x) * 2;
	mldir.y = (low.y - med.y) * 2;

	rdsor = mldir.y;
	rnum = mldir.x / 2;
	rint = med.x;
	/*while(rnum < 0)
	{
		rnum += rdsor;
		rint--;
	}*/
	pmx(&rint, &rnum, rdsor, mldir.x);

	rz = getZ(&mldir, rint - med.x, rnum, rdsor, low.z - med.z);
	rstepz = rz * 2;
	rz += med.z;

	while(med.y < low.y)
	{
		DrawInterval(lint, lnum, ldsor, rint, rnum, rdsor, med.y, lz, rz);
		lz += lstepz;
		rz += rstepz;
		med.y++;
		lnum += tldir.x;
		/*while(lnum < 0)
		{
			lnum += ldsor;
			lint--;
		}*/
		pmx(&lint, &lnum, ldsor, tldir.x);

		rnum += mldir.x;
		/*while(rnum < 0)
		{
			rnum += rdsor;
			rint--;
		}*/
		pmx(&rint, &rnum, rdsor, mldir.x);
	}
}

void DrawLeftLeftRight(vec2i top, vec2i med, vec2i low)
{
	/*vec2i topi, medi, lowi;
	if(top.x - floor(top.x) == 0.5f) topi.x = (int)top.x;
	else topi.x = (int)roundf(top.x);
	if(top.x - floor(top.y) == 0.5f) topi.y = (int)top.y;
	else topi.y = (int)roundf(top.y);

	if(med.x - floor(med.x) == 0.5f) medi.x = (int)med.x;
	else medi.x = (int)roundf(med.x);
	if(med.x - floor(med.y) == 0.5f) medi.y = (int)med.y;
	else medi.y = (int)roundf(med.y);

	if(low.x - floor(low.x) == 0.5f) lowi.x = (int)low.x;
	else lowi.x = (int)roundf(low.x);
	if(low.x - floor(low.y) == 0.5f) lowi.y = (int)low.y;
	else lowi.y = (int)roundf(low.y);

	topi.z = top.z;
	medi.z = med.z;
	lowi.z = low.z;*/

	vec2i tldir, tmdir;
	tldir.x = (low.x - top.x) * 2;
	tldir.y = (low.y - top.y) * 2;

	tmdir.x = (med.x - top.x) * 2;
	tmdir.y = (med.y - top.y) * 2;

	int lnum, ldsor, rnum, rdsor, lint, rint;
	lnum = tmdir.x / 2;
	ldsor = tmdir.y;
	lint = top.x;
	/*while(lnum < 0)
	{
		lnum += ldsor;
		lint--;
	}*/
	pmx(&lint, &lnum, ldsor, tmdir.x);

	rnum = tldir.x / 2;
	rdsor = tldir.y;
	rint = top.x;
	/*while(rnum > rdsor)
	{
		rnum -= rdsor;
		rint++;
	}*/
	pmx(&rint, &rnum, rdsor, tldir.x);

	float lz = getZ(&tmdir, lint - top.x, lnum, ldsor, med.z - top.z);
	float lstepz = lz * 2;
	lz += top.z;

	float rz = getZ(&tldir, rint - top.x, rnum, rdsor, low.z - top.z);
	float rstepz = rz * 2;
	rz += top.z;

	while(top.y < med.y)
	{
		DrawInterval(lint, lnum, ldsor, rint, rnum, rdsor, top.y, lz, rz);
		lz += lstepz;
		rz += rstepz;
		top.y++;
		lnum += tmdir.x;
		/*while(lnum < 0)
		{
			lnum += ldsor;
			lint--;
		}*/
		pmx(&lint, &lnum, ldsor, tmdir.x);

		rnum += tldir.x;
		/*while(rnum > rdsor)
		{
			rnum -= rdsor;
			rint++;
		}*/
		pmx(&rint, &rnum, rdsor, tldir.x);
	}

	vec2i mldir;
	mldir.x = (low.x - med.x) * 2;
	mldir.y = (low.y - med.y) * 2;

	lnum = mldir.x / 2;
	ldsor = mldir.y;
	lint = med.x;
	/*while(lnum > ldsor)
	{
		lnum -= ldsor;
		lint++;
	}*/
	pmx(&lint, &lnum, ldsor, mldir.x);

	lz = getZ(&mldir, lint - med.x, lnum, ldsor, low.z - med.z);
	lstepz = lz * 2;
	lz += med.z;

	while(med.y < low.y)
	{
		DrawInterval(lint, lnum, ldsor, rint, rnum, rdsor, med.y, lz, rz);
		lz += lstepz;
		rz += rstepz;
		med.y++;
		lnum += mldir.x;
		/*while(lnum > ldsor)
		{
			lnum -= ldsor;
			lint++;
		}*/
		pmx(&lint, &lnum, ldsor, mldir.x);

		rnum += tldir.x;
		/*while(rnum > rdsor)
		{
			rnum -= rdsor;
			rint++;
		}*/
		pmx(&rint, &rnum, rdsor, tldir.x);
	}
}

#define TOP 1
#define LOW 10
#define LEFT 100
#define RIGHT 1000

void DrawPolygon( const vec3* v0, const vec3* v1, const vec3* v2)
{
	int casenum = 0;
	/*if(v1->y - v0->y == 0 && v1->x - v0->x < 0) casenum += LOW;
	else if(v1->y - v0->y == 0 && v1->x - v0->x > 0) casenum += TOP;
	else if(v1->y - v0->y > 0) casenum += RIGHT;
	else if(v1->y - v0->y < 0) casenum += LEFT;

	if(v2->y - v1->y == 0 && v2->x - v1->x < 0) casenum += LOW;
	else if(v2->y - v1->y == 0 && v2->x - v1->x > 0) casenum += TOP;
	else if(v2->y - v1->y > 0) casenum += RIGHT;
	else if(v2->y - v1->y < 0) casenum += LEFT;

	if(v0->y - v2->y == 0 && v0->x - v2->x < 0) casenum += LOW;
	else if(v0->y - v2->y == 0 && v0->x - v2->x > 0) casenum += TOP;
	else if(v0->y - v2->y > 0) casenum += RIGHT;
	else if(v0->y - v2->y < 0) casenum += LEFT;*/


	vec2i vi0, vi1, vi2, top, med, low;
	if(v0->x - floor(v0->x) == 0.5f) vi0.x = (int)v0->x;
	else vi0.x = (int)roundf(v0->x);
	if(v0->y - floor(v0->y) == 0.5f) vi0.y = (int)v0->y;
	else vi0.y = (int)roundf(v0->y);

	if(v1->x - floor(v1->x) == 0.5f) vi1.x = (int)v1->x;
	else vi1.x = (int)roundf(v1->x);
	if(v1->y - floor(v1->y) == 0.5f) vi1.y = (int)v1->y;
	else vi1.y = (int)roundf(v1->y);

	if(v2->x - floor(v2->x) == 0.5f) vi2.x = (int)v2->x;
	else vi2.x = (int)roundf(v2->x);
	if(v2->y - floor(v2->y) == 0.5f) vi2.y = (int)v2->y;
	else vi2.y = (int)roundf(v2->y);

	vi0.z = v0->z;
	vi1.z = v1->z;
	vi2.z = v2->z;

	if((vi0.x == vi1.x && vi0.x == vi2.x) || (vi0.y == vi1.y && vi0.y == vi2.y)) return;
	else if(vi0.y == vi1.y)
	{
		if(vi2.y < vi0.y)
		{
			casenum += LOW;
			top = vi2;
			if(vi0.x < vi1.x) {low = vi0; med = vi1;}
			else {low = vi1; med = vi0;}
		}
		else
		{
			casenum += TOP;
			low = vi2;
			if(vi0.x < vi1.x) {top = vi0; med = vi1;}
			else {top = vi1; med = vi0;}
		}
	}
	else if(vi0.y == vi2.y)
	{
		if(vi1.y < vi0.y)
		{
			casenum += LOW;
			top = vi1;
			if(vi0.x < vi2.x) {low = vi0; med = vi2;}
			else {low = vi2; med = vi0;}
		}
		else
		{
			casenum += TOP;
			low = vi1;
			if(vi0.x < vi2.x) {top = vi0; med = vi2;}
			else {top = vi2; med = vi0;}
		}
	}
	else if(vi2.y == vi1.y)
	{
		if(vi0.y < vi1.y)
		{
			casenum += LOW;
			top = vi0;
			if(vi2.x < vi1.x) {low = vi2; med = vi1;}
			else {low = vi1; med = vi2;}
		}
		else
		{
			casenum += TOP;
			low = vi0;
			if(vi2.x < vi1.x) {top = vi2; med = vi1;}
			else {top = vi1; med = vi2;}
		}
	}
	else if(vi0.y < vi1.y && vi0.y < vi2.y)
	{
		casenum += RIGHT + LEFT;
		top = vi0;
		if(vi1.y < vi2.y) {med = vi1; low = vi2;}
		else {med = vi2; low = vi1;}
	}
	else if(vi1.y < vi0.y && vi1.y < vi2.y)
	{
		casenum += RIGHT + LEFT;
		top = vi1;
		if(vi0.y < vi2.y) {med = vi0; low = vi2;}
		else {med = vi2; low = vi0;}
	}
	else
	{
		casenum += RIGHT + LEFT;
		top = vi2;
		if(vi0.y < vi1.y) {med = vi0; low = vi1;}
		else {med = vi1; low = vi0;}
	}
	//vec3 top, low, med;
	vec3 intmed, dir;

/*	printf("%d %d\n", top.x, top.y);
	printf("%d %d\n", med.x, med.y);
	printf("%d %d\n", low.x, low.y);
	printf("%d\n", casenum);
*/

/*	DrawLineDepth(top, med);
	DrawLineDepth(med, low);
	DrawLineDepth(low, top);
	return;*/

	vec2i tldir;
	switch(casenum)
	{
		case TOP: DrawLeftTopRight(top, med, low);
		break;
		case LOW: DrawLeftRightLow(top, med, low);
		break;
		case RIGHT + LEFT:
		tldir.x = low.x - top.x;
		tldir.y = low.y - top.y;
		int mint, mnum, mdsor, lint, lnum, ldsor;
		mint = med.x - 1;
		mnum = mdsor = ldsor = tldir.y;

		lnum = tldir.x * (med.y - top.y);
		lint = top.x;
		pmx(&lint, &lnum, ldsor, tldir.x);
		int dx = mint - lint;
		if(dx > 0) casenum = LEFT;
		else if(dx < 0) casenum = RIGHT;
		else if((mnum - lnum) > 0) casenum = LEFT;
		else casenum = RIGHT;

		if(casenum == RIGHT) DrawLeftLeftRight(top, med, low);
		else DrawLeftRightRight(top, med, low);
		break;
	}
	/*{
		case LEFT + RIGHT + LOW:
		if(v0->y < v1->y && v0->y < v2->y)
		{
			top = *v0;
			if(v1->x > v2->x)
			{
				low = *v2;
				med = *v1;
			}
			else
			{
				low = *v1;
				med = *v2;
			}
		}
		else if(v1->y < v2->y && v1->y < v0->y)
		{
			top = *v1;
			if(v0->x > v2->x)
			{
				low = *v2;
				med = *v0;
			}
			else
			{
				low = *v0;
				med = *v2;
			}
		}
		else
		{
			top = *v2;
			if(v0->x > v1->x)
			{
				low = *v1;
				med = *v0;
			}
			else
			{
				low = *v0;
				med = *v1;
			}
		}
		DrawLeftRightLow(top, med, low);
		break;
		case LEFT + RIGHT + TOP:
		if(v0->y > v1->y && v0->y > v2->y)
		{
			low = *v0;
			if(v1->x > v2->x)
			{
				med = *v1;
				top = *v2;
			}
			else
			{
				med = *v2;
				top = *v1;
			}
		}
		else if(v1->y > v0->y && v1->y > v2->y)
		{
			low = *v1;
			if(v0->x > v2->x)
			{
				med = *v0;
				top = *v2;
			}
			else
			{
				med = *v2;
				top = *v0;
			}
		}
		else
		{
			low = *v2;
			if(v0->x > v1->x)
			{
				med = *v0;
				top = *v1;
			}
			else
			{
				med = *v1;
				top = *v0;
			}
		}
		DrawLeftTopRight(top, med, low);
		break;
		case LEFT + LEFT + RIGHT:
		if(v0->y < v1->y && v0->y < v2->y)
		{
			top = *v0;
			if(v1->y < v2->y)
			{
				med = *v1;
				low = *v2;
			}
			else
			{
				med = *v2;
				low = *v1;
			}
		}
		else if(v1->y < v2->y && v1->y < v0->y)
		{
			top = *v1;
			if(v0->y < v2->y)
			{
				med = *v0;
				low = *v2;
			}
			else
			{
				med = *v2;
				low = *v0;
			}
		}
		else
		{
			top = *v2;
			if(v0->y < v1->y)
			{
				med = *v0;
				low = *v1;
			}
			else
			{
				med = *v1;
				low = *v0;
			}
		}
#ifndef FLOAT_LINE
		DrawRightRightLeft(top, med, low);
#else
		sub_v(&low, &top, &dir);
		intmed.y = med.y;
		intmed.x = (med.y - top.y) * (dir.x / dir.y) + top.x;
		DrawLeftRightLow(top, intmed, med);
		DrawLeftTopRight(med, intmed, low);
#endif
		break;
		case LEFT + RIGHT + RIGHT:
		if(v0->y < v1->y && v0->y < v2->y)
		{
			top = *v0;
			if(v1->y < v2->y)
			{
				med = *v1;
				low = *v2;
			}
			else
			{
				med = *v2;
				low = *v1;
			}
		}
		else if(v1->y < v2->y && v1->y < v0->y)
		{
			top = *v1;
			if(v0->y < v2->y)
			{
				med = *v0;
				low = *v2;
			}
			else
			{
				med = *v2;
				low = *v0;
			}
		}
		else
		{
			top = *v2;
			if(v0->y < v1->y)
			{
				med = *v0;
				low = *v1;
			}
			else
			{
				med = *v1;
				low = *v0;
			}
		}
#ifndef FLOAT_LINE
		DrawLeftRightRight(top, med, low);
#else
		sub_v(&low, &top, &dir);
		intmed.y = med.y;
		intmed.x = (med.y - top.y) * (dir.x / dir.y) + top.x;
		DrawLeftRightLow(top, med, intmed);
		DrawLeftTopRight(intmed, med, low);
#endif
		break;
	}*/
//	printf("%d\n", casenum);
}
