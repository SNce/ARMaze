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
//// Object
//class Object
//{
//public:
//	bool isSolid;
//
//	Object(bool solid);
//};
//
//// Wall
//
//class Wall : public Object
//{
//public:
//
//	int id;
//
//	float offset;				// offset of the plane
//
//	float k;				//decision parameter (used in collision detection
//	float prev_k;			//previous value of decision parameter 
//
//	a3d::vector3<float> vertex[4];
//	a3d::vector3<float> normal1;
//	a3d::vector3<float> normal2;
//
//	Wall(float** vertices,bool soild);
//	void calcD();
//	
//	void calcNormals();
//};
//
//// Maze
//class Maze
//{
//public:
//	Wall **wall;
//	int nWalls;
//	GLuint list;
//	
//	Maze(float **vArray,int nWalls,GLuint mazeList);
//	int createList();
//	int calcNormals();
//	void DrawNormals();		// Debug Purposes
//	void Draw();
//	
//};
//
//// Ball
//
//class Ball : public Object
//{
//public:
//	float radius;
//	int stacks;
//	int slices;
//	int list;
//	float drag;
//
//	a3d::vector3<float> position;
//
//	a3d::vector3<float> f_gravity;
//	a3d::vector3<float> forceAccum;
//	a3d::vector3<float> accel;
//	a3d::vector3<float> velocity;
//
//	float mass;
//	float inv_mass;
//
//	Ball(float *initial_pos,float r, int st,int sl,float m,bool solid,int l);
//	
//	int createList();
//	void addForce(a3d::vector3<float> force);
//	void clearForceAccum();
//	void clearAccel();
//	void clearVelocity();
//	void calcGravity(a3d::vector3<float> gravity);
//	void Draw();
//};
//#endif

#ifndef _OBJECT
#define _OBJECT

#include "vect.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/GL.h>
#include <list>
#include <rapidxml\rapidxml.hpp>

#define SCALE 1.0

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
	void draw();
};

// Wall
class Wall
{
private:
	Vector3 normal;
	Vector3* vertices;
public:
	Wall(Vector3* v);
	~Wall();
	void calcNormal();
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
	void draw();
	void setGravity(float* g_array);
};



#endif