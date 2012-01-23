#include "game.h"
#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif

// Initialize static member variables
ARParam Game::cparam;
char* Game::vconf = "flipH,showDlg,deviceName=Microsoft DV Camera,deinterlaceState=on,deinterlaceMethod=blend,"
	                  "pixelFormat=PIXELFORMAT_RGB32,videoWidth=320,videoHeight=240";
#ifdef _DEBUG
char* Game::patt_name = "../debug/Data/patt.hiro";
char* Game::cparam_name = "../debug/Data/camera_para.dat";
#else
char* Game::patt_name = "Data/patt.hiro";
char* Game::cparam_name = "Data/camera_para.dat";
#endif

int Game::xsize;
int Game::ysize;
int Game::count = 0;
int Game::patt_id;
double Game::patt_width = 80.0;
double Game::patt_centre[2] = {0.0, 0.0};
double Game::patt_trans[3][4];
double Game::ref_trans[3][4];

#ifdef _DEBUG
Maze* Game::maze = new Maze("../debug/maze1.xml");
#else
Maze* Game::maze = new Maze("maze1.xml");
#endif

char* Game::cap_msg = "Hold the pattern horizontally and press C";
char* Game::start_msg = "Press S to start";
float Game::text_color[3] = {0.75, 0.0, 0.0};
float Game::text_pos[3] = {-0.95, -0.20, 0.0};

float Game::abs_gravity[4] = {0.0, 0.0, -1.0, 1.0};
float Game::grav_trans[3] = {0.0, 0.0, 0.0};

int Game::patt_visible = 0;
int Game::ref_set = 0;

float Game::prev_time = 0.0;

void print_string(char *string, float color[3], float pos[3], void *font)
{
  int i;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glTranslatef(pos[0], pos[1], pos[2]);
  glColor3fv(color);
  glRasterPos2i(0.0, 0.0);
  for (i=0; i<(int)strlen(string); i++) {
      if(string[i] != '\n' )
		  glutBitmapCharacter(font, string[i]);
      
      else {
          glTranslatef(0.0, -0.07, 0.0);
          glRasterPos2i(0.0, 0.0);
      }
  }
}

void mult(float lhs[3], double trans_mat[3][4], float rhs[4]) 
{
	float sum;
	for(int i = 0; i < 3;i++)
	{
		sum = 0;
		for(int j = 0; j < 4;j++)
		{
			sum = sum + (trans_mat[i][j] * rhs[j]);
		}
		lhs[i] = sum;
	}
}

void Game::init()
{
	ARParam wparam;
	arVideoDispOption();
	if(arVideoOpen("input_device=WDM_CAP,flipV,showDlg,pixelFormat=PIXELFORMAT_RGB32,videoWidth=320,videoHeight=240") < 0) 
		exit(0);

	if(arVideoInqSize(&xsize,&ysize) < 0) 
		exit(0);

	if(arParamLoad(cparam_name,1,&wparam) < 0)
	{
		printf("[failed]");
		exit(0);
	}

	arParamChangeSize(&wparam,xsize,ysize,&cparam);
	arInitCparam(&cparam);

	if((patt_id = arLoadPatt(patt_name)) < 0)
	{
		printf("[failed]");
		exit(0);
	}

	argInit(&cparam,1.0,0,0,0,0);
}

void Game::mainLoop()
{
	ARUint8 *dataPtr;
	ARMarkerInfo *marker_info;
	int marker_num;
	int j, k;

	if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) 
	{
		arUtilSleep(2);
		return;
	}
	if( count == 0 ) arUtilTimerReset();
	count++;

	argDrawMode2D();
	argDispImage( dataPtr, 0,0 );

	if( arDetectMarker(dataPtr, threshold, &marker_info, &marker_num) < 0 ) 
	{
		patt_visible = 0;
		count = 0;
		prev_time = 0.0;
		cleanup();
		exit(0);
	}

	arVideoCapNext();

	/* check for object visibility */
	k = -1;
	for( j = 0; j < marker_num; j++ ) 
	{
		if( patt_id == marker_info[j].id ) 
		{
			if( k == -1 ) k = j;
			else if( marker_info[k].cf < marker_info[j].cf ) k = j;
		}
	}
	if( k == -1 ) 
	{
		patt_visible = 0;
		count = 0;
		prev_time = 0.0;
		argSwapBuffers();
		return;
	}

	/* get the transformation between the marker and the real camera */
	arGetTransMat(&marker_info[k], patt_centre, patt_width, patt_trans);
	patt_visible = 1;

	if(ref_set == 0)
	{
		print_string(cap_msg, text_color, text_pos, GLUT_BITMAP_HELVETICA_18);
		argSwapBuffers();
		return;
	}
	else
	{
		double inv_trans[3][4];
		double trans_mat[3][4];

		arUtilMatInv(patt_trans, inv_trans);
		arUtilMatMul(inv_trans, ref_trans, trans_mat);
		mult(grav_trans, trans_mat, abs_gravity);

		maze->setGravity(grav_trans);
		float time = arUtilTimer();
		float dt = time - prev_time;
		prev_time = time;
		maze->tick(dt); 
		draw();
		argSwapBuffers();
	}
}

void Game::keyEvent(unsigned char key, int x, int y)
{
	if(key == 0x1b)
	{
		cleanup();
		exit(0);
	}

	if(key == 'c' || key == 'C')
	{
		if(patt_visible == 1 && ref_set == 0)
		{
			for(int i = 0;i < 3;i++)
			{
				for(int j = 0; j < 4; j++)
					ref_trans[i][j] = patt_trans[i][j];
			}
			ref_set = 1;
		}
	}
}

void Game::cleanup()
{
	arVideoCapStop();
	arVideoClose();
	argCleanup();
}

void Game::draw()
{
	double gl_para[16];
	GLfloat mat_ambient[] = {0.6, 0.6, 0.6, 1.0};
	GLfloat mat_flash[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_flash_shiny[] = {50.0};
	GLfloat light1_position[] = {120.0,170.0,50.0,1.0};
	GLfloat light2_position[] = {-100.0,-100.0,5.0,1.0};
	GLfloat ambi[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat lightZeroColor[] = {0.5, 0.5, 0.5, 1.0};

	argDrawMode3D();
	argDraw3dCamera( 0, 0 );
	glClearDepth( 1.0 );
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	argConvGlpara(patt_trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd( gl_para );

	glLightfv(GL_LIGHT0, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);

	//glLightfv(GL_LIGHT1, GL_POSITION, light2_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_flash);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_flash_shiny); 
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMatrixMode(GL_MODELVIEW);

	maze->draw();
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3fv(grav_trans);
	glVertex3f(10.0, 10.0, 0.0);
	glEnd();
	glPopMatrix();
	glDisable( GL_LIGHTING );

	glDisable( GL_DEPTH_TEST );
}