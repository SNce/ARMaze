#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include"object.h"
#include"matrix.h"
#include"collision.h"

void integrator(float time, Maze *maze, Ball *ball);

#endif