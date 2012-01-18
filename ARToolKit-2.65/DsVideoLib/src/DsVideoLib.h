 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    MAIN INCLUDE FILE (CGraphManager)
  * Version: 0.0.3 (02/21/2003)
  * ========================================================================
  * Author:  Thomas Pintaric, Vienna University of Technology
  * Contact: pintaric@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
  * =======================================================================
  *
  * Copyright (C) 2003  Vienna University of Technology
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
  * ========================================================================
  *
  *	 [insert general description here]
  *
  *
  *  This library has been tested with the following video cameras:
  * 
  *	 FireFly IEEE-1394 camera from Point Grey Research, http://www.ptgrey.com/
  *	 Logitech QuickCam USB webcam, http://www.logitech.com
  *	 Sony VX2000 PAL miniDV camcorder, http://www.sony.com/
  *  Canon XL1S PAL miniDV camera, http://canon.com
  *
 /* ======================================================================= */

#ifdef DSGRAPHBUILDER_EXPORTS
#define DSGRAPHBUILDER_API __declspec(dllexport)
#else
#define DSGRAPHBUILDER_API __declspec(dllimport)
#endif

#include <D3DX9.h>
#include <streams.h>     // DirectShow (includes windows.h)
#include <comutil.h>
#include <atlcomcli.h>
#include <process.h>	 // Multi-threading support
#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const

DEFINE_GUID(CLSID_SpyTrIpFilter, 
0xb5000006, 0xde20, 0x11cf, 0xa9, 0x1e, 0xc4, 0xf, 0xde, 0x92, 0x5d, 0x5);

DEFINE_GUID(CLSID_DsRenderer, 
0x1c0f6f40, 0xd88, 0x4ba1, 0xb5, 0x36, 0x8, 0x48, 0x30, 0x62, 0x5d, 0x51);
const char* DsRendererFileName = "DsRenderer.ax";
const char* DsVideoLibFileName = "DsVideoLib.dll";

struct IRendererParam;
struct IRenderTargetDirect3D;
struct IRenderTargetOpenGL;
struct IRenderTargetUserCallback;
struct IRenderTargetMemoryBuffer;
struct IRendererVFx;

#include "PixelFormat.h"
const PIXELFORMAT default_PIXELFORMAT = PIXELFORMAT_RGB32;

#include "VFxBaseParam.h"
#include "VFxDeinterlaceParam.h"

#ifdef DSGRAPHBUILDER_EXPORTS
#include "Round.h"
#endif
// -----------------------------------------------------------------------------------------------------------------

#define WM_GRAPHNOTIFY  WM_APP + 1
#define CLASSNAME "EventNotify"

// -----------------------------------------------------------------------------------------------------------------
typedef enum _VIDEO_INPUT_DEVICE
{
	WDM_VIDEO_CAPTURE_FILTER = 0, // select video source from among WDM Streaming Capture Device filters in 
							      // class CLSID_VideoInputDeviceCategory.
	ASYNC_FILE_INPUT_FILTER  = 1, // read video from an AVI file (through an asynchronous file input filter)
	INVALID_INPUT_FILTER  = 2     // do not use

} VIDEO_INPUT_DEVICE;


static const char *VIDEO_INPUT_DEVICE_names[]=
{
	"WDM_CAP",
	"AVI_FILE",
	"invalid" // do not use
};

const VIDEO_INPUT_DEVICE default_VIDEO_INPUT_DEVICE = WDM_VIDEO_CAPTURE_FILTER;
// -----------------------------------------------------------------------------------------------------------------
typedef enum _VIDEO_INPUT_FLAGS // can be combined, i.e. (WDM_SHOW_DIALOG|WDM_MATCH_FILTER_NAME)
{
	WDM_MATCH_FORMAT   		= 0x0001, // first media type matching the a given set of format requirements
	WDM_SHOW_FORMAT_DIALOG	= 0x0002, // displays either the capture pin's property pages (non-DV cameras) or
										  // the DV-decoder filter's format dialog before connecting anything
	WDM_SHOW_CONTROL_DIALOG	= 0x0004, // displays the source filter's property pages before connecting anything
	WDM_MATCH_FILTER_NAME	= 0x0008, // tries find a match based on a filter name substring, i.e. "QuickCam" or "Sony"
	WDM_MATCH_IEEE1394_ID	= 0x0010, // match filter based on a unique 64-bit IEEE 1394 device ID (MSDV driver only)
								      // this technique allows to explicitly choose between different cameras connected
								      // to the same IEEE 1394 bus. Setting this flag usually requires use of the WDM
								      // MSDV driver(qdv.dll)
								      // >> use "/bin/IEEE1394_id.exe" to determine your camera's ID.
	WDM_USE_MAX_FRAMERATE	= 0x0020  // uses the max. available frame rate for the format requested

} VIDEO_INPUT_FLAGS;

// Media type matching is based on the AM_MEDIA_TYPE structures obtained from
// each WDM Video Capture Filter.
//
//   AM_MEDIA_TYPE.majortype (MUST BE MEDIATYPE_Video)
//   AM_MEDIA_TYPE.subtype
//   AM_MEDIA_TYPE.formattype (MUST BE FORMAT_VideoInfo)
//	 VIDEOINFOHEADER.AvgTimePerFrame
//	 VIDEOINFOHEADER.BITMAPINFOHEADER.biWidth 
//	 VIDEOINFOHEADER.BITMAPINFOHEADER.biHeight 

const VIDEO_INPUT_FLAGS default_VIDEO_INPUT_FLAGS = WDM_SHOW_FORMAT_DIALOG;
// -----------------------------------------------------------------------------------------------------------------
typedef enum _ASYNC_INPUT_FLAGS
{
	ASYNC_INPUT_DO_NOT_USE_CLOCK   = 0x0100, // call IMediaFilter::SetSyncSource(NULL) on async file source filter
										     // this will effectively prevent samples from being dropped, but
										     // may result in 100% CPU usage on single-processor systems
	ASYNC_LOOP_VIDEO               = 0x0200, // continuously loops through an input file
	ASYNC_RENDER_SECONDARY_STREAMS = 0x0400  // The Async File Source Filter is always used in combination with an
	                                         // AVI Splitter Filter (CLSID_AviSplitter), where Stream 0 is assumed
										     // to contain video data (MEDIATYPE_Video). This flag instructs CGraphManager
										     // to call IGraphBuilder->Render(IPin*) on Streams 1..n
										     // Set this flag if your AVI file contains an audio stream AND you want it
										     // rendered by DirectSound (usually through CLSID_DSoundRender).
										     // DO NOT SET this flag if your AVI file contains more than one video stream
// not yet implemented:
//	ASYNC_SHOW_DECODER_DIALOG      = 0x0800, // displays either the video decoder's property pages


} ASYNC_INPUT_FLAGS;

const DWORD default_ASYNC_INPUT_FLAGS = 0;
// -----------------------------------------------------------------------------------------------------------------
typedef enum _CP_INTERFACE
{
	CP_CameraControl	= 0, // IAMCameraControl
	CP_VideoProcAmp		= 1  // IAMVideoProcAmp
} CP_INTERFACE;


typedef void (*PTR_UserCallback)(IMediaSample*);
typedef void (*PTR_SimpleUserCallback)(BYTE*,int,int,PIXELFORMAT); 

// -----------------------------------------------------------------------------------------------------------------
typedef struct _DS_MEDIA_FORMAT
{
	VIDEO_INPUT_DEVICE inputDevice;
	LONG biWidth;
	LONG biHeight;
	double frameRate;
	GUID subtype;
	bool isInterlaced;
	LPWSTR sourceFilterName;   // (inputDevice == WDM_VIDEO_CAPTURE_FILTER): WDM capture filter's "friendly name"
	                           // (inputDevice == ASYNC_FILE_INPUT_FILTER):  input file name

	DWORD inputFlags;          // (inputDevice == WDM_VIDEO_CAPTURE_FILTER): combination of ASYNC_INPUT_FLAGS
	                           // (inputDevice == ASYNC_FILE_INPUT_FILTER):  combination of VIDEO_INPUT_FLAGS
							   // NOTE: don't forget to set (defaultInputFlags = false) if you use custom flags!
	bool defaultInputFlags;
	PIXELFORMAT pixel_format;  // default: PIXELFORMAT_RGB32
	char* ieee1394_id;         // unique 64-bit camera identifier (IEEE 1394 devices only)
							   // will be ignored/set to zero for non-IEEE 1394 cameras
	bool flipH;
	bool flipV;
	VFX_STATE		   deinterlace_state;
	DEINTERLACE_METHOD deinterlace_method;

} DS_MEDIA_FORMAT;

DS_MEDIA_FORMAT default_DS_MEDIA_FORMAT();
// -----------------------------------------------------------------------------------------------------------------
const GLint _INVALID_TEXTURE = (-1);
const REFERENCE_TIME _INVALID_TIMESTAMP = (0);

//###############################################################################################
class DSGRAPHBUILDER_API CGraphManager;

//------------------------------------------------------------------------------------------------

#ifdef DSGRAPHBUILDER_EXPORTS
	#include "Helpers.h"
#endif

double DSGRAPHBUILDER_API avg2fps(REFERENCE_TIME AvgTimePerFrame, int precision = 3);
REFERENCE_TIME DSGRAPHBUILDER_API fps2avg(double fps);

//------------------------------------------------------------------------------------------------
class DSGRAPHBUILDER_API CGraphManager {
public:
	CGraphManager();
	~CGraphManager();

/*****************************************************************************/
	HRESULT BuildGraph(DS_MEDIA_FORMAT mf);
/*****************************************************************************/
	HRESULT BuildGraph(char* paramString = NULL);

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
 *   BuildGraph(NULL);
 *   BuildGraph("inputDevive=WDM_CAP,showDlg");
 *   BuildGraph("inputDevive=WDM_CAP,flipH,flipV,showDlg");
 *   BuildGraph("inputDevive=WDM_CAP,pixelFormat=PIXELFORMAT_RGB24,showDlg");
 *   BuildGraph("inputDevive=WDM_CAP,showDlg,deinterlaceState=on,deinterlaceMethod=duplicate1");
 *   BuildGraph("inputDevive=WDM_CAP,videoWidth=640,flipH,videoHeight=480,showDlg,deinterlaceState=auto");
 *   BuildGraph("inputDevive=WDM_CAP,deviceName=Microsoft DV Camera,videoWidth=720,videoHeight=480");
 *   BuildGraph("inputDevive=WDM_CAP,deviceName=Logitech,videoWidth=320,videoHeight=240,flipV");
 *   BuildGraph("inputDevive=WDM_CAP,deviceName=Microsoft DV Camera,ieee1394id=437d3b0201460008");
 *   BuildGraph("inputDevive=AVI_FILE,fileName=C:\Some Directory\Another Directory\Video.AVI");
 *   BuildGraph("inputDevive=AVI_FILE,fileName=Video.AVI,pixelFormat=PIXELFORMAT_RGB24");
 *
 *****************************************************************************/

	HRESULT GetCurrentMediaFormat(DS_MEDIA_FORMAT *mf);

	HRESULT GetCurrentTimestamp(REFERENCE_TIME *Timestamp);
			// I'd recommend you Lock() before querying for timestamps
	REFERENCE_TIME GetCurrentTimestamp();

	DWORD WaitForNextSample(long dwMilliseconds = INFINITE); 
		// behaves just like WaitForSingleObject

	HRESULT EnableD3DTextureTarget(IDirect3DTexture9 *D3DTexture = NULL);
			// NULL parameter WILL NOT CHANGE previous texture assignment to NULL
			// i.e. [pseudo-code]: if(D3DTexture != NULL) then ChangeTextureTo(D3DTexture)
	HRESULT DisableD3DTextureTarget();

	HRESULT SetOpenGLContextInfo(HGLRC RC,HDC DC); 
			// the shared OpenGL rendering context will be established only after mediaControl->Run 
			// is called, see DRGRAPH.CPP for details
			// obviously, you must call this method from the same process where the context was
			// created using wglCreateContext()

	HRESULT EnableOpenGLTextureTarget(GLint OpenGLTextureID = _INVALID_TEXTURE);
			// NOTE: you're responsible for initializing the texture first
			//
			// INVALID_TEXTURE parameter WILL NOT CHANGE previous texture assignment
			// i.e. [pseudo-code]: if(OpenGLTextureID != INVALID_TEXTURE) then 
			//							ChangeTextureTo(OpenGLTextureID)

	HRESULT DisableOpenGLTextureTarget();

	HRESULT EnableCallback(PTR_UserCallback callback);
	HRESULT EnableCallback(PTR_SimpleUserCallback callback);
	_inline HRESULT EnableSimpleCallback(PTR_SimpleUserCallback callback) {return(EnableCallback(callback));}
	HRESULT DisableAllUserCallbacks();

	HRESULT EnableMemoryBuffer();
	HRESULT DisableMemoryBuffer();

	HRESULT CheckoutMemoryBuffer(IMediaSample** MediaSample);
	// every check-out operation will increment an internal instance counter
	// samples get released only if there are no active instances
	HRESULT CheckoutMemoryBuffer(BYTE** Buffer);
	HRESULT CheckoutMemoryBuffer(BYTE** Buffer,
								 unsigned int *Width,
								 unsigned int *Height,
								 PIXELFORMAT* PixelFormat);

	HRESULT CheckinMemoryBuffer(bool ForceRelease = false); 
	// decrements the instance counter

	HANDLE	 ESync;
	const LPTSTR ESyncName;

	// display property pages
	HRESULT ShowFilterProperties(HWND hWnd = NULL);
	HRESULT ShowPinProperties(HWND hWnd = NULL);

// ------------------------------------------------------------------------
public:

	// valid types for (long property): CameraControlProperty, VideoProcAmpProperty
	HRESULT GetCameraParameterRange(CP_INTERFACE interface_type,  
									long property,
									long *pMin,
									long *pMax,
									long *pSteppingDelta,
									long *pDefault,
									long *pCapsFlags);

	HRESULT GetCameraParameter(CP_INTERFACE interface_type, long Property, long *lValue, bool *bAuto);
	HRESULT SetCameraParameter(CP_INTERFACE interface_type, long Property, long lValue, bool bAuto);

protected:
	long GetCameraPropertyAUTOFlag(CP_INTERFACE interface_type, bool bAUTO = true);
	HRESULT GetCameraParameter(CP_INTERFACE interface_type, long Property, long *lValue, long *Flags);
	HRESULT SetCameraParameter(CP_INTERFACE interface_type, long Property, long lValue, long Flags);

public:
	// applies settings to a normalized the parameter range [0..1]
	HRESULT GetCameraParameterN(CP_INTERFACE interface_type, long Property, double *dValue);
	HRESULT SetCameraParameterN(CP_INTERFACE interface_type, long Property, double dValue); // dValue will be clamped to [0..1]

	HRESULT SetCameraParameterToDefault(CP_INTERFACE interface_type, long Property, bool bAuto = TRUE); 
	// bAuto: indicates if the property should be controlled automatically

	HRESULT ResetCameraParameters(bool bAuto = TRUE); // reset all parameters to their defaults

	// Applies to all methods above:
	// -----------------------------
	// "Property" can be one of the following: 
	// (see the Microsoft DirectX 8.1 (C++) SDK documentation for details)
	//
	//		interface_type == CP_CameraControl
	//			CameraControl_Pan
    //			CameraControl_Tilt
    //			CameraControl_Roll
    //			CameraControl_Zoom
    //			CameraControl_Exposure
    //			CameraControl_Iris
	//			CameraControl_Focus
	//		interface_type == CP_VideoProcAmp
	//			VideoProcAmp_Brightness,
	//			VideoProcAmp_Contrast,
	//		    VideoProcAmp_Hue,
	//			VideoProcAmp_Saturation,
	//			VideoProcAmp_Sharpness,
	//			VideoProcAmp_Gamma,
	//			VideoProcAmp_ColorEnable,
	//			VideoProcAmp_WhiteBalance,
	//			VideoProcAmp_BacklightCompensation,
	//			VideoProcAmp_Gain
	//
	//
	// Return values:  S_OK			 call succeeded
	// --------------  S_FALSE		 call succeeded, but value remained unchanged 
	//								 (GetCameraParameterN/SetCameraParameterN only)
	//				   E_INVALIDARG  interface not supported by WDM driver / unknown interface
	//				   E_FAIL		 call failed
// ------------------------------------------------------------------------

	// media flow control
	HRESULT Run();
	HRESULT Pause();
	HRESULT Stop(bool forcedStop = false);

	inline void Lock() { m_Lock->Lock(); }
	inline void Unlock() { m_Lock->Unlock(); }

private:

	CCritSec* m_Lock;
	#ifdef _DEBUG
	DWORD dwRegisterROT;
	#endif

	DS_MEDIA_FORMAT media_format;

protected:

#pragma warning(disable : 4251)
// >> warning C4251: 'CGraphManager::captureGraphBuilder' : class 'ATL::CComPtr<T>'
// >> needs to have dll-interface to be used by clients of class 'CGraphManager'

	CComPtr <ICaptureGraphBuilder2> captureGraphBuilder;
	CComPtr <IGraphBuilder>			graphBuilder;
	CComPtr <IMediaControl>			mediaControl;

	CComPtr<IRendererParam>             rendererParam;
	CComPtr<IRenderTargetDirect3D>      renderTargetDirect3D;
	CComPtr<IRenderTargetOpenGL>        renderTargetOpenGL;
	CComPtr<IRenderTargetUserCallback>  renderTargetUserCallback;
	CComPtr<IRenderTargetMemoryBuffer>  renderTargetMemoryBuffer;
	CComPtr<IRendererVFx>  rendererVFx;

	CComPtr <IMediaEventEx>	mediaEvent;
	CComPtr <IMediaSeeking>	mediaSeeking;

	CComPtr <IAMCameraControl>	cameraControl;
	CComPtr <IAMDroppedFrames>	droppedFrames;
	CComPtr <IAMVideoControl>	videoControl;
	CComPtr <IAMVideoProcAmp>	videoProcAmp;

/*  also supported, but not particularly useful to us:
	IAMAnalogVideoDecoder
	IAMDroppedFrames
	IAMExtDevice
	IAMExtTransport
	IAMFilterMiscFlags
	IAMTimecodeReader
	IAMVideoCompression
	IKsPropertySet
	IReferenceClock
*/

	// references kept for our convenience
	CComPtr<IBaseFilter>	sourceFilter;
	CComPtr<IPin>			capturePin;
	CComPtr<IBaseFilter>	decoderFilter;
	CComPtr<IBaseFilter>	rendererFilter;

};

DWORD WINAPI TShowFilterProperties(LPVOID lpParameter);
DWORD WINAPI TShowPinProperties(LPVOID lpParameter);
