#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#ifdef _WIN32

// EXAMPLES:
//
// char *vconf = NULL;
// char *vconf = "showDlg";
// char *vconf = "deviceName=Microsoft DV Camera,videoWidth=720,videoHeight=576";
// char    *vconf = "ieee1394id=437d3b0201460008,videoWidth=180,videoHeight=144";
// char    *vconf = "showDlg";
   char   *vconf = "showDlg,deviceName=Microsoft DV Camera,deinterlaceState=on,deinterlaceMethod=blend,"
                   "pixelFormat=PIXELFORMAT_RGB32,videoWidth=320,videoHeight=240";

/*  parameter format is either NULL or a list of tokens, separated by commas ","
 *
 *   BINARY TOKENS:
 *   --------------
 *   flipH    : flip image horizontally (WARNING: non-optimal performance)
 *   flipV    : flip image vertically (WARNING: non-optimal performance)
 *   showDlg  : displays either WDM capture filter's property page or
 *              MSDV decoder format dialog (depending on source media type).
 *
 *   PARAMETRIZED TOKENS:
 *   --------------------
 *   videoWidth=?  : preferred video width, EXAMPLE: "videoWidth=720"
 *   videoHeight=? : preferred video height, EXAMPLE: "videoHeight=576"
 *   deviceName=?  : preferred WDM device (WARNING: locale-dependent), 
 *                   i.e. try to match substring <?> with "friendly names" of
 *                   enumerated DirectShow WDM wrappers (ksproxy.ax).
 *                   EXAMPLE: "deviceName=Microsoft DV Camera" for IEEE1394 DV devices
 *                            "deviceName=QuickCam" for Logitech QuickCam
 *   ieee1394id=?  : Unique 64-bit device identifier, as defined by IEEE 1394.
 *					 Hexadecimal value expected, i.e. "ieee1394id=437d3b0201460008"
 *                   Use /bin/IEEE394_id.exe to determine your camera's ID.
 *
 *   EXAMPLES:
 *
 *   arVideoOpen(NULL);
 *   arVideoOpen("showDlg");
 *   arVideoOpen("flipH,flipV,showDlg");
 *   arVideoOpen("videoWidth=640,flipH,videoHeight=480,showDlg");
 *   arVideoOpen("deviceName=Microsoft DV Camera,videoWidth=720,videoHeight=480");
 *   arVideoOpen("deviceName=Logitech,videoWidth=320,videoHeight=240,flipV");
 *   arVideoOpen("deviceName=Microsoft DV Camera,ieee1394id=437d3b0201460008");
 *   ...
 */

#else
char            *vconf = "";
#endif
/*****************************************************************************/

int             xsize, ysize;
int             thresh = 100;
int             count = 0;

int             mode = 1;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

char           *patt_name      = "Data/patt.hiro";
int             patt_id;
double          patt_width     = 80.0;
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];

static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw(double marker_trans[3][4]);

main(int argc, char *argv[])
{
    init();

    arVideoCapStart();
    argMainLoop( NULL, keyEvent, mainLoop );
}

static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }

	/* change translation modes */
	if( key == 'c' ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        count = 0;

        mode = 1 - mode;
        if( mode ) printf("Continuous mode: Using arGetTransMatCont.\n");
         else      printf("One shot mode: Using arGetTransMat.\n");
    }

	/* change the threshold value when 't' key pressed */
    if( key == 't' ) {
        printf("Enter new threshold value (default = 100): ");
        scanf("%d",&thresh); while( getchar()!='\n' );
		if(thresh < 0) thresh = 0;
		else if (thresh > 255) thresh = 255;
        printf("\n");
    }
}

/* main loop */
static void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j, k;

    /* grab a vide frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count == 0 ) arUtilTimerReset();
    count++;

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

    /* detect the markers in the video frame */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }

    arVideoCapNext();

    /* check for object visibility */
    k = -1;
    for( j = 0; j < marker_num; j++ ) {
        if( patt_id == marker_info[j].id ) {
            if( k == -1 ) k = j;
            else if( marker_info[k].cf < marker_info[j].cf ) k = j;
        }
    }
    if( k == -1 ) {
        argSwapBuffers();
        return;
    }

    /* get the transformation between the marker and the real camera */
    if(mode == 0) {
        arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
    }
    else {
        arGetTransMatCont(&marker_info[k], patt_trans, patt_center, patt_width, patt_trans);
    }

    draw(patt_trans);

    argSwapBuffers();
}

static void init( void )
{
    ARParam  wparam;

    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }

    /* open the graphics window */
    argInit( &cparam, 2.0, 0, 0, 0, 0 );
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

static void draw(double patt_trans[3][4])
{
    double    gl_para[16];
    GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
    
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* load the camera transformation matrix */
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
    glTranslatef( 0.0, 0.0, 25.0 );
    glutSolidCube(50.0);
    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}
