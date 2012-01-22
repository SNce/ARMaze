#include "object.h"
#include <string>
#include <fstream>
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
	ball->tick(t);
}

void Maze::setGravity(float* g_array)
{
	gravity = Vector3(g_array);
	gravity.setMagnitude(9.8);
	ball->setGravity(gravity.getValue());
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
//#ifdef _DEBUG
//	cout<<netForce.x<<"  "<<netForce.y<<"  "<<netForce.z<<endl;
//#endif

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
}

Wall::~Wall()
{
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