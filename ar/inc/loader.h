#ifndef _LOADER_H_
#define _LOADER_H_

#include<stdio.h>

int getNumWalls(const char* file);
int loadMaze(const char* file,float **vArray,int nWalls);

#endif