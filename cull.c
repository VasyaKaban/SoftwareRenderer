#include "cull.h"
#include "draw.h"
#include "malloc.h"

#include <stdio.h>

void polygon_list_push_back(polygon_list *l, pipe_polygon *pol, size_t data_ceil_size)
{
	if(l->end == NULL)//empty list
	{
		l->start = malloc(sizeof(struct node));
		l->end = l->start;
		l->end->next = l->start->prev = NULL;
		l->end->data = pipe_polygon_alloc_data(data_ceil_size);
		*((pipe_polygon*)l->end->data) = *pol;
		memcpy(&((pipe_polygon*)l->end->data)->v2 + 1, &pol->v2 + 1, data_ceil_size * 3);
	}
	else
	{
		l->end->next = malloc(sizeof(struct node));
		l->end->next->prev = l->end;
		l->end = l->end->next;
		l->end->data = pipe_polygon_alloc_data(data_ceil_size);
		*((pipe_polygon*)l->end->data) = *pol;
		memcpy(&((pipe_polygon*)l->end->data)->v2 + 1, &pol->v2 + 1, data_ceil_size * 3);
		l->end->next = NULL;
	}
}

void polygon_list_push_after_node(polygon_list *l, struct node *n, pipe_polygon *pol, size_t data_ceil_size)
{
	if(n == NULL)
	{
		return;
	}
	else if(n == l->end)
	{
		polygon_list_push_back(l, pol, data_ceil_size);
	}
	else
	{
		struct node *next = n->next;
		n->next = malloc(sizeof(struct node));
		n->next->data = pipe_polygon_alloc_data(data_ceil_size);
		*((pipe_polygon*)n->next->data) = *pol;
		memcpy(((pipe_polygon*)n->next->data) + 1, pol + 1, data_ceil_size * 3);
		n->next->prev = n;
		n->next->next = next;
		next->prev = n->next;
	}
}

void polygon_list_pop(polygon_list *l, struct node *n)
{
	if(n == l->start && n == l->end)//one node on list
	{
		pipe_polygon_free_alloc(l->start->data);
		free(l->start);
		l->start = l->end = NULL;
	}
	else if(n == l->start)//pop start
	{
		l->start = l->start->next;
		l->start->prev = NULL;
		pipe_polygon_free_alloc(n->data);
		free(n);
	}
	else if(n == l->end)//pop end
	{
		l->end = l->end->prev;
		l->end->next = NULL;
		pipe_polygon_free_alloc(n->data);
		free(n);
	}
	else
	{
		struct node *next, *prev;
		next = n->next;
		prev = n->prev;
		pipe_polygon_free_alloc(n->data);
		free(n);
		next->prev = prev;
		prev->next = next;
	}
}

void polygon_list_delete(polygon_list *l)
{
	struct node *target = l->start;
	struct node *next;
	while(target != NULL)
	{
		next = target->next;
		pipe_polygon_free_alloc(target->data);
		free(target);
		target = next;
	}
	l->start = l->end = NULL;
}

/*
not intersect{
ret 0 if inside
ret 2 if outside
}
intersect{
ret 1, if en on another side from plane normal
ret -1 if st on another side from plane normal
}
*/
int line_intersect_plane_3D(vec4 *st, void *st_data, vec4 *en, void *en_data, float *pl, vec4 *out, void *out_data, pipe_op *op)
{
	//A(v0x+axt) + B(v0y+ayt) +C(v0z+azt) = -D
	//Av0x + Aaxt + Bv0y + Bayt + Cv0z + Cazt = -D
	//t = -(D + Av0x + Bv0y + Cv0z) / (Aax + Bay + Caz)
	vec4 a;
	sub_vec4(en, st, &a);
	float t = -(pl[3] + pl[0]*st->x + pl[1]*st->y + pl[2]*st->z) / (pl[0]*a.x + pl[1]*a.y + pl[2]*a.z);
	float st_test = (pl[0]*st->x + pl[1]*st->y + pl[2]*st->z + pl[3]);
	float en_test = (pl[0]*en->x + pl[1]*en->y + pl[2]*en->z + pl[3]);

	if(st_test < 0.0f && en_test < 0.0f)
		return 2;
	else if(st_test >= 0.0f && en_test >= 0.0f)
		return 0;
	else
	{
		out->x = st->x + t * a.x;
		out->y = st->y + t * a.y;
		out->z = st->z + t * a.z;
		out->w = st->w + t * a.w;

#warning ADD IN NEXT SAFETY UPD(is op->data_ceil_size == 0 or lerp_functions == NULL)!
		op->lerp_functions.lerp_pol_data(st_data, en_data, t, out_data);

		if(en_test < 0.0f)
			return 1;
		else
			return -1;
	}
}

/*
not intersect{
//ret 0 if vertexes on side of plane normal
//ret 4 if vertexes on another side of plane normal
}
intersect{
ret 1 if v0 on side and v1 and v2 not at side
ret 2 if v1 on side and v0 and v2 not at side
ret 3 if v2 on side and v1 and v0 not at side
ret -1 if v1 and v2 on side and v0 not at side
ret -2 if v0 and v2 on side and v1 not at side
ret -3 if v1 and v0 on side and v2 not at side
}
*/
int polygon_intersect_plane_3D(pipe_polygon *pol, float *pl, pipe_polygon *out0, pipe_polygon *out1, pipe_op *op)
{
	int result = 0;
	vec4 i10, i21, i02;
	void *i10_data;
	void *i21_data;
	void *i02_data;
	if(op->data_ceil_size != 0)
	{
		i10_data = malloc(op->data_ceil_size);
		i21_data = malloc(op->data_ceil_size);
		i02_data = malloc(op->data_ceil_size);
	}
	int test10 = 0, test21 = 0, test02 = 0;
	test10 = line_intersect_plane_3D(&pol->v0, &pol->v2 + 1, &pol->v1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, pl, &i10, i10_data, op);
	test21 = line_intersect_plane_3D(&pol->v1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, &pol->v2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, pl, &i21, i21_data, op);
	test02 = line_intersect_plane_3D(&pol->v2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, &pol->v0, &pol->v2 + 1, pl, &i02, i02_data, op);

	if(test10 == 2 && test21 == 2 && test02 == 2)//polygon outside
		result =  4;
	else if(test10 == 0 && test21 == 0 && test02 == 0)//polygon inside
		result =  0;
	else if(test10 == 1 && test21 == 2 && test02 == -1)//v0 inside
	{
		out0->v0 = pol->v0;
		out0->v1 = i10;
		out0->v2 = i02;

		memcpy(&out0->v2 + 1, &pol->v2 + 1, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, i02_data, op->data_ceil_size);
		result =  1;
	}
	else if(test10 == -1 && test21 == 1 && test02 == 2)//v1 inside
	{
		out0->v0 = i10;
		out0->v1 = pol->v1;
		out0->v2 = i21;

		memcpy(&out0->v2 + 1, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, i21_data, op->data_ceil_size);
		result =  2;
	}
	else if(test10 == 2 && test21 == -1 && test02 == 1)//v2 inside
	{
		out0->v0 = i02;
		out0->v1 = i21;
		out0->v2 = pol->v2;

		memcpy(&out0->v2 + 1, i02_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, i21_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);
		result =  3;
	}
	else if(test10 == -1 && test21 == 0 && test02 == 1)//v0 outside
	{
		out0->v0 = i10;
		out0->v1 = pol->v1;
		out0->v2 = pol->v2;
		out1->v0 = pol->v2;
		out1->v1 = i02;
		out1->v2 = i10;

		memcpy(&out0->v2 + 1, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);

		memcpy(&out1->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size, i02_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size * 2, i10_data, op->data_ceil_size);
		result = -1;
	}
	else if(test10 == 1 && test21 == -1 && test02 == 0)//v1 outside
	{
		out0->v0 = pol->v0;
		out0->v1 = i10;
		out0->v2 = pol->v2;
		out1->v0 = pol->v2;
		out1->v1 = i10;
		out1->v2 = i21;

		memcpy(&out0->v2 + 1, &pol->v2 + 1, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);

		memcpy(&out1->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size * 2, i21_data, op->data_ceil_size);
		result = -2;
	}
	else if(test10 == 0 && test21 == 1 && test02 == -1)//v2 outside
	{
		out0->v0 = pol->v0;
		out0->v1 = pol->v1;
		out0->v2 = i21;
		out1->v0 = i21;
		out1->v1 = i02;
		out1->v2 = pol->v0;

		memcpy(&out0->v2 + 1, &pol->v2 + 1, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, i21_data, op->data_ceil_size);

		memcpy(&out1->v2 + 1, i21_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size, i02_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size * 2, &pol->v2 + 1, op->data_ceil_size);
		result = -3;
	}

	#warning DO NOT ALLOC, FREE AND MEMCPY IF DATA_CEIL_SIZE == 0!
	if(op->data_ceil_size != 0)
	{
		free(i10_data);
		free(i21_data);
		free(i02_data);
	}
	return result;
}

/*
not intersect{
ret 0 if inside BB
ret 2 if outside BB
}
intersect{
ret 1, if en outside BB
ret -1 if st outside BB
}
*/
int line_intersect_BB_edge(vec4 *st, void *st_data, vec4 *en, void *en_data, vec4 *lh, vec4 *rl, vec4 *out, void *out_data, enum bb_edge_type bt, pipe_op *op)
{
	float t;
	vec4 a;
	switch(bt)
	{
		case LEFT:
			if(st->x < lh->x && en->x < lh->x)
				return 2;
			else if(st->x >= lh->x && en->x >= lh->x)
				return 0;
			else
			{
				t = (lh->x - st->x) / (en->x - st->x);
				sub_vec4(en, st, &a);
				out->x = lh->x;
				out->y = st->y + a.y * t;
				out->z = st->z + a.z * t;
				out->w = st->w + a.w * t;

				op->lerp_functions.lerp_pol_data(st_data, en_data, t, out_data);
				if(st->x < lh->x && en->x >= lh->x)
					return -1;
				else //st->x > lh->x && en->x < lh->x
					return 1;
			}
			break;
		case TOP:
			if(st->y > lh->y && en->y > lh->y)
				return 2;
			else if(st->y <= lh->y && en->y <= lh->y)
				return 0;
			else
			{
				t = (lh->y - st->y) / (en->y - st->y);
				sub_vec4(en, st, &a);
				out->x = st->x + a.x * t;
				out->y = lh->y;
				out->z = st->z + a.z * t;
				out->w = st->w + a.w * t;

				op->lerp_functions.lerp_pol_data(st_data, en_data, t, out_data);
				if(st->y > lh->y && en->y <= lh->y)
					return -1;
				else //st->y < lh->y && en->y > lh->y
					return 1;
			}
			break;
		case RIGHT:
			if(st->x > rl->x && en->x > rl->x)
				return 2;
			else if(st->x <= rl->x && en->x <= rl->x)
				return 0;
			else
			{
				t = (rl->x - st->x) / (en->x - st->x);
				sub_vec4(en, st, &a);
				out->x = rl->x;
				out->y = st->y + a.y * t;
				out->z = st->z + a.z * t;
				out->w = st->w + a.w * t;

				op->lerp_functions.lerp_pol_data(st_data, en_data, t, out_data);
				if(st->x > rl->x && en->x <= rl->x)
					return -1;
				else //st->x < rl->x && en->x > rl->x
					return 1;
			}
			break;
		case BOTTOM:
			if(st->y < rl->y && en->y < rl->y)
				return 2;
			else if(st->y >= rl->y && en->y >= rl->y)
				return 0;
			else
			{
				t = (rl->y - st->y) / (en->y - st->y);
				sub_vec4(en, st, &a);
				out->x = st->x + a.x * t;
				out->y = rl->y;
				out->z = st->z + a.z * t;
				out->w = st->w + a.w * t;

				op->lerp_functions.lerp_pol_data(st_data, en_data, t, out_data);
				if(st->y < rl->y && en->y >= rl->y)
					return -1;
				else //st->y > rl->y && en->y < rl->y
					return 1;
			}
			break;
	}
}

/*
not intersect{
//ret 0 if vertexes inside BB
//ret 4 if vertexes outside BB
}
intersect{
ret 1 if v0 inside and v1 and v2 outside
ret 2 if v1 inside and v0 and v2 outside
ret 3 if v2 inside and v1 and v0 outside
ret -1 if v1 and v2 inside and v0 outside
ret -2 if v0 and v2 inside and v1 outside
ret -3 if v1 and v0 inside and v2 outside
}
*/
int polygon_intersect_BB_edge(pipe_polygon *pol, vec4 *lh, vec4 *rl, pipe_polygon *out0, pipe_polygon *out1, enum bb_edge_type bt, pipe_op *op)
{
	int result = 0;
	vec4 i10, i21, i02;
	void *i10_data;
	void *i21_data;
	void *i02_data;
	if(op->data_ceil_size != 0)
	{
		i10_data = malloc(op->data_ceil_size);
		i21_data = malloc(op->data_ceil_size);
		i02_data = malloc(op->data_ceil_size);
	}
	int test10 = 0, test21 = 0, test02 = 0;
	test10 = line_intersect_BB_edge(&pol->v0, &pol->v2 + 1, &pol->v1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, lh, rl, &i10, i10_data, bt, op);
	test21 = line_intersect_BB_edge(&pol->v1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, &pol->v2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, lh, rl, &i21, i21_data, bt, op);
	test02 = line_intersect_BB_edge(&pol->v2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2 , &pol->v0, &pol->v2 + 1 , lh, rl, &i02, i02_data, bt, op);

	if(test10 == 2 && test21 == 2 && test02 == 2)//polygon outside
		result = 4;
	else if(test10 == 0 && test21 == 0 && test02 == 0)
		result = 0;
	else if(test10 == 1 && test21 == 2 && test02 == -1)//v0 inside
	{
		out0->v0 = pol->v0;
		out0->v1 = i10;
		out0->v2 = i02;

		memcpy(&out0->v2 + 1, &pol->v2 + 1, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, i02_data, op->data_ceil_size);
		result = 1;
	}
	else if(test10 == -1 && test21 == 1 && test02 == 2)//v1 inside
	{
		out0->v0 = i10;
		out0->v1 = pol->v1;
		out0->v2 = i21;

		memcpy(&out0->v2 + 1, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, i21_data, op->data_ceil_size);
		result = 2;
	}
	else if(test10 == 2 && test21 == -1 && test02 == 1)//v2 inside
	{
		out0->v0 = i02;
		out0->v1 = i21;
		out0->v2 = pol->v2;

		memcpy(&out0->v2 + 1, i02_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, i21_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);
		result = 3;
	}
	else if(test10 == -1 && test21 == 0 && test02 == 1)//v0 outside
	{
		out0->v0 = i10;
		out0->v1 = pol->v1;
		out0->v2 = pol->v2;
		out1->v0 = pol->v2;
		out1->v1 = i02;
		out1->v2 = i10;

		memcpy(&out0->v2 + 1, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);

		memcpy(&out1->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size, i02_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size * 2, i10_data, op->data_ceil_size);
		result = -1;
	}
	else if(test10 == 1 && test21 == -1 && test02 == 0)//v1 outside
	{
		out0->v0 = pol->v0;
		out0->v1 = i10;
		out0->v2 = pol->v2;
		out1->v0 = pol->v2;
		out1->v1 = i10;
		out1->v2 = i21;

		memcpy(&out0->v2 + 1, &pol->v2 + 1, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);

		memcpy(&out1->v2 + 1, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size * 2, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size, i10_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size * 2, i21_data, op->data_ceil_size);
		result = -2;
	}
	else if(test10 == 0 && test21 == 1 && test02 == -1)//v2 outside
	{
		out0->v0 = pol->v0;
		out0->v1 = pol->v1;
		out0->v2 = i21;
		out1->v0 = i21;
		out1->v1 = i02;
		out1->v2 = pol->v0;

		memcpy(&out0->v2 + 1, &pol->v2 + 1, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size, ((uint8_t*)(&pol->v2 + 1)) + op->data_ceil_size, op->data_ceil_size);
		memcpy(((uint8_t*)(&out0->v2 + 1)) + op->data_ceil_size * 2, i21_data, op->data_ceil_size);

		memcpy(&out1->v2 + 1, i21_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size, i02_data, op->data_ceil_size);
		memcpy(((uint8_t*)(&out1->v2 + 1)) + op->data_ceil_size * 2, &pol->v2 + 1, op->data_ceil_size);

		result = -3;
	}

	#warning DO NOT ALLOC, FREE AND MEMCPY IF DATA_CEIL_SIZE == 0!
	if(op->data_ceil_size != 0)
	{
		free(i10_data);
		free(i21_data);
		free(i02_data);
	}
	return result;
}
