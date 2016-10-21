/*
 * Copyright (C) 2015-2016, Michiel Sikma <michiel@sikma.org>
 * MIT License
 */

#ifndef __CEEGEE_GFX_STARFIELD_ALGOS__
#define __CEEGEE_GFX_STARFIELD_ALGOS__

extern int (*ALGORITHMS[])(float *x, float *y, int *n, int a, int b, float c);
extern int (*stars_algo_ptr)(float *x, float *y, int *n, int a, int b, float c);
extern int ALGOS;
int stars_circle_weird(float *x, float *y, int *n, int a, int b, float c);
int stars_circle(float *x, float *y, int *n, int a, int b, float c);
int stars_net(float *x, float *y, int *n, int a, int b, float c);
int stars_random_f(float *x, float *y, int *n, int a, int b, float c);
int stars_random_i(float *x, float *y, int *n, int a, int b, float c);
int stars_zigzag(float *x, float *y, int *n, int a, int b, float c);

#endif
