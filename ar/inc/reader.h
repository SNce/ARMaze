#ifndef _READER_H_
#define _READER_H_

#include<stdio.h>

int getNumWalls(const char* file);
void loadMaze(const char* file,float **vArray,int nWalls);

#endif