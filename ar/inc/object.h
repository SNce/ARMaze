//#ifndef _OBJECT_H_
//#define _OBJECT_H_
//
//#include"vector.h"
//#include<stdio.h>
//#include<GL/glut.h>
//
//#define SCALE (float)1077.4410775
//#define DAMPING (float)0.7000
//#define NEU (float)0.7000
//#define MIN_PRECISION (float)0.000001
//
#ifndef _OBJECT
#define _OBJECT

#include "vect.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/GL.h>
#include <list>
#include <rapidxml\rapidxml.hpp>

#define SCALE 5.0
#define DAMP 0.8
#define COEFF_FRIC 0.6
#define THRESHOLD 3.0

// Ball
class Ball
{
private:
	float radius;
	float mass;
	Vector3 gravity;
	Vector3 netForce;
	Vector3 acceleration;
	Vector3 velocity;
	Vector3 position;
public:
	Ball(float r, float m);
	~Ball();

	void tick(float t);
	void setGravity(float* g_array);
	void addForce(Vector3& force);
	void damp();
	Vector3& getPosition();
	Vector3& getVelocity();
	float getRadius();
	float getMass();
	void draw();
};

// Wall
class Wall
{
private:
	Vector3 normal;
	Vector3* vertices;
	float minX, maxX;
	float minY, maxY;
public:
	Wall(Vector3* v);
	~Wall();
	void calcNormal();
	Vector3& getNormal();
	Vector3* getVertices();
	void correctNormal();
	bool inBounds(Vector3& pos);
	void draw();
};

// Maze
class Maze
{
private:
	Vector3 gravity;
	Ball* ball;
	std::list<Wall*> walls;
public:
	Maze(char* mazeFile);
	~Maze();
	void tick(float t);
	void setGravity(float* g_array);
	void collisions(std::list<Wall*>& c_walls);
	void resolveCollisions(std::list<Wall*>& collisions, float t);
	void draw();
};

#endif