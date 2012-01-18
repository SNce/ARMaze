 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:	 OpenGL/GLUT shared memory buffer sample
  * Version: 0.0.3 (02/21/2003)
  * ========================================================================
  * Author:  Thomas Pintaric, Vienna University of Technology
  * Contact: pintaric@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
  * =======================================================================
  *
  * Copyright (C) 2003  Vienna University of Technology
  * 
  * OpenGL framework by NeHe (nehe.gamedev.net)
  *	GLUT port by milix (milix_gr@hotmail.com)
  *
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  *
  * For further information please contact Thomas Pintaric under
  * <pintaric@ims.tuwien.ac.at> or write to Thomas Pintaric,
  * Vienna University of Technology, Favoritenstr. 9-11/188, A1040 Vienna,
  * Austria.
  * ========================================================================*/

 /*
  *		This sample demonstrates the use of a shared memory buffer to access
  *		video frames.
  *
  */

#define SHOW_ROTATING_CUBE

#ifdef _DEBUG
	static unsigned long frame_counter = 0;
#endif


#include "DsVideoLib.h"		 // DirectShow Video Processing Library

#include <stdio.h>           // Standard C/C++ Input-Output
#include <windows.h>         // Standard Header For MSWindows Applications
#include <gl/glut.h>         // The GL Utility Toolkit (GLUT) Header

// The Following Directive Fixes The Problem With Extra Console Window
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

//---------------------------------------------------------------------------
CGraphManager *graphManager = NULL;

BYTE* g_pPixelBuffer = NULL;
REFERENCE_TIME g_Timestamp = _INVALID_TIMESTAMP;

unsigned int cap_width = 0;
unsigned int cap_height = 0;
double		 cap_fps = 0.0;

//---------------------------------------------------------------------------

#define TEXTURES_NUM 1       // We Have 1 Texture 
#define CUBE_TEX     0       // Our Cube Texture Index

// A Structure For RGB Bitmaps
typedef struct _RGBIMG {
	GLuint   w;    // Image's Width
	GLuint   h;    // Image's Height
	GLubyte* data; // Image's Data (Pixels)
} RGBIMG;

const GLuint TEX_WIDTH = 1024;	// texture should be large enough to store full-size
const GLuint TEX_HEIGHT =1024;  // PAL video frames (720x576px)

// Global Variables
bool    g_gamemode;            // GLUT GameMode ON/OFF
bool    g_fullscreen;          // Fullscreen Mode ON/OFF (When g_gamemode Is OFF)
GLfloat	g_xrot = 0.0f;         // X Rotation
GLfloat	g_yrot = 0.0f;         // Y Rotation
GLfloat	g_zrot = 0.0f;         // Z Rotation
GLuint	g_texid[TEXTURES_NUM]; // Our Textures' Id List

const bool showFullscreenDialog = false;

#ifdef SHOW_ROTATING_CUBE
	bool showRotatingCube = true;
#else
	bool showRotatingCube = false;
#endif
//---------------------------------------------------------------------------
bool setup_dsWrapper(char* lpCmdLine = "")
{
	CoInitialize(NULL);
	graphManager = new CGraphManager();

//	char* default_param = "inputDevice=AVI_FILE,fileName=sample.avi,renderSec,pixelFormat=PIXELFORMAT_RGB32";
	char* default_param = "inputDevice=WDM_CAP,showDlg,pixelFormat=PIXELFORMAT_RGB32";
	if(FAILED(graphManager->BuildGraph(strlen(lpCmdLine) > 0 ? lpCmdLine : default_param))) return(false);

	DS_MEDIA_FORMAT mf;
	graphManager->GetCurrentMediaFormat(&mf);
	cap_width =	 mf.biWidth;
	cap_height = mf.biHeight;
	cap_fps =	 mf.frameRate;

	if(FAILED(graphManager->EnableMemoryBuffer())) return(false);
	if(FAILED(graphManager->Run())) return(false);
	return(true);
}

//---------------------------------------------------------------------------
// Setup Our Textures. Returns true On Success, false On Fail
bool setup_textures()
{
	RGBIMG img = {TEX_WIDTH,TEX_HEIGHT,NULL};
	long data_size = 4 * sizeof(GLubyte) * img.h * img.w;
	img.data = (GLubyte*) malloc(data_size);
	FillMemory(img.data,data_size,0x7f);

	// Create The Textures' Id List
	glGenTextures(TEXTURES_NUM, g_texid);          
	// Typical Texture Generation Using Data From The Image
	glBindTexture(GL_TEXTURE_2D, g_texid[CUBE_TEX]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, img.w, img.h, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.data);
	// Finished With Our Image, Free The Allocated Data
	delete img.data;
	return true;
}

// Our GL Specific Initializations. Returns true On Success, false On Fail.
bool init(void)
{
    glShadeModel(GL_SMOOTH);						   // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);			   // Black Background
    glClearDepth(1.0f);								   // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);						   // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);							   // The Type Of Depth Testing To Do
	if (!setup_textures()) return false;
	glEnable(GL_TEXTURE_2D);                           // Enable Texture Mapping
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);             // Pixel Storage Mode To Byte Alignment
    glEnable(GL_CULL_FACE);                            // Cull Polygons
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return true;
}

// Our Rendering Is Done Here
void render(void)   
{
	DWORD wait_result = graphManager->WaitForNextSample(100/60);
	if(wait_result == WAIT_OBJECT_0)
	{
		#ifdef _DEBUG
		frame_counter++;
		#endif
		
		graphManager->Lock();
		graphManager->CheckoutMemoryBuffer(&g_pPixelBuffer);
		glBindTexture(GL_TEXTURE_2D, g_texid[CUBE_TEX]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cap_width, cap_height,
						GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)g_pPixelBuffer);

		g_Timestamp = graphManager->GetCurrentTimestamp();
		graphManager->CheckinMemoryBuffer();
		graphManager->Unlock();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	float u_rt = (float)cap_width /  TEX_WIDTH;
	float v_rt = (float)cap_height / TEX_HEIGHT;

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, g_texid[CUBE_TEX]);
	glBegin(GL_QUADS);
		// video backplate
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.0f);
		glTexCoord2f(u_rt, 0.0f); glVertex3f( 1.0f, -1.0f,  0.0f);
		glTexCoord2f(u_rt, v_rt); glVertex3f( 1.0f,  1.0f,  0.0f);
		glTexCoord2f(0.0f, v_rt); glVertex3f(-1.0f,  1.0f,  0.0f);
	glEnd();
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glTranslatef(0.0f,0.0f,-5.0f);

	glRotatef(g_xrot,1.0f,0.0f,0.0f);
	glRotatef(g_yrot,0.0f,1.0f,0.0f);
	glRotatef(g_zrot,0.0f,0.0f,1.0f);

	glBindTexture(GL_TEXTURE_2D, g_texid[CUBE_TEX]);

	if(showRotatingCube)
	{
	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(u_rt, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(u_rt, v_rt); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, v_rt); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glTexCoord2f(u_rt, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(u_rt, v_rt); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, v_rt); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glTexCoord2f(0.0f, v_rt); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(u_rt, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(u_rt, v_rt); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glTexCoord2f(u_rt, v_rt); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, v_rt); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(u_rt, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		glTexCoord2f(u_rt, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(u_rt, v_rt); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, v_rt); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(u_rt, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(u_rt, v_rt); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, v_rt); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
	}

	g_xrot += 0.3f;
	g_yrot += 0.2f;
	g_zrot += 0.4f;

    // Swap The Buffers To Become Our Rendering Visible
    glutSwapBuffers ( );
}

// Our Reshaping Handler (Required Even In Fullscreen-Only Modes)
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix
	glLoadIdentity();                // Reset The Projection Matrix
	// Calculate The Aspect Ratio And Set The Clipping Volume
	if (h == 0) h = 1;
	gluPerspective(50, (float)w/(float)h, 1.0, 5000.0);
	glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix
}

// Our Keyboard Handler (Normal Keys)
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:        // When Escape Is Pressed...
			graphManager->Stop();

			/*
			#ifdef _DEBUG
			char msg[MAX_PATH];
			sprintf(msg,"frames: %i\n",frame_counter);
			MessageBox(NULL,msg,"DEBUG",MB_OK);
			#endif
			*/

			delete graphManager;
			exit(0);    // Exit The Program
			break;      // Ready For Next Case
		
		case 99:        // 'c'
		case 67:        // 'C'
			showRotatingCube = !showRotatingCube;
			break;      

		default:        // Now Wrap It Up
		break;
	}
}

// Our Keyboard Handler For Special Keys (Like Arrow Keys And Function Keys)
void special_keys(int a_keys, int x, int y)
{
	switch (a_keys) {
		case GLUT_KEY_F1:
			// We Can Switch Between Windowed Mode And Fullscreen Mode Only
			if (!g_gamemode) {
				g_fullscreen = !g_fullscreen;       // Toggle g_fullscreen Flag
				if (g_fullscreen) glutFullScreen(); // We Went In Fullscreen Mode
				else 
				{
					glutReshapeWindow(640, 480);   // We Went In Windowed Mode
					glutPositionWindow(80, 60);
				}
			}
		break;
		default:
		break;
	}
}

// Ask The User If He Wish To Enter GameMode Or Not
// Ask The User If He Wish To Enter GameMode Or Not
void ask_gamemode()
{
	if(showFullscreenDialog)
	{
		int answer;
		// Use Windows MessageBox To Ask The User For Game Or Windowed Mode
		answer = MessageBox(NULL, "Do you want to enter exclusive mode?\n(a.k.a. 'game mode')", "Display Question",
							MB_ICONQUESTION | MB_YESNO);
		g_gamemode = (answer == IDYES);
		// If Not Game Mode Selected, Use Windowed Mode (User Can Change That With F1)
	} else g_gamemode = false;
	g_fullscreen = false; 
}

// Main Function For Bringing It All Together.
int main(int argc, char** argv)
{
	ask_gamemode();                                  // Ask For Fullscreen Mode
	glutInit(&argc, argv);                           // GLUT Initializtion
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE); // (CHANGED)
	if (g_gamemode) {
		glutGameModeString("640x480:32");            // Select The 640x480 In 32bpp Mode
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
			glutEnterGameMode();                     // Enter Full Screen
		else g_gamemode = false;                     // Cannot Enter Game Mode, Switch To Windowed
	}
	if (!g_gamemode) {
		glutInitWindowSize(640, 480);                // Window Size If We Start In Windowed Mode
		glutCreateWindow("DirectShow VideoCapture OpenGL/GLUT Sample, "
						 "© 2002 Thomas Pintaric");	 // Window Title 
	}
	if (!init()) {                                   // Our Initialization
		MessageBox(NULL,"Cannot initialize textures.","Error",MB_ICONSTOP);
		return -1;
	}

	if (!setup_dsWrapper(argc > 1 ? argv[1] : "")) {        // DirectShow Capture Graph Initialization
		MessageBox(NULL,"Cannot initialize capture graph.","Error",MB_ICONSTOP);
		return -1;
	}

	glutDisplayFunc(render);                         // Register The Display Function
	glutReshapeFunc(reshape);                        // Register The Reshape Handler
	glutKeyboardFunc(keyboard);                      // Register The Keyboard Handler
	glutSpecialFunc(special_keys);                   // Register Special Keys Handler
	glutIdleFunc(render);                            // We Render In Idle Time
	glutMainLoop();                                  // Go To GLUT Main Loop
	return 0;
}
