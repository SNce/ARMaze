#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<GL/glut.h>
#include<GL/gl.h>
#include<string.h>
#include<stdio.h>

void print_string(char *string,float color[3],float pos[3],void * font);
void print_time(float time);


#endif