///////////////////////////////////////////////////////////////
//
//			Augmented Reality Game
//						-C Krishnakanth Mallik
//
//
///////////////////////////////////////////////////////////////

//*************************		STL,OS headers

#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>

//*************************		OpenGL

#include<GL/gl.h>
#include<GL/glut.h>

//*************************		ARToolkit

#include<AR/ar.h>
#include<AR/gsub.h>
#include<AR/video.h>
#include<AR/param.h>

//*************************		Timing

#include<time.h>

//*************************		Custom headers

//#include"interface.h"
#include"object.h"
#include"loader.h"
#include"collision.h"
#include"physics.h"
#include"matrix.h"
#include"graphics.h"

using namespace std;

char *vconf = "showDlg,deviceName=Microsoft DV Camera,deinterlaceState=on,deinterlaceMethod=blend,"
                   "pixelFormat=PIXELFORMAT_RGB32,videoWidth=320,videoHeight=240";

char *cparam_name = "Data/camera_para.dat";
char *patt_name = "Data/patt.hiro";

ARParam cparam;
int xsize,ysize;
int count = 0;
int thresh = 100;

float initial_pos[3] = {0.0,0.0,5.0};
float force[3] = {20000.0,20000.0,10000.0};
float **vArray;
int nWalls;

static clock_t current_frame;
static clock_t previous_frame = 0;
static clock_t elapsed;
float time_elapsed;

//******	Objects
Maze *maze;
Ball *ball;

//******	Display Lists
GLuint mazeList;
GLuint ballList;

int patt_id;
double patt_width = 80.0;
double patt_centre[2] = {0.0,0.0};
double patt_trans[3][4];
double inv_trans[3][4];
double trans_mat[3][4];
double ref_trans[3][4];
int ref_set; 
int patt_visible;
int running;
char cap[256];
char play[256];

float grav_array[] = {0.0,0.0, SCALE * -9.807};
float grav_array2[4] = {0.0,0.0, SCALE * -9.807,1};
a3d::vector3<float> gravity;
float grav_trans[3];
float cap_color[3] = {0.75,0.0,0.0};
float cap_pos[3] = {-0.95,-0.20,0.0};

static void init();
static void cleanup();
static void mainLoop();
static void keyEvent(unsigned char key,int x,int y);
static void draw();
static void drawAxes();			// reference purpose

// Main-------------
int main(int args,char **argv)
{
	init();
	arVideoCapStart();
	argMainLoop(NULL,keyEvent,mainLoop);
	return 0;
}

static void init()
{
	ARParam wparam;

	ref_set = 0;
	patt_visible = 0;
	running = 0;

	sprintf(cap,"Press c to capture reference");
	sprintf(play,"Press s to Start");

	if(arVideoOpen(vconf) < 0) 
		exit(0);

	if(arVideoInqSize(&xsize,&ysize) < 0) 
		exit(0);

	printf("\nLoading Camera Parameters...");
	if(arParamLoad(cparam_name,1,&wparam) < 0)
	{
		printf("[failed]");
		exit(0);
	}
	printf("[ok]");

	arParamChangeSize(&wparam,xsize,ysize,&cparam);
	arInitCparam(&cparam);

	printf("\nLoading Pattern...");
	if((patt_id = arLoadPatt(patt_name)) < 0)
	{
		printf("[failed]");
		exit(0);
	}
	printf("[ok]");

	argInit(&cparam,1.0,0,0,0,0);

	printf("\nLoading Maze...");
	nWalls = getNumWalls("basic.txt");
	if(nWalls < 0)
	{
		printf("[failed]");
		exit(0);
	}

	vArray = new float*[4 * nWalls];
	for(int i = 0;i< (4 * nWalls);i++)
		vArray[i] = new float[3];

	if(loadMaze("basic.txt",vArray,nWalls) < 0)
	{
		printf("[failed]");
		exit(0);
	}
	printf("[ok]");

	printf("\nInitializing Objects...");
	mazeList = glGenLists(4);
	maze = new Maze(vArray,nWalls,mazeList);

	//maze->DrawNormals();

	ballList = glGenLists(4);
	ball = new Ball(initial_pos,5.0,100,100,1.0,1,ballList);

	//a3d::vector3<float> gravity(grav_array);
	//ball->calcGravity(gravity);
	printf("[ok]");

	printf("\n\n Set up the reference pattern parallel to the ground and press ""c"" to capture.");

	count = 0;
}

static void cleanup()
{
	arVideoCapStop();
	arVideoClose();
	argCleanup();
	
	for(int i = 0;i < (4 * nWalls);i++)
		delete[] vArray[i];

	glDeleteLists(mazeList,3);
	glDeleteLists(ballList,3);

	delete maze;
	delete ball;
}

static void mainLoop()
{
	current_frame = clock();
	elapsed = current_frame - previous_frame;
	previous_frame = current_frame;
	time_elapsed = (float)elapsed / CLOCKS_PER_SEC;

	ARUint8 *image;

	ARMarkerInfo *markerInfo;
	int markerNum;
	int j,k;

	if((image = (ARUint8 *)arVideoGetImage()) == NULL)
	{
		arUtilSleep(2);
		return;
	}

	if(count == 0)
		arUtilTimerReset();
	count++;

	argDrawMode2D();
	argDispImage(image,0,0);

	if(arDetectMarker(image,thresh,&markerInfo,&markerNum) < 0)
	{
		cleanup();
		exit(0);
	}

	arVideoCapNext();

	k = -1;
	for(j = 0;j < markerNum;j++)
	{
		if(patt_id == markerInfo[j].id)
		{
			if(k == -1)
				k = j;
			else if(markerInfo[k].cf < markerInfo[j].cf)
				k = j;
		}
	}
		if(k == -1)
		{
			patt_visible = 0;
			argSwapBuffers();
			return;
		}
		arGetTransMatCont(&markerInfo[k],patt_trans,patt_centre,patt_width,patt_trans);
		patt_visible = 1;

		if(ref_set == 0)
		{
			print_string(cap,cap_color,cap_pos,GLUT_BITMAP_HELVETICA_18);
			argSwapBuffers();
			return;
		}

		else if(ref_set == 1)
		{
			if(running == 0)
			{
				print_string(play,cap_color,cap_pos,GLUT_BITMAP_HELVETICA_18);
			}

			else if(running == 1)
			{
				arUtilMatInv(patt_trans,inv_trans);
				arUtilMatMul(inv_trans,ref_trans,trans_mat);

				mult(grav_trans,trans_mat,grav_array2);

				gravity.set_value(grav_trans);
				ball->calcGravity(gravity);

				ball->addForce(ball->f_gravity);
				//a3d::vector3<float> init_force(force);						// initial force (debug)
				//static int m = 0;
				//if(m < 1 )
				//ball->addForce(init_force);
				integrator(time_elapsed,maze,ball);
				draw();
				print_time(time_elapsed);
		
				//m++;
			}
		}
		argSwapBuffers();
}

static void keyEvent(unsigned char key,int x,int y)
{
	if(key == 0x1b)
	{
		cleanup();
		exit(0);
	}

	if(key == 'c')
	{
		if(patt_visible == 1 && ref_set == 0)
		{
			for(int i = 0;i < 3;i++)
			{
				for(int j = 0;j < 4;j++)
					ref_trans[i][j] = patt_trans[i][j];
			}
			ref_set = 1;
			printf("\nCaptured!");
		}
	}

	if(key == 's')
	{
		if(ref_set == 1)
			running = 1;
	}
}

static void draw()
{	
	double gl_para[16];
	GLfloat light_pos[] = {300.0,-300.0,300.0,0.0};
	GLfloat light_amb[] = {0.2,0.2,0.2,1.0};
	GLfloat light_dif[] = {0.4,0.4,0.4,1.0};
	GLfloat light_spe[] = {0.7,0.7,0.7,1.0};

	GLfloat mat_ambient[] = {1.0,1.0,1.0,1.0};
	GLfloat mat_flash[] = {0.7,0.7,0.7,1.0};

	argDrawMode3D();
	argDraw3dCamera(0,0);
	//glClearDepth(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	argConvGlpara(patt_trans,gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(gl_para);

	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_amb);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_dif);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_spe);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_flash);

	glMatrixMode(GL_MODELVIEW);

	maze->Draw();
	maze->DrawNormals();
	ball->Draw();

	//drawAxes();

	//glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

}

static void drawAxes()		// reference purpose
{
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex3i(0,0,0);
		glVertex3i(100,0,0);

		glColor3f(0.0,1.0,0.0);
		glVertex3i(0,0,0);
		glVertex3i(0,100,0);

		glColor3f(0.0,0.0,1.0);
		glVertex3i(0,0,0);
		glVertex3i(0,0,100);
	glEnd();
}