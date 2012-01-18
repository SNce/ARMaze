#include"object.h"

//----------------------Object------------------------

Object::Object(bool solid)
{
	this->isSolid = solid;
}

Wall::Wall(float** vertices,bool solid):Object(solid)
{
	for(int i = 0;i < 4;i++)
	{
		this->vertex[i].x = vertices[i][0];
		this->vertex[i].y = vertices[i][1];
		this->vertex[i].z = vertices[i][2];
	}

	this->k = 1.0;
	this->prev_k = 1.0;

#ifdef __DEBUG
	printf("-------------The Wall vertex data---------\n");
	for(int i = 0;i < 4;i++)
	{
		printf("%f ,%f, %f \n",vertex[i].x,vertex[i].y,vertex[i].z);
	}
#endif
}

//----------------------Wall------------------------

void Wall::calcNormals()
{
	a3d::vector3<float> vec1;
	a3d::vector3<float> vec2;
	a3d::vector3<float> n1;
	a3d::vector3<float> n2;

	vec1 = this->vertex[1] - this->vertex[0];
	vec2 = this->vertex[2] - this->vertex[1];

	n1 = cross(vec2,vec1);

	n1.normalize();

	n2 = -n1;

	printf("\n1 = %f, %f , %f",n1.x,n1.y,n1.z);

	this->normal1.x = n1.x;
	this->normal1.y = n1.y;
	this->normal1.z = n1.z;

	this->normal2.x = n2.x;
	this->normal2.y = n2.y;
	this->normal2.z = n2.z;
}

void Wall::calcD()
{
	this->offset = dot(this->vertex[0],this->normal1);
	//printf("\nd = %f",this->offset);
}

//----------------------Maze------------------------

Maze::Maze(float **vArray,int nWalls,GLuint mazeList)
{
	int i,k;

	this->nWalls = nWalls;
	this->list = mazeList;

	printf("\n nWalls = %d",this->nWalls);

	wall = new Wall*[nWalls];
	float **vertexArray;

	vertexArray = new float*[4];
	for(i = 0;i < 4;i++)
		vertexArray[i] = new float[3];

	i = 0;
	k = 0;


	while(i < (4 * nWalls))
	{	
		for(int j = 0;j < 4;j++)
		{
			vertexArray[j][0] = vArray[i][0];
			vertexArray[j][1] = vArray[i][1];
			vertexArray[j][2] = vArray[i][2];
			i++;
		}

		if( i % 4 == 0)
		{
			wall[k] = new Wall(vertexArray,1);
			wall[k]->id = k+1;
			k++;
		}
	}

	for(i = 0;i < 4;i++)
		delete[] vertexArray[i];

	this->createList();
	this->calcNormals();
}

int Maze::createList()
{
	glNewList(list,GL_COMPILE);
		glBegin(GL_QUADS);
		for(int i = 0;i < nWalls;i++)
		{
			glNormal3f(this->wall[i]->normal1.x,this->wall[i]->normal1.y,this->wall[i]->normal1.z);
			for(int j = 0;j < 4;j ++)
			{
				glVertex3f(this->wall[i]->vertex[j].x,this->wall[i]->vertex[j].y,this->wall[i]->vertex[j].z);
			}
		}
		glEnd();
	glEndList();
	return 1;
	
}

int Maze::calcNormals()
{
	for(int i = 0;i < this->nWalls;i++)
	{
		this->wall[i]->calcNormals();
		this->wall[i]->calcD();
	}
	return 1;
}

void Maze::Draw()
{
	glColor3f(0.5,0.5,0.5);
	glPushMatrix();
	glCallList(this->list);
	glPopMatrix();
}

void Maze::DrawNormals()
{
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINES);
	for(int i = 0;i < this->nWalls;i++)
	{
		a3d::vector3<float> res;
		a3d::vector3<float> a;

		a.x = (this->wall[i]->vertex[0].x + this->wall[i]->vertex[2].x)/2;
		a.y = (this->wall[i]->vertex[0].y + this->wall[i]->vertex[2].y)/2;
		a.z = (this->wall[i]->vertex[0].z + this->wall[i]->vertex[2].z)/2;

		glVertex3f(a.x,a.y,a.z);
		res = a + (this->wall[i]->normal1) * 40;
		glVertex3f(res.x,res.y,res.z);
	}
	glEnd();
}

//----------------------Ball------------------------

Ball::Ball(float *initial_pos,float r,int st,int sl,float m, bool solid,int l):Object(solid)
{
	this->radius = r;
	this->stacks = st;
	this->slices = sl;
	this->mass = m;

	if(this->mass != 0)
		this->inv_mass = (float)1/m;

	this->list = l;

	this->position.x = initial_pos[0];
	this->position.y = initial_pos[1];
	this->position.z = initial_pos[2];

	this->drag = 0.995;
	this->clearForceAccum();
	this->clearAccel();
	this->clearVelocity();
}

int Ball::createList()
{
	glNewList(this->list,GL_COMPILE);
		glutSolidSphere(this->radius,this->slices,this->stacks);
	glEndList();
	return 1;
}

void Ball::addForce(a3d::vector3<float> force)
{
	forceAccum += force;
}

void Ball::clearForceAccum()
{
	this->forceAccum.x = 0.0;
	this->forceAccum.y = 0.0;
	this->forceAccum.z = 0.0;
}

void Ball::clearAccel()
{
	this->accel.x = 0.0;
	this->accel.y = 0.0;
	this->accel.z = 0.0;
}

void Ball::clearVelocity()
{
	this->velocity.x = 0.0;
	this->velocity.y = 0.0;
	this->velocity.z = 0.0;
}

void Ball::calcGravity(a3d::vector3<float> gravity)
{
	this->f_gravity = mass * gravity;
}

void Ball::Draw()
{
	glColor3f(1.0,0.0,0.0);
	glPushMatrix();
		glTranslatef(this->position.x,this->position.y,this->position.z);
		glutSolidSphere(this->radius,this->stacks,this->slices);
	glPopMatrix();
}