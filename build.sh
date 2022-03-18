#!/bin/bash

gcc \
	-mfpmath=387 \
	main.c \
	framebuffer.c \
	draw.c \
	vec3.c \
	-lm -lSDL2 \
	-o prog

