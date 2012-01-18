#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "object.h"

void update(Maze *maze,Ball *ball);
int collision(Maze *maze, Ball *ball, float time);
void getOppParallelVect(a3d::vector3<float> vect,a3d::vector3<float>,a3d::vector3<float> para);

#endif