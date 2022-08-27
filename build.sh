#!/bin/bash

gcc \
	-mfpmath=387 \
	main.c \
	framebuffer.c \
	draw.c \
	vec.c \
	cull.c \
	pipe_op.c \
	pipe_pol.c \
	-lm -lSDL2 \
	-o prog

