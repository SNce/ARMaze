#include "game.h"

// Initialize static member variables
ARParam Game::cparam;
char* Game::vconf = "flipH,showDlg,deviceName=Microsoft DV Camera,deinterlaceState=on,deinterlaceMethod=blend,"
	                  "pixelFormat=PIXELFORMAT_RGB32,videoWidth=320,videoHeight=240";
char* Game::cparam_name = "Data/camera_para.dat";
char* Game::patt_name = "Data/patt.hiro";
int Game::xsize;
int Game::ysize;
int Game::count = 0;
int Game::patt_id;
double Game::patt_width = 80.0;
double Game::patt_centre[2] = {0.0, 0.0};
double Game::patt_trans[3][4];
Maze* Game::maze = new Maze("maze1.xml");

void Game::init()
{
	ARParam wparam;

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
		argSwapBuffers();
		return;
	}

	/* get the transformation between the marker and the real camera */
	arGetTransMat(&marker_info[k], patt_centre, patt_width, patt_trans);

	draw();

	argSwapBuffers();
}

void Game::keyEvent(unsigned char key, int x, int y)
{
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
	GLfloat mat_ambient[] = {0.1, 0.1, 1.0, 1.0};
	GLfloat mat_flash[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_flash_shiny[] = {50.0};
	GLfloat light_position[] = {50.0,-20.0,20.0,0.0};
	GLfloat ambi[] = {0.1, 0.1, 0.1, 0.1};
	GLfloat lightZeroColor[] = {0.9, 0.9, 0.9, 0.1};

	argDrawMode3D();
	argDraw3dCamera( 0, 0 );
	glClearDepth( 1.0 );
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	argConvGlpara(patt_trans, gl_para);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd( gl_para );

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny); 
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMatrixMode(GL_MODELVIEW);
	/*glTranslatef( 0.0, 0.0, 25.0 );*/
	/*glutSolidCube(50.0);*/
	maze->draw();
	glDisable( GL_LIGHTING );

	glDisable( GL_DEPTH_TEST );
}