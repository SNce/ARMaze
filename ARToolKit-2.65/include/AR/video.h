/*******************************************************
 *
 * Author: Hirokazu Kato, Atsishi Nakazawa
 *         (modified by Thomas Pintaric)
 *
 *         kato@sys.im.hiroshima-cu.ac.jp
 *         nakazawa@inolab.sys.es.osaka-u.ac.jp
 *         pintaric@ims.tuwien.ac.at
 *
 * Revision: 4.3b
 * Date: 2003/02/02
 *
*******************************************************/
#ifndef AR_VIDEO_H
#define AR_VIDEO_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <AR/config.h>
#include <AR/ar.h>

#ifdef _WIN32
#include <AR/sys/videoWin32DirectShow.h>

#ifdef LIB_AR_VIDEO_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#endif

#ifdef __linux
#ifdef AR_INPUT_V4L
#include <AR/sys/videoLinuxV4L.h>
#endif

#ifdef  AR_INPUT_DV
#include <AR/sys/videoLinuxDV.h>
#endif

#ifdef  AR_INPUT_1394CAM
#include <AR/sys/videoLinux1394Cam.h>
#endif
#endif

#ifdef __sgi
#include <AR/sys/videoSGI.h>
#endif

#ifdef __APPLE__
#include <AR/sys/videoMacOSX.h>
#endif


#ifdef _WIN32

/*****************************************************************************/
int        DLL_API arVideoOpen( char *config );

/*  parameter format is either NULL or a list of tokens, separated by commas ","
 *
 *   BINARY TOKENS:
 *   --------------
 *   flipH     : flip image horizontally (WARNING: non-optimal performance)
 *               not (yet) implemented --> HAS NO EFFECT IF USED WITHOUT ARTOOLKIT
 *   flipV     : flip image vertically (WARNING: non-optimal performance)
 *               not (yet) implemented --> HAS NO EFFECT IF USED WITHOUT ARTOOLKIT
 *   showDlg   : displays either WDM capture filter's property page or
 *               MSDV decoder format dialog (depending on source media type).
 *               only applies to WDM_CAP, will be ignored for AVI_FILE
 *   loopAVI   : continuously loops through an AVI file (applies only to AVI_FILE)
 *   noClock   : does not use a Reference Clock to synchronize samples;
 *               use this flag for offline post-processing (applies only to AVI_FILE)
 *   renderSec : render secondary streams (applies only to AVI_FILE)
 *               An AVI file can contain an arbitrary number of multiplexed A/V substreams,
 *               usually there are at most 2 substreams (1st: video, 2nd: audio).
 *               the AVI_FILE input module will only try to render stream 0x00 (assuming that
 *               it's video) and ignore the remaning substreams.
 *               Use this flag to force IGraphBuilder->Render(IPin*) calls on substreams 1..n
 *               DO NOT SET this flag if your AVI file contains more than one video stream
 *
 *   PARAMETRIZED TOKENS:
 *   --------------------
 *   inputDevice=? : supported parameters: 
 *                   "WDM_CAP"  (WDM_VIDEO_CAPTURE_FILTER) use the DirectShow WDM wrapper
 *                              to obtain live video input from a streaming capture device
 *                              such as a IEEE 1394 DV camera or USB webcam.
 *                              OPTIONAL: set deviceName=? and/or ieee1394id=? for better
 *                                        control over the choice of suitable WDM drivers
 *                   "AVI_FILE" (ASYNC_FILE_INPUT_FILTER) use an Asynchronous File Input
 *                              Filter to read & decode AVI video data
 *                              NOTE: be sure to specify the input file name by pointing
 *                                    fileName=? to a valid AVI file.
 *
 *                   EXAMPLE: "inputDevive=WDM_CAP", or "inputDevice=AVI_FILE"
 *                   DEFAULT: "WDM_CAP" will be selected if you omit this token
 *
 *   videoWidth=?  : preferred video width, EXAMPLE: "videoWidth=720"
 *                   only applies to WDM_CAP, will be ignored for AVI_FILE
 *   videoHeight=? : preferred video height, EXAMPLE: "videoHeight=576"
 *                   only applies to WDM_CAP, will be ignored for AVI_FILE
 *
 *   pixelFormat=? : internal pixel format (see PixelFormat.h for supported types)
 *                   PERFORMANCE WARNING: Be sure to match your IDirect3DTexture/OpenGL texture
 *                                        formats to whatever you specify here, i.e. use
 *                                        PXtoOpenGL(format), PXtoD3D(format) for creating your
 *                                        textures! (see PixelFormat.h for details)
 *                   EXAMPLE: "pixelFormat=PIXELFORMAT_RGB32"
 *					 NOTE: if you omit the pixelFormat=? token, the global
 *                         constant (default_PIXELFORMAT, usually PIXELFORMAT_RGB32)
 *                         will be selected.
 *
 *   deviceName=?  : only applies to WDM_CAP, will be IGNORED if "inputDevice=WDM_CAP" is not set.
 *                   preferred WDM device (WARNING: WDM "friendly names" are locale-dependent), 
 *                   i.e. try to match substring <?> with "friendly names" of
 *                   enumerated DirectShow WDM wrappers (ksproxy.ax).
 *                   EXAMPLE: "deviceName=Microsoft DV Camera" for IEEE1394 DV devices
 *                            "deviceName=QuickCam" for Logitech QuickCam
 *
 *   fileName=?    : only applies to AVI_FILE, will be IGNORED if "inputDevice=AVI_FILE" is not set.
 *                   input file name, if just use a file's name (without its full path), the WIN32
 *                   API fuction SearchPath() (Winbase.h) will be used to locate the file.
 *                   EXAMPLE: "fileName=C:\Some Directory\Another Directory\Video.AVI"
 *                            "fileName=video.AVI" (will succeed if C:\Some Directory\Another Directory\
 *                                                  is:  * the application's startup directory
 *                                                       * the current directory
 *                                                       * listed in the PATH environment variable)
 *
 *   ieee1394id=?  : only applies to WDM_CAP, will be IGNORED if "inputDevice=WDM_CAP" is not set.
 *                   Unique 64-bit device identifier, as defined by IEEE 1394.
 *					 Hexadecimal value expected, i.e. "ieee1394id=437d3b0201460008"
 *                   Use /bin/IEEE394_id.exe to determine your camera's ID.
 *
 *   deinterlaceState=?  : supported parameters (see VFX_STATE_names[])
 *                           "off"  : disable deinterlacing (DEFAULT)
 *                           "on"   : force deinterlacing (even for progressive frames) 
 *                           "auto" : enable deinterlacing only if 
 *                                    (VIDEOINFOHEADER.dwInterlaceFlags & AMINTERLACE_IsInterlaced)
 *                         WARNING: EXPERIMENTAL FEATURE!
 *
 *   deinterlaceMethod=? : deinterlacing method (see VFxDeinterlaceParam.h for supported modes)
 *                         supported parameters (see DEINTERLACE_METHOD_names[]):
 *                           "blend"      : blend fields (best quality)
 *                           "duplicate1" : duplicate first field
 *                           "duplicate2" : duplicate second field
 *                         NOTE: omitting this token results in default mode (DEINTERLACE_BLEND) being used.
 *                         WARNING: EXPERIMENTAL FEATURE!
 *
 *   EXAMPLES:
 *
 *   arVideoOpen(NULL);
 *   arVideoOpen("inputDevive=WDM_CAP,showDlg");
 *   arVideoOpen("inputDevive=WDM_CAP,flipH,flipV,showDlg");
 *   arVideoOpen("inputDevive=WDM_CAP,pixelFormat=PIXELFORMAT_RGB24,showDlg");
 *   arVideoOpen("inputDevive=WDM_CAP,showDlg,deinterlaceState=on,deinterlaceMethod=duplicate1");
 *   arVideoOpen("inputDevive=WDM_CAP,videoWidth=640,flipH,videoHeight=480,showDlg,deinterlaceState=auto");
 *   arVideoOpen("inputDevive=WDM_CAP,deviceName=Microsoft DV Camera,videoWidth=720,videoHeight=480");
 *   arVideoOpen("inputDevive=WDM_CAP,deviceName=Logitech,videoWidth=320,videoHeight=240,flipV");
 *   arVideoOpen("inputDevive=WDM_CAP,deviceName=Microsoft DV Camera,ieee1394id=437d3b0201460008");
 *   arVideoOpen("inputDevive=AVI_FILE,fileName=C:\Some Directory\Another Directory\Video.AVI");
 *   arVideoOpen("inputDevive=AVI_FILE,fileName=Video.AVI,pixelFormat=PIXELFORMAT_RGB24");
 *
 *****************************************************************************/

int        DLL_API arVideoClose( void );
int        DLL_API arVideoDispOption( void );

int        DLL_API arVideoInqSize( int *x, int *y );
ARUint8    DLL_API *arVideoGetImage( void );

int        DLL_API arVideoCapStart( void );
int        DLL_API arVideoCapStop( void );
int        DLL_API arVideoCapNext( void );



int             DLL_API ar2VideoDispOption ( void );
AR2VideoParamT  DLL_API *ar2VideoOpen       ( char *config );
int             DLL_API ar2VideoClose      ( AR2VideoParamT *vid );
int             DLL_API ar2VideoCapStart   ( AR2VideoParamT *vid );
int             DLL_API ar2VideoCapNext    ( AR2VideoParamT *vid );
int             DLL_API ar2VideoCapStop    ( AR2VideoParamT *vid );
ARUint8			DLL_API *ar2VideoGetImage   ( AR2VideoParamT *vid );
int             DLL_API ar2VideoInqSize    ( AR2VideoParamT *vid, int *x,int *y );

#else // !_WIN32

int        arVideoOpen( char *config );
int        arVideoClose( void );
int        arVideoDispOption( void );

int        arVideoInqSize( int *x, int *y );
ARUint8*   arVideoGetImage( void );

int        arVideoCapStart( void );
int        arVideoCapStop( void );
int        arVideoCapNext( void );



int             ar2VideoDispOption ( void );
AR2VideoParamT *ar2VideoOpen       ( char *config );
int             ar2VideoClose      ( AR2VideoParamT *vid );
int             ar2VideoCapStart   ( AR2VideoParamT *vid );
int             ar2VideoCapNext    ( AR2VideoParamT *vid );
int             ar2VideoCapStop    ( AR2VideoParamT *vid );
ARUint8        *ar2VideoGetImage   ( AR2VideoParamT *vid );
int             ar2VideoInqSize    ( AR2VideoParamT *vid, int *x,int *y );

#endif // !_WIN32

#ifdef  __cplusplus
}
#endif
#endif
