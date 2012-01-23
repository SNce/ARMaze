#include "object.h"
#include <string>
#include <fstream>
#include <float.h>
#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;


Maze::Maze(char* mazeFile)
{
	// Read the xml map file.
	char* fileText;
	ifstream file;
	string buffer;
	string line;

	file.open(mazeFile, ios::in);

	if(file.is_open())
	{
		while(getline(file, line))
			buffer += line;	
	}	

	fileText = new char[buffer.size() + 1];
	copy(buffer.begin(), buffer.end(), fileText);
	fileText[buffer.size()] = '\0';

	file.close();

	using namespace rapidxml;

	xml_document<> doc;
	doc.parse<0>(fileText);

	for(xml_node<>* wall = doc.first_node()->first_node(); wall; wall = wall->next_sibling())
	{
		Vector3* vertices;
		vertices = new Vector3[4];

		int i = 0;
		for(xml_node<>* vertex = wall->first_node(); vertex; vertex = vertex->next_sibling())
		{
			float c[3];

			xml_attribute<>* coordinate = vertex->first_attribute();

			c[0] = atof(coordinate->value());
			coordinate = coordinate->next_attribute();
			c[1] = atof(coordinate->value());
			coordinate = coordinate->next_attribute();
			c[2] = atof(coordinate->value());

			*(vertices + i) = Vector3(c);
			i++;
		}

		Wall* wall = new Wall(vertices);
		walls.push_back(wall);
	}

	ball = new Ball(5.0, 1.0);
}

Maze::~Maze()
{
}

void Maze::tick(float t)
{
	std::list<Wall*> c_walls;
	collisions(c_walls);
	resolveCollisions(c_walls, t);
	ball->tick(t);
}

void Maze::setGravity(float* g_array)
{
	gravity = Vector3(g_array);
	gravity.setMagnitude(9.8);
	ball->setGravity(gravity.getValue());
}

void Maze::collisions(std::list<Wall*>& c_walls)
{	
	std::list<Wall*>::iterator it;
	for(it = this->walls.begin(); it != this->walls.end(); it++)
	{
		float dist;
		Vector3 rel_pos;
		Vector3 vert;

		vert = (*it)->getVertices()[0];

		rel_pos = ball->getPosition() - vert;

		dist = abs(dot((*it)->getNormal(), rel_pos));

		// within bounds of the wall
		if((*it)->inBounds(ball->getPosition()))
		{
			// touching or more! :P
			if(dist - (ball->getRadius() + THRESHOLD) <= 0)
				c_walls.push_back(*it);
		}
	}
}

void Maze::resolveCollisions(std::list<Wall*>& c_walls, float t)
{
	if(!c_walls.empty())
	{
		list<Wall*>::iterator it;
		for(it = c_walls.begin(); it != c_walls.end(); it++)
		{
			//Velocity component along normal.
			float v_component = dot(ball->getVelocity(), (*it)->getNormal());
		
			if(v_component > 0)
			{
				(*it)->correctNormal();
				v_component = -v_component;
			}

			// Velocity along normal.
			Vector3 normal = (*it)->getNormal();
			Vector3 n_velocity;
			n_velocity = normal * v_component;

			// Impusle calculation.
			n_velocity = -n_velocity;
			n_velocity *= 2;
			Vector3 n_force;
			n_force = n_velocity * ball->getMass();
			n_force /= t;
			ball->addForce(n_force);
		}
	}
}

void Maze::draw()
{
	float f_v1[3] = {-100.0, -160.0, 0.0};
	float f_v2[3] = {-100.0, 160.0, 0.0};
	float f_v3[3] = {100.0, 160.0, 0.0};
	float f_v4[3] = {100.0, -160.0, 0.0};

	glColor3f(0.7,0.6,0.6);

	glBegin(GL_QUADS);
	glVertex3fv(f_v1);
	glVertex3fv(f_v2);
	glVertex3fv(f_v3);
	glVertex3fv(f_v4);
	glEnd();

	list<Wall*>::iterator it;
	for(it = walls.begin(); it != walls.end(); it++)
		(*it)->draw();

	ball->draw();
}

Ball::Ball(float r, float m)
{
	radius = r;
	mass = m;
	netForce = Vector3(0.0);
	acceleration = Vector3(0.0);
	velocity = Vector3(0.0);
	position = Vector3(0.0, 0.0, 5.0);
}

Ball::~Ball()
{
}

void Ball::tick(float t)
{
	// gravitational force
	Vector3 gForce;
	Vector3 dv;
	Vector3 ds1, ds2;

	gForce = gravity * (mass * SCALE);
	netForce += gForce;

	acceleration = netForce / mass;
	dv = acceleration * t;
	velocity += dv;
	ds1 = velocity * t;
	ds2 = (0.5 * t * t) * acceleration;
	position += ds1;
	position += ds2;

	//hack to keep the ball on surface
	position.z = 5.0;
	netForce = Vector3(0.0);
}

void Ball::setGravity(float* g_array)
{
	gravity = Vector3(g_array);
}

void Ball::addForce(Vector3& force)
{
	netForce += force;
}

Vector3& Ball::getPosition()
{
	return position;
}

float Ball::getRadius()
{
	return radius;
}

float Ball::getMass()
{
	return mass;
}

Vector3& Ball::getVelocity()
{
	return velocity;
}

void Ball::draw()
{
	glColor3f(1.0, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glutSolidSphere(radius, 100, 100);
	glPopMatrix();
}

Wall::Wall(Vector3* v)
{
	vertices = new Vector3[4];

	vertices[0] = Vector3(v[0]);
	vertices[1] = Vector3(v[1]);
	vertices[2] = Vector3(v[2]);
	vertices[3] = Vector3(v[3]);

	minX = vertices[0].x;
	maxX = vertices[0].x;
	minY = vertices[0].y;
	maxY = vertices[0].y;

	for(int i = 1; i < 4; i++)
	{
		minX = min(minX, vertices[i].x);
		minY = min(minY, vertices[i].y);
		maxX = max(maxX, vertices[i].x);
		maxY = max(maxY, vertices[i].y);
	}

	calcNormal();
}

Wall::~Wall()
{
}

void Wall::calcNormal()
{
	Vector3 v0 = Vector3(vertices[0]);
	Vector3 v1 = Vector3(vertices[1]);
	Vector3 v2 = Vector3(vertices[2]);

	Vector3 a, b;
	a = v1 - v0;
	b = v2 - v0;

	normal = cross(a, b);
	normal.normalize();
}

Vector3& Wall::getNormal()
{
	return this->normal;
}

Vector3* Wall::getVertices()
{
	return vertices;
}

void Wall::correctNormal()
{
	normal = -normal;
}

bool Wall::inBounds(Vector3& pos)
{
	bool rv = true;

	if(minX != maxX)
	{
		rv &= pos.x >= minX;
		rv &= pos.x <= maxX;
	}

	if(minY != maxY)
	{
		rv &= pos.y >= minY;
		rv &= pos.y <= maxY;
	}

	return rv;
}

void Wall::draw()
{
	glColor3f(0.6,0.5,0.5);

	glBegin(GL_QUADS);
	glVertex3fv(vertices[0]._array);
	glVertex3fv(vertices[1]._array);
	glVertex3fv(vertices[2]._array);
	glVertex3fv(vertices[3]._array);
	glEnd();
}