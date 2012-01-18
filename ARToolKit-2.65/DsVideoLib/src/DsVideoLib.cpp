 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    CGraphManager implementation
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
  * ========================================================================*/

#include "DsVideoLib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <objbase.h>
#include <comutil.h>
#include <atlcomcli.h>
#include <process.h>

#include "CPU.h"

#include "GlDebug.h"

#include "IRendererParam.h"
#include "IRenderTargetDirect3D.h"
#include "IRenderTargetOpenGL.h"
#include "IRenderTargetUserCallback.h"
#include "IRenderTargetMemoryBuffer.h"
#include "IRendererVFx.h"

#include "VFxBase.h"
#include "VFxDeinterlace.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// -----------------------------------------------------------------------------------------------------------------
DS_MEDIA_FORMAT default_DS_MEDIA_FORMAT()
{
	DS_MEDIA_FORMAT mf;
	ZeroMemory(&mf, sizeof(DS_MEDIA_FORMAT));
	mf.inputDevice = default_VIDEO_INPUT_DEVICE;
	mf.pixel_format = default_PIXELFORMAT;
	mf.inputFlags = 0; // clear flags
	mf.defaultInputFlags = true;
	mf.deinterlace_state = default_VFX_STATE;
	mf.deinterlace_method = default_DEINTERLACE_METHOD;

	return(mf);
}

// -----------------------------------------------------------------------------------------------------------------
const char* VideoInputDeviceToString(VIDEO_INPUT_DEVICE device)
{
	if(device > INVALID_INPUT_FILTER) return(NULL);
	else return(VIDEO_INPUT_DEVICE_names[device]);
}

// -----------------------------------------------------------------------------------------------------------------
VIDEO_INPUT_DEVICE StringVideoInputDevice(char *device_str)
{
	for(unsigned int i=0; i<INVALID_INPUT_FILTER; i++)
		if(strcmp(device_str,VIDEO_INPUT_DEVICE_names[i]) == 0) return((VIDEO_INPUT_DEVICE)i);
	return(INVALID_INPUT_FILTER);
}

// -----------------------------------------------------------------------------------------------------------------
HRESULT AMErrorMessage(HRESULT hr, char* error_title = "DirectShow ERROR")
{
	TCHAR buffer[MAX_PATH];
	DWORD cRet = AMGetErrorText(hr,buffer,MAX_PATH);
	if(cRet > 0) MessageBox(NULL,_T(buffer),error_title,MB_OK);
	else return(E_FAIL);
	return(S_OK);
}

// -----------------------------------------------------------------------------------------------------------------

HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
// see Microsoft DirectX SDK 9.0 "Loading a Graph From an External Process" 
{
	CComPtr<IMoniker> pMoniker;
	CComPtr<IRunningObjectTable> pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
    }
    return hr;
}

void RemoveFromRot(DWORD pdwRegister)
{
    CComPtr<IRunningObjectTable> pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
    }
}

// -----------------------------------------------------------------------------------------------------------------

HRESULT DisplayPinProperties(CComPtr<IPin> pSrcPin, HWND hWnd = NULL)
{
	CComPtr<ISpecifyPropertyPages> pPages;

	HRESULT hr = pSrcPin->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pPages);
	if (SUCCEEDED(hr))
	{
		PIN_INFO PinInfo;
		pSrcPin->QueryPinInfo(&PinInfo);

		CAUUID caGUID;
		pPages->GetPages(&caGUID);

		OleCreatePropertyFrame(
			hWnd,
			0,
			0,
			L"Property Sheet",
			1,
			(IUnknown **)&(pSrcPin.p),
			caGUID.cElems,
			caGUID.pElems,
			0,
			0,
			NULL);
		CoTaskMemFree(caGUID.pElems);
		PinInfo.pFilter->Release();
	}
	else return(hr);

	return(S_OK);
}

// -----------------------------------------------------------------------------------------------------------------

HRESULT DisplayFilterProperties(IBaseFilter *pFilter, HWND hWnd = NULL)
{
	CComPtr<ISpecifyPropertyPages> pProp;
	HRESULT hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) 
	{
	    // Get the filter's name and IUnknown pointer.
	    FILTER_INFO FilterInfo;
	    pFilter->QueryFilterInfo(&FilterInfo); 

	    // Show the page. 
	    CAUUID caGUID;
	    pProp->GetPages(&caGUID);
	    OleCreatePropertyFrame(
		    hWnd,                   // Parent window
			0, 0,                   // (Reserved)
			FilterInfo.achName,     // Caption for the dialog box
			1,                      // Number of objects (just the filter)
			(IUnknown **)&pFilter,  // Array of object pointers. 
	        caGUID.cElems,          // Number of property pages
		    caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL);               // Reserved

    // Clean up.
	if(FilterInfo.pGraph != NULL) FilterInfo.pGraph->Release(); 
    CoTaskMemFree(caGUID.pElems);
	}
	return(hr);
}

// -----------------------------------------------------------------------------------------------------------------

HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter, 
						  LPWSTR filterNameSubstring = NULL,
						  char* ieee1394id_str = NULL)   // unique 64-bit identifier 
														 // (use output from IEEE1394_id.exe)
{
    HRESULT hr;
    IBaseFilter * pSrc = NULL;
    CComPtr <IMoniker> pMoniker =NULL;
    ULONG cFetched;
   
    // Create the system device enumerator
    CComPtr <ICreateDevEnum> pDevEnum =NULL;

    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
        IID_ICreateDevEnum, (void ** ) &pDevEnum);
    if (FAILED(hr))
    {
		AMErrorMessage(hr,"Couldn't create system enumerator!");
        return hr;
    }

    // Create an enumerator for the video capture devices
    CComPtr <IEnumMoniker> pClassEnum = NULL;

    hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr))
    {
		AMErrorMessage(hr,"Couldn't create system enumerator!");
        return hr;
    }

    // If there are no enumerators for the requested type, then 
    // CreateClassEnumerator will succeed, but pClassEnum will be NULL.
    if (pClassEnum == NULL)
    {
        MessageBox(NULL,TEXT("No video capture device was detected.\r\n\r\n")
                   TEXT("You need at least one WDM video capture device, such as a USB WebCam.\r\n"),
                   TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);
        return E_FAIL;
    }

	while(S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
	{
		CComPtr<IPropertyBag> pProp;
        pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pProp);
        VARIANT varName;
        VariantInit(&varName); // Try to match the friendly name.
        hr = pProp->Read(L"FriendlyName", &varName, 0); 

		if((filterNameSubstring == NULL) || 
		   (SUCCEEDED(hr) && (wcsstr(varName.bstrVal, filterNameSubstring) != NULL)))
        {
	        // Bind Moniker to a filter object
		    hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
			// check for matching IEEE-1394 identifier
			if(ieee1394id_str != NULL)
			{
				IAMExtDevice *pExtDev = NULL;
				hr = pSrc->QueryInterface(IID_IAMExtDevice, (void**)&pExtDev);
				if(SUCCEEDED(hr))
				{
					LPOLESTR ole_str = NULL;
					bool matching_id = false;
					hr = pExtDev->get_ExternalDeviceID(&ole_str);

					unsigned __int64 msdv_id = *((unsigned __int64*) ole_str);
					if(ole_str != NULL) CoTaskMemFree(ole_str);

					char* temp_str = (char*) CoTaskMemAlloc(sizeof(char) * MAX_PATH);
                    _ui64toa(msdv_id,temp_str,16);
					if(strcmp(ieee1394id_str,temp_str) == 0) matching_id = true;
					CoTaskMemFree(temp_str);

					if(SUCCEEDED(hr) && matching_id)
					{
						*ppSrcFilter = pSrc;
						return(S_OK);
					}
					else // pExtDev->get_ExternalDeviceID() failed || identifier mismatch
					{
						pSrc->Release();
					}
				}
				else
				{
					pSrc->Release();
				}
			}
			else // (ieee1394id == 0)
			{
				*ppSrcFilter = pSrc;
				return(S_OK);
			}
        }
        VariantClear(&varName);
        pMoniker = NULL; // Release for the next loop.
    }

	pClassEnum->Reset();

    // Use the first video capture device on the device list.
    // Note that if the Next() call succeeds but there are no monikers,
    // it will return S_FALSE (which is not a failure).  Therefore, we
    // check that the return code is S_OK instead of using SUCCEEDED() macro.
	if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
    {
        // Bind Moniker to a filter object
        hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
        if (FAILED(hr))
        {
			AMErrorMessage(hr,"Couldn't bind moniker to filter object!");
            return hr;
        }
    }
    else
    {
		MessageBox(NULL,"Unable to access video capture device!","ERROR",MB_OK);
        return E_FAIL;
    }

    // Copy the found filter pointer to the output parameter.
    // Do NOT Release() the reference, since it will still be used
    // by the calling function.
    *ppSrcFilter = pSrc;

    return hr;
}

//###############################################################################################


CGraphManager::CGraphManager() :
	  captureGraphBuilder(NULL),
	  graphBuilder(NULL),
	  mediaControl(NULL),
	  rendererParam(NULL),
	  rendererVFx(NULL),
	  renderTargetDirect3D(NULL),
	  renderTargetOpenGL(NULL),
	  renderTargetUserCallback(NULL),
	  renderTargetMemoryBuffer(NULL),
	  mediaEvent(NULL),
	  mediaSeeking(NULL),
	  cameraControl(NULL),
	  droppedFrames(NULL),
	  videoControl(NULL),
	  videoProcAmp(NULL),
	  sourceFilter(NULL),
	  decoderFilter(NULL),
	  rendererFilter(NULL),
	  capturePin(NULL),
	  ESync(NULL),
	  ESyncName("DsGraph SyncEvent")
{
	m_Lock = new CCritSec();
	ESync = CreateEvent(NULL,0,0,ESyncName);
	media_format = default_DS_MEDIA_FORMAT();
	media_format.subtype = GUID_NULL;

#ifdef _DEBUG
	DbgInitialise((struct HINSTANCE__ *) GetModuleHandle(DsVideoLibFileName));
	DbgSetModuleLevel(LOG_TRACE,5);
	DbgSetModuleLevel(LOG_MEMORY,5);
	DbgSetModuleLevel(LOG_ERROR,5);
	DbgSetModuleLevel(LOG_TIMING,5);
	DbgSetModuleLevel(LOG_LOCKING,5);
#endif

	CPU_SetupFeatureFlag();
}

// -----------------------------------------------------------------------------------------------------------------
CGraphManager::~CGraphManager()
{
  HRESULT hr;

  #ifdef _DEBUG
    RemoveFromRot(dwRegisterROT);
  #endif

  OAFilterState fState;
  hr = mediaControl->GetState(100,&fState);
  if(FAILED(hr) || fState != State_Stopped)
  {
	  if(FAILED(hr = mediaControl->StopWhenReady()))
			hr = mediaControl->Stop();
  }

// -------------------------------------------------------------------
/* CComPtr will release outstanding interfaces when going out of scope

#ifdef _DEBUG
  captureGraphBuilder.Release();
  graphBuilder.Release();
  mediaControl.Release();
  rendererParam.Release();
  renderTargetDirect3D.Release();
  renderTargetOpenGL.Release();
  renderTargetUserCallback.Release();
  renderTargetMemoryBuffer.Release();
  mediaEvent.Release();
  mediaSeeking.Release();
  cameraControl.Release();
  droppedFrames.Release();
  videoControl.Release();
  videoProcAmp.Release();
  sourceFilter.Release();
  capturePin.Release();
  decoderFilter.Release();
  rendererFilter.Release();
#endif
// -------------------------------------------------------------------*/

  #ifdef _DEBUG
	DbgOutString("*** DbgDumpObjectRegister:\n");
	DbgDumpObjectRegister();
  #endif

}

// -----------------------------------------------------------------------------------------------------------------
HRESULT MatchMediaTypes(IPin *pin, DS_MEDIA_FORMAT *mf, AM_MEDIA_TYPE *req_mt) 
// will overwrite req_mt with best match
{
	HRESULT hr;
	CComPtr<IEnumMediaTypes> enum_mt;
	pin->EnumMediaTypes(&enum_mt);
	enum_mt->Reset();
	AM_MEDIA_TYPE *p_mt = NULL;
	while(S_OK == (hr = enum_mt->Next(1,&p_mt,NULL)))
	{
		    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) p_mt->pbFormat;
			if( ((p_mt->subtype == mf->subtype) || (mf->subtype == GUID_NULL)) &&
				((pvi->bmiHeader.biHeight == mf->biHeight) || (mf->biHeight == 0)) &&
				((pvi->bmiHeader.biWidth  == mf->biWidth)  || (mf->biWidth == 0)) &&
				((avg2fps(pvi->AvgTimePerFrame,3) == RoundDouble(mf->frameRate,3)) || (mf->frameRate == 0.0))
			  )
			{
				// found a match!
				CopyMediaType(req_mt,p_mt);
				DeleteMediaType(p_mt);
				return(S_OK);
			}
			else DeleteMediaType(p_mt);
	}

	return(E_FAIL);
}

// -----------------------------------------------------------------------------------------------------------------

bool CanDeliverVideo(IPin *pin) 
// checks if a pin can deliver MEDIAFORMAT_Video
{
	HRESULT hr;
	CComPtr <IEnumMediaTypes> enum_mt;
	pin->EnumMediaTypes(&enum_mt);
	enum_mt->Reset();
	AM_MEDIA_TYPE *p_mt = NULL;
	while(S_OK == (hr = enum_mt->Next(1,&p_mt,NULL)))
	{
		if(p_mt->majortype == MEDIATYPE_Video)
		{
			DeleteMediaType(p_mt);
			return(true);
		}
		else DeleteMediaType(p_mt);
	}

	return(false);
}

// -----------------------------------------------------------------------------------------------------------------
bool CanDeliverDV(IPin *pin) 
// checks if a pin can deliver compressed DV formats
// (refer to Microsoft DirectX Documentation, "DV Video Subtypes" for more information)

/*
	FOURCC	GUID				Data Rate	Description 
	'dvsl'	MEDIASUBTYPE_dvsl	12.5 Mbps	SD-DVCR 525-60 or SD-DVCR 625-50 
	'dvsd'	MEDIASUBTYPE_dvsd	25 Mbps		SDL-DVCR 525-60 or SDL-DVCR 625-50 
	'dvhd'	MEDIASUBTYPE_dvhd	50 Mbps		HD-DVCR 1125-60 or HD-DVCR 1250-50 
	'dv25'  MEDIASUBTYPE_dv25	25 Mbps		DVCPRO 25 (525-60 or 625-50) 
	'dv50'  MEDIASUBTYPE_dv50	50 Mbps		DVCPRO 50 (525-60 or 625-50) 
	'dvh1'  MEDIASUBTYPE_dvh1	100 Mbps	DVCPRO 100 (1080/60i, 1080/50i, or 720/60P) 
*/
{
	HRESULT hr;
	CComPtr <IEnumMediaTypes> enum_mt;
	pin->EnumMediaTypes(&enum_mt);
	enum_mt->Reset();
	AM_MEDIA_TYPE *p_mt = NULL;
	while(S_OK == (hr = enum_mt->Next(1,&p_mt,NULL)))
	{
		if((p_mt->subtype == MEDIASUBTYPE_dvsl) ||
		   (p_mt->subtype == MEDIASUBTYPE_dvsd) ||
		   (p_mt->subtype == MEDIASUBTYPE_dvhd) ||
		   (p_mt->subtype == MEDIASUBTYPE_dv25) ||
		   (p_mt->subtype == MEDIASUBTYPE_dv50) ||
		   (p_mt->subtype == MEDIASUBTYPE_dvh1))
		{
			DeleteMediaType(p_mt);
			return(true);
		}
		else DeleteMediaType(p_mt);
	}

	return(false);
}

// -----------------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::BuildGraph(char* paramString)

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

{
	HRESULT hr;
	DS_MEDIA_FORMAT mf = default_DS_MEDIA_FORMAT();
	mf.defaultInputFlags = false;

	mf.sourceFilterName = (LPWSTR) CoTaskMemAlloc(sizeof(wchar_t) * MAX_PATH);
	swprintf(mf.sourceFilterName,L"");
	mf.ieee1394_id = (char*) CoTaskMemAlloc(sizeof(char) * MAX_PATH);

	char* temp_str;

	if(paramString != NULL)
	{
		char config_cpy[MAX_PATH];
		strcpy(config_cpy,paramString);

		char* token = strtok(config_cpy, ",\0");
	    while( token != NULL )
	    {
			if(strcmp(token,"flipH") == 0) mf.flipH = true;
			
			else if(strcmp(token,"flipV") == 0) mf.flipV = true;
			
			else if(strcmp(token,"showDlg") == 0) 
				mf.inputFlags |= WDM_SHOW_FORMAT_DIALOG;

			else if(strcmp(token,"loopAVI") == 0) 
				mf.inputFlags |= ASYNC_LOOP_VIDEO;

			else if(strcmp(token,"noClock") == 0) 
				mf.inputFlags |= ASYNC_INPUT_DO_NOT_USE_CLOCK;

			else if(strcmp(token,"renderSec") == 0) 
				mf.inputFlags |= ASYNC_RENDER_SECONDARY_STREAMS;

			else if((temp_str = strtok_prefix(token,"inputDevice=")) != NULL)
			{
				if((mf.inputDevice = StringVideoInputDevice(temp_str)) == INVALID_INPUT_FILTER)
					mf.inputDevice = default_VIDEO_INPUT_DEVICE;
			}

			else if((temp_str = strtok_prefix(token,"videoHeight=")) != NULL)
			{
				mf.biHeight = strtol(temp_str,NULL,10);
				if(mf.biHeight <= 0) return(E_INVALIDARG);
				mf.inputFlags |= WDM_MATCH_FORMAT;
			}

			else if((temp_str = strtok_prefix(token,"videoWidth=")) != NULL)
			{
				mf.biWidth = strtol(temp_str,NULL,10);
				if(mf.biWidth <= 0) return(E_INVALIDARG);
				mf.inputFlags |= WDM_MATCH_FORMAT;
			}

			else if((temp_str = strtok_prefix(token,"pixelFormat=")) != NULL)
			{
				PIXELFORMAT px = StringToPX(temp_str);
				if(px == PIXELFORMAT_UNKNOWN) return(E_INVALIDARG);
				mf.pixel_format = px;
			}

			else if(((temp_str = strtok_prefix(token,"deviceName=")) != NULL) ||
			         (temp_str = strtok_prefix(token,"fileName=")) != NULL)
			{
				swprintf(mf.sourceFilterName,_bstr_t(temp_str));
				mf.inputFlags |= WDM_MATCH_FILTER_NAME;
			}

			else if((temp_str = strtok_prefix(token,"ieee1394id=")) != NULL)
			{
				strcpy(mf.ieee1394_id,_bstr_t(temp_str));
				mf.inputFlags |= WDM_MATCH_IEEE1394_ID;
			}

			else if((temp_str = strtok_prefix(token,"deinterlaceState=")) != NULL)
			{
				mf.deinterlace_state = StringToVfxState(temp_str);
			}

			else if((temp_str = strtok_prefix(token,"deinterlaceMethod=")) != NULL)
			{
				mf.deinterlace_method = StringToDeinterlaceMode(temp_str);
			}

			token = strtok(NULL,",\0");
		}

	}

	hr = BuildGraph(mf);
	CoTaskMemFree(mf.ieee1394_id); // clean up

	return(hr);
}

// -----------------------------------------------------------------------------------------------------------------

HRESULT CGraphManager::BuildGraph(DS_MEDIA_FORMAT mf)
{
// fully undocumented (hahaha!)
	
	// check for mf.defaultInputFlags and set default flags if necessary
	if(mf.defaultInputFlags)
	{
		switch(mf.inputDevice)
		{
		case(WDM_VIDEO_CAPTURE_FILTER): mf.inputFlags = default_VIDEO_INPUT_FLAGS;
			                            break;
		case(ASYNC_FILE_INPUT_FILTER):  mf.inputFlags = default_ASYNC_INPUT_FLAGS;
			                            break;
		default:                        return(E_INVALIDARG);
			                            break;
		};
	}


	HRESULT hr;

	CComPtr <IBaseFilter> pVideoSource = NULL;
	CComPtr <IBaseFilter> pStreamSplitter = NULL; // used in conjunction with the Async File Source filter
	CComPtr <IBaseFilter> pVideoDecoder = NULL;	// used for changing DV resolution
	CComPtr <IBaseFilter> pVideoRenderer = NULL;
	CComPtr <IAMStreamConfig> pStreamConfig = NULL;

	FILTER_INFO filter_info;

    hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
        IID_IGraphBuilder, (void **) &graphBuilder);
    if (FAILED(hr)) return hr;

	#ifdef _DEBUG
	hr = AddToRot(graphBuilder, &dwRegisterROT);
	#endif

    // Create the capture graph builder
    hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
        IID_ICaptureGraphBuilder2, (void **) &captureGraphBuilder);
    if (FAILED(hr))
        return hr;

	// ----------------------------------------------------------------------------------------
    // Create the video renderer
	//
	// Since DsRenderer.ax is a COM object, it needs to be "registered" (i.e. there has to be an entry under
	// its GUID in the Windows Registry) for the following call to succeed.
	//
	hr = CoCreateInstance(CLSID_DsRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(pVideoRenderer));
	if(hr == 0x80040154) // class not registered
	{
		char path[MAX_PATH];
		char* path_offset = NULL;
		if(SearchPath(NULL,DsRendererFileName,NULL,MAX_PATH,path,&path_offset) > 0)
		{
			char quoted_path[MAX_PATH];
			sprintf(quoted_path,"%c%s%c",'"',path,'"');

			if(_spawnlp(_P_WAIT,"regsvr32.exe","regsvr32.exe","/s",quoted_path,NULL) != (-1))
					// try again:
					hr = CoCreateInstance(CLSID_DsRenderer, 
										  NULL,
										  CLSCTX_INPROC_SERVER,
										  IID_IBaseFilter,
										  (void**)&(pVideoRenderer));
		}
	}
	rendererFilter = pVideoRenderer;

	if(FAILED(hr))
	{
		MessageBox(NULL,"Please check if DsRenderer.ax is registered properly.\n"
			            "Run 'C:\\WINDOWS\\SYSTEM32\\regsvr32.exe DsRenderer.ax' if required.",
						"COM object not registered.",MB_OK);
		return(hr);
	}
	// ----------------------------------------------------------------------------------------

	if(FAILED(pVideoRenderer->QueryInterface(IID_IRendererParam,(void**)&rendererParam))) return(hr);
	//if(FAILED(pVideoRenderer->QueryInterface(IID_IRendererVFx,(void**)&rendererVFx))) return(hr);
	if(FAILED(pVideoRenderer->QueryInterface(IID_IRendererVFx,(void**)&rendererVFx)))
	{
		MessageBox(NULL,"rendererVFx","ERROR",MB_OK);
	}

	if(FAILED(pVideoRenderer->QueryInterface(IID_IRenderTargetOpenGL,(void**)&renderTargetOpenGL))) return(hr);
	if(FAILED(pVideoRenderer->QueryInterface(IID_IRenderTargetDirect3D,(void**)&renderTargetDirect3D))) return(hr);
	if(FAILED(pVideoRenderer->QueryInterface(IID_IRenderTargetUserCallback,(void**)&renderTargetUserCallback))) return(hr);
	if(FAILED(pVideoRenderer->QueryInterface(IID_IRenderTargetMemoryBuffer,(void**)&renderTargetMemoryBuffer))) return(hr);

	hr = rendererParam->setPixelFormat(mf.pixel_format);
	hr = rendererParam->setSyncPrimitives(m_Lock,ESyncName);

	if(mf.deinterlace_state != VFX_INVALID)
		if(rendererVFx) rendererVFx->set_VFxDeinterlaceState(mf.deinterlace_state);
	if(mf.deinterlace_state != DEINTERLACE_INVALID)
		if(rendererVFx) rendererVFx->set_VFxDeinterlaceMethod(mf.deinterlace_method);

    // Obtain interfaces for media control
    hr = graphBuilder->QueryInterface(IID_IMediaControl,(LPVOID *) &mediaControl);
    if (FAILED(hr))
        return hr;

    hr = graphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID *) &mediaEvent);
    if (FAILED(hr))
        return hr;

    hr = captureGraphBuilder->SetFiltergraph(graphBuilder);
    if (FAILED(hr))
        return hr;



	// ###########################################################################################
	if (mf.inputDevice == WDM_VIDEO_CAPTURE_FILTER) {
	// ###########################################################################################

		hr = FindCaptureDevice(&pVideoSource,
		                   (mf.inputFlags & WDM_MATCH_FILTER_NAME ? mf.sourceFilterName : NULL),
						   (mf.inputFlags & WDM_MATCH_IEEE1394_ID ? mf.ieee1394_id : 0));
	    if (FAILED(hr))
		{
			// Don't display a message because FindCaptureDevice will handle it
	        return hr;
		}

		hr = graphBuilder->AddFilter(pVideoSource, L"Video Source");

		if (FAILED(hr))
	    {
		    AMErrorMessage(hr,"Couldn't add capture filter to graph!");
			return hr;
	    }
	
		sourceFilter = pVideoSource;
		capturePin = getPin(pVideoSource,PINDIR_OUTPUT,1); 
	
		// -------------
		hr = captureGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,NULL,pVideoSource,IID_IAMStreamConfig,(void**)&pStreamConfig);
	    if (FAILED(hr))
		{
			AMErrorMessage(hr,"Couldn't find IAMStreamConfig interface.");
	        return hr;
	    }

		// ---------------------------------------------------------------------------------
		// Unfortunately, WDM DV Video Capture Devices (such as consumer miniDV camcorders)
		// require special handling. Since the WDM source will only offer DVRESOLUTION_FULL,
		// any changes must be made through IIPDVDec on the DV Decoder filter.
		bool pinSupportsDV = CanDeliverDV(capturePin);

		// ---------------------------------------------------------------------------------
		if(pinSupportsDV)
		{
			CComPtr<IIPDVDec> pDVDec;
			// insert a DV decoder (CLSID_DVVideoCodec) into our graph.
			if(FAILED(hr = CoCreateInstance(CLSID_DVVideoCodec, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(pVideoDecoder)))) return(hr);
			decoderFilter = pVideoDecoder;

			hr = graphBuilder->AddFilter(pVideoDecoder, L"Video Decoder");

			if(mf.inputFlags & WDM_MATCH_FORMAT)
			// since we're dealing with DV, there's only a limited range of possible resolutions:
			//
			//	 DVDECODERRESOLUTION_720x480 (PAL: 720x576)  = DVRESOLUTION_FULL 
			//	 DVDECODERRESOLUTION_360x240 (PAL: 360x266)  = DVRESOLUTION_HALF
			//	 DVDECODERRESOLUTION_180x120 (PAL: 180x144)	 = DVRESOLUTION_QUARTER
			//	 DVDECODERRESOLUTION_88x60   (PAL: 88x72)	 = DVRESOLUTION_DC

			{
				if(FAILED(pVideoDecoder->QueryInterface(IID_IIPDVDec,(void**)&pDVDec))) return(hr);

				int dvRes;
				if(FAILED(hr = pDVDec->get_IPDisplay(&dvRes))) return(hr); // get default resolution

				if((mf.biWidth == 720)	    && ((mf.biHeight == 480) || (mf.biHeight == 576))) dvRes = DVRESOLUTION_FULL;
				else if((mf.biWidth == 360) && ((mf.biHeight == 240) || (mf.biHeight == 266))) dvRes = DVRESOLUTION_HALF;
				else if((mf.biWidth == 180) && ((mf.biHeight == 120) || (mf.biHeight == 144))) dvRes = DVRESOLUTION_QUARTER;
				else if((mf.biWidth == 88)  && ((mf.biHeight == 60)  || (mf.biHeight == 72)))  dvRes = DVRESOLUTION_DC;

				if(FAILED(hr = pDVDec->put_IPDisplay(dvRes))) return(hr);
			}

			if((mf.inputFlags & WDM_SHOW_FORMAT_DIALOG)) // displaying the DV decoder's FILTER property page amounts to
														// the same as showing the WDM capture PIN property page.
			{
				if(FAILED(hr = DisplayFilterProperties(pVideoDecoder))) return(hr);
			}

		} // pinSupportsDV


		// ---------------------------------------------------------------------------------
		else // !pinSupportsDV
		{
			if(mf.inputFlags & WDM_MATCH_FORMAT)
			{
				AM_MEDIA_TYPE mt;
				if(FAILED(hr = MatchMediaTypes(capturePin, &mf, &mt)))
				{
					// automated media type selection failed -- display property page!
					mf.inputFlags &= WDM_SHOW_FORMAT_DIALOG;
				}
				else pStreamConfig->SetFormat(&mt);
			}

			if((mf.inputFlags & WDM_SHOW_FORMAT_DIALOG) && !pinSupportsDV)
				if(FAILED(hr = DisplayPinProperties(capturePin))) return(hr);

		} // !pinSupportsDV
		
		// ---------------------------------------------------------------------------------

		if(mf.inputFlags & WDM_SHOW_CONTROL_DIALOG)
			if(FAILED(hr = DisplayFilterProperties(sourceFilter))) return(hr);
		// ---------------------------------------------------------------------------------


		if(FAILED(pVideoSource->QueryInterface(IID_IAMCameraControl,(void**)&cameraControl)))
			cameraControl = NULL; // will be NULL anyway (replace with something intelligent)

		if(FAILED(pVideoSource->QueryInterface(IID_IAMDroppedFrames,(void**)&droppedFrames)))
			droppedFrames = NULL; // will be NULL anyway (replace with something intelligent)

		if(FAILED(pVideoSource->QueryInterface(IID_IAMVideoControl,(void**)&videoControl)))
			videoControl = NULL; // will be NULL anyway (replace with something intelligent)

		if(FAILED(pVideoSource->QueryInterface(IID_IAMVideoProcAmp,(void**)&videoProcAmp)))
			videoProcAmp = NULL; // will be NULL anyway (replace with something intelligent)


	// ###########################################################################################
	} else if (mf.inputDevice == ASYNC_FILE_INPUT_FILTER) {
	// ###########################################################################################

		if(FAILED(hr = CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, 
			IID_IBaseFilter, (void**)&(pVideoSource)))) return(hr);

		CComPtr<IFileSourceFilter> pFileSource = NULL;		
		if(FAILED(pVideoSource->QueryInterface(IID_IFileSourceFilter,(void**)&pFileSource))) return(hr);
		if(FAILED(hr = pFileSource->Load(mf.sourceFilterName,NULL)))
		{
			char path[MAX_PATH];
			char* path_offset = NULL;
			if(SearchPath(NULL,_bstr_t(mf.sourceFilterName),NULL,MAX_PATH,path,&path_offset) > 0)
			{
				if(FAILED(hr = pFileSource->Load(_bstr_t(path),NULL)))
				{
					// file not found
					AMErrorMessage(hr,"Input file not found.");
					return(hr);
				}
			}
		}

		if(FAILED(hr = graphBuilder->AddFilter(pVideoSource, L"File Reader")))
	    {
		    AMErrorMessage(hr,"Couldn't add async file source filter to graph!");
			return hr;
	    }

		if(FAILED(hr = CoCreateInstance(CLSID_AviSplitter, NULL, CLSCTX_INPROC_SERVER, 
			IID_IBaseFilter, (void**)&(pStreamSplitter)))) return(hr);

		if(FAILED(hr = graphBuilder->AddFilter(pStreamSplitter, L"Stream Splitter"))) return(hr);
		if(FAILED(hr = ConnectFilters(pVideoSource,1,pStreamSplitter,1)))
	    {
		    AMErrorMessage(hr,"Couldn't connect Async File Source to AVI Splitter!");
			return hr;
	    }

		CComPtr<IPin> pStreamPin00 = getPin(pStreamSplitter,PINDIR_OUTPUT,1);
		if(pStreamPin00 == NULL || !CanDeliverVideo(pStreamPin00))
		{
		    AMErrorMessage(hr,"AVI file format error. Substream 0x00 does not deliver MEDIATYPE_Video.");
			return(E_FAIL);
		}

	// -------------------------------------------------------------------------------------------
	} else return(E_INVALIDARG);
	// ###########################################################################################


	// -------------
	hr = graphBuilder->AddFilter(pVideoRenderer, L"Video Renderer");
	// -------------
    // Render the capture pin on the video capture filter
    // Use this instead of g_pGraph->RenderFile


	// -------------------------------------------------------------------------------------------
	if (mf.inputDevice == WDM_VIDEO_CAPTURE_FILTER) {

	/*
		CComPtr<IBaseFilter> pSpy;
		hr = CoCreateInstance(CLSID_SpyTrIpFilter, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&(pSpy));
		if(FAILED(hr)) return(hr);
		hr = graphBuilder->AddFilter(pSpy, L"Debug SPY");

		hr = ConnectFilters(pVideoSource,1,pVideoDecoder,1);
		hr = ConnectFilters(pVideoDecoder,1,pSpy,1);
		hr = ConnectFilters(pSpy,1,pVideoRenderer,1);
	*/

		hr = captureGraphBuilder->RenderStream (NULL, &MEDIATYPE_Video,
												pVideoSource, pVideoDecoder, pVideoRenderer);
		if (FAILED(hr))
		{
			AMErrorMessage(hr,"Couldn't render capture stream.");
			return hr;
		}

		// FIX FOR UNRESOLVED BUG: detach the Reference Clock from our renderer
		//                         filter to prevent deadlocks
		graphBuilder->SetDefaultSyncSource();
		pVideoSource->SetSyncSource(NULL);		// fixes the CAMEvent-dealocking issue
		// -----------------

	// -------------------------------------------------------------------------------------------
	} else if (mf.inputDevice == ASYNC_FILE_INPUT_FILTER) {

		hr = AutoConnectFilters(pStreamSplitter,1,pVideoRenderer,1,graphBuilder);
		if (FAILED(hr))
		{
			AMErrorMessage(hr,"Couldn't find a matching decoder filter for stream 0x00.\n"
				              "Check if the required AVI codec is installed.");
			return hr;
		}

		if(mf.inputFlags & ASYNC_INPUT_DO_NOT_USE_CLOCK)
		{
			pVideoSource->SetSyncSource(NULL);
			pStreamSplitter->SetSyncSource(NULL);
			pVideoRenderer->SetSyncSource(NULL);
		}

		if(mf.inputFlags & ASYNC_RENDER_SECONDARY_STREAMS)
		{
			IPin *pPin;
			CComPtr<IEnumPins> EnumPins;
			ULONG		fetched;
			PIN_INFO	pinfo;

			pStreamSplitter->EnumPins(&EnumPins);
			EnumPins->Reset();
			EnumPins->Next(1, &pPin, &fetched);
			pPin->QueryPinInfo(&pinfo);

			if(fetched > 0) do
			{
				if(pinfo.dir == PINDIR_OUTPUT)
				{
					IPin* pConnectedPin = NULL;
					if(pPin->ConnectedTo(&pConnectedPin) == VFW_E_NOT_CONNECTED)
						hr = graphBuilder->Render(pPin);
					if(pConnectedPin != NULL) pConnectedPin->Release();
				}
				pPin->Release();
				EnumPins->Next(1, &pPin, &fetched);
				pPin->QueryPinInfo(&pinfo);

			} while(fetched > 0);

		}

	// -------------------------------------------------------------------------------------------
	} else return(E_INVALIDARG);



	AM_MEDIA_TYPE mediaType;
	CComPtr<IPin> rendererPin = getPin(pVideoRenderer,PINDIR_INPUT,1); 
	rendererPin->ConnectionMediaType(&mediaType);
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) mediaType.pbFormat;

	pVideoSource->QueryFilterInfo(&filter_info);
		    // ISSUE: filter_info.achName will just contain an integer (e.g. '0001', probably in order of creation)
			// instead of the filter's "friendly name"
	media_format.sourceFilterName = (LPWSTR) CoTaskMemAlloc(sizeof(WCHAR)*wcslen(filter_info.achName)+1);
	wcscpy(media_format.sourceFilterName, filter_info.achName);
	if(filter_info.pGraph != NULL) filter_info.pGraph->Release(); 

	media_format.subtype  = mediaType.subtype;
	media_format.biWidth  = pvi->bmiHeader.biWidth;
	media_format.biHeight = pvi->bmiHeader.biHeight;
	media_format.frameRate = avg2fps(pvi->AvgTimePerFrame);
	media_format.pixel_format = MEDIASUBTYPEtoPX(mediaType.subtype);
	FreeMediaType(mediaType);
	// -------------
	return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::GetCurrentMediaFormat(DS_MEDIA_FORMAT *mf)
{ 
	CopyMemory(mf,&media_format,sizeof(DS_MEDIA_FORMAT));
	return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::EnableD3DTextureTarget(IDirect3DTexture9 *D3DTexture)
{
	HRESULT hr;

	if(D3DTexture != NULL)
		if(FAILED( hr = (renderTargetDirect3D->setDirect3DTextureTarget(D3DTexture)))) return(hr);

	if(FAILED(hr = (renderTargetDirect3D->enableDirect3DRenderTarget(true))))return(hr);
	return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::DisableD3DTextureTarget()
{
	return(renderTargetDirect3D->enableDirect3DRenderTarget(false));
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::SetOpenGLContextInfo(HGLRC RC,HDC DC)
{
	return(renderTargetOpenGL->setOpenGLContext(RC,DC));
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::EnableOpenGLTextureTarget(GLint OpenGLTextureID)
{
	HRESULT hr;

	if(OpenGLTextureID != INVALID_TEXTURE)
		if(FAILED(hr = renderTargetOpenGL->setOpenGLTextureTarget(OpenGLTextureID))) return(hr);

	if(FAILED(hr = renderTargetOpenGL->enableOpenGLRenderTarget(true))) return(hr);
	return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::DisableOpenGLTextureTarget()
{
	return(renderTargetOpenGL->enableOpenGLRenderTarget(false));
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::EnableCallback(PTR_UserCallback callback)
{
	HRESULT hr;

	if(callback != NULL)
		if(FAILED( hr = renderTargetUserCallback->setUserCallback(callback))) return(hr);

	if(FAILED(hr = renderTargetUserCallback->enableUserCallback(true))) return(hr);
	return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::EnableCallback(PTR_SimpleUserCallback callback)
{
	HRESULT hr;

	if(callback != NULL)
		if(FAILED( hr = renderTargetUserCallback->setUserCallback(callback))) return(hr);

	if(FAILED(hr = renderTargetUserCallback->enableUserCallback(true))) return(hr);
	return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::DisableAllUserCallbacks()
{
	return(renderTargetUserCallback->enableUserCallback(false));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::CheckoutMemoryBuffer(IMediaSample** MediaSample)
{
	return(renderTargetMemoryBuffer->checkoutQueuedMediaSample(MediaSample));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::CheckoutMemoryBuffer(BYTE** Buffer)
{
	return(renderTargetMemoryBuffer->checkoutMemoryBuffer(Buffer,NULL,NULL,NULL));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::CheckoutMemoryBuffer(BYTE** Buffer,
								 unsigned int *Width,
								 unsigned int *Height,
								 PIXELFORMAT* PixelFormat)
{
	return(renderTargetMemoryBuffer->checkoutMemoryBuffer(Buffer,Width,Height,PixelFormat));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::CheckinMemoryBuffer(bool ForceRelease)
{
	return(renderTargetMemoryBuffer->checkinQueuedMediaSample(ForceRelease));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::EnableMemoryBuffer()
{
	return(renderTargetMemoryBuffer->enableMemoryBuffer(true));
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::DisableMemoryBuffer()
{
	return(renderTargetMemoryBuffer->enableMemoryBuffer(false));
}
// ----------------------------------------------------------------------------------------------------------

DWORD WINAPI TShowFilterProperties(LPVOID lpParameter)
{
	return(DisplayFilterProperties((IBaseFilter*)lpParameter));
}
// ----------------------------------------------------------------------------------------------------------
DWORD WINAPI TShowPinProperties(LPVOID lpParameter)
{
	return(DisplayPinProperties((IPin*)lpParameter));
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::ShowFilterProperties(HWND hWnd)
{
	DWORD dwThreadID;
	HANDLE hFPropThread = CreateThread(NULL,0,TShowFilterProperties,(LPVOID)sourceFilter,0,&dwThreadID);
	if(hFPropThread == NULL) return(E_FAIL);
	else return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::ShowPinProperties(HWND hWnd)
{
	DWORD dwThreadID;
	HANDLE hPPropThread = CreateThread(NULL,0,TShowPinProperties,(LPVOID)capturePin,0,&dwThreadID);
	if(hPPropThread == NULL) return(E_FAIL);
	else return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::GetCameraParameterRange(CP_INTERFACE interface_type,  
									long property,
									long *pMin,
									long *pMax,
									long *pSteppingDelta,
									long *pDefault,
									long *pCapsFlags)
{
	switch(interface_type)
	{
	case(CP_CameraControl): if(!cameraControl) return(E_INVALIDARG);
							return(cameraControl->GetRange(property,pMin,pMax,pSteppingDelta,pDefault,pCapsFlags));
		break;
	case(CP_VideoProcAmp):  if(!videoProcAmp) return(E_INVALIDARG);
							return(videoProcAmp->GetRange(property,pMin,pMax,pSteppingDelta,pDefault,pCapsFlags));
		break;
	}

	// unknown interface_type
	return(E_INVALIDARG);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::GetCameraParameter(CP_INTERFACE interface_type, long Property, long *lValue, bool *bAuto)
{
	HRESULT hr;
	long Flags = 0;
	hr = GetCameraParameter(interface_type,Property,lValue,&Flags);
	(*bAuto) = (Flags == GetCameraPropertyAUTOFlag(interface_type, true));
	return(hr);
}

HRESULT CGraphManager::GetCameraParameter(CP_INTERFACE interface_type, long Property, long *lValue, long *Flags)
{
	switch(interface_type)
	{
	case(CP_CameraControl): if(!cameraControl) return(E_INVALIDARG);
							return(cameraControl->Get(Property,lValue,Flags));
		break;
	case(CP_VideoProcAmp):  if(!videoProcAmp) return(E_INVALIDARG);
							return(videoProcAmp->Get(Property,lValue,Flags));
		break;
	}

	// unknown interface_type
	return(E_INVALIDARG);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::SetCameraParameter(CP_INTERFACE interface_type, long Property, long lValue, bool bAuto)
{
	return(SetCameraParameter(interface_type,Property,lValue,GetCameraPropertyAUTOFlag(interface_type, bAuto)));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::SetCameraParameter(CP_INTERFACE interface_type, long Property, long lValue, long Flags)
{
	switch(interface_type)
	{
	case(CP_CameraControl): if(!cameraControl) return(E_INVALIDARG);
							return(cameraControl->Set(Property,lValue,Flags));
		break;
	case(CP_VideoProcAmp):  if(!videoProcAmp) return(E_INVALIDARG);
							return(videoProcAmp->Set(Property,lValue,Flags));
		break;
	}

	// unknown interface_type
	return(E_INVALIDARG);
}
// ----------------------------------------------------------------------------------------------------------

long CGraphManager::GetCameraPropertyAUTOFlag(CP_INTERFACE interface_type, bool bAUTO)
{
	switch(interface_type)
	{
	case(CP_CameraControl): return(bAUTO ? CameraControl_Flags_Auto : CameraControl_Flags_Manual);
		break;
	case(CP_VideoProcAmp):  return(bAUTO ? VideoProcAmp_Flags_Auto : VideoProcAmp_Flags_Manual);
		break;
	}
#ifdef _DEBUG
	ASSERT(false);
#endif
	return(0x01);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::SetCameraParameterToDefault(CP_INTERFACE interface_type, long Property, bool bAuto)
{
	long min;
	long max;
	long steppingDelta;
	long defaultValue;
	long capsFlags;

	HRESULT hr;
	if(FAILED(hr = GetCameraParameterRange(interface_type,Property,&min,&max,&steppingDelta,&defaultValue,&capsFlags)))
		return(hr);
	if(FAILED(hr = SetCameraParameter(interface_type,Property,defaultValue,GetCameraPropertyAUTOFlag(interface_type,bAuto))))
		return(hr);

	return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::ResetCameraParameters(bool bAuto)
// bAuto: indicates if the property should be controlled automatically
{
	int s = 0;
	if(cameraControl)
	{
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Pan, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Roll, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Tilt, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Zoom, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Exposure, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Iris, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_CameraControl, CameraControl_Focus, bAuto));
		// impressive, but show me a camera that supports all of these options! :(
	}
	if(videoProcAmp)
	{
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Brightness, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Contrast, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Hue, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Saturation, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Sharpness, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Gamma, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_ColorEnable, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_WhiteBalance, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_BacklightCompensation, bAuto));
		s += (int)SUCCEEDED(SetCameraParameterToDefault(CP_VideoProcAmp, VideoProcAmp_Gain, bAuto));
	}
	
	if(s==0) return(E_FAIL);
	else return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------

HRESULT CGraphManager::GetCameraParameterN(CP_INTERFACE interface_type, long Property, double *dValue)
{
	HRESULT hr;
	long oldValue;
	long oldFlags;
	long min;
	long max;
	long steppingDelta;
	long defaultValue;
	long capsFlags;

	if(FAILED(hr = GetCameraParameter(interface_type,Property,&oldValue, &oldFlags))) return(hr);
	if(FAILED(hr = GetCameraParameterRange(interface_type,Property,&min,&max,&steppingDelta,&defaultValue,&capsFlags)))
		return(hr);

	(*dValue) = ((double)(oldValue-min))/((double)(max-min));
	return(S_OK);
}
// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::SetCameraParameterN(CP_INTERFACE interface_type, long Property, double dValue)
{
	HRESULT hr;
	long oldValue;
	long oldFlags;
	long min;
	long max;
	long steppingDelta;
	long defaultValue;
	long capsFlags;

	if(FAILED(hr = GetCameraParameter(interface_type,Property,&oldValue, &oldFlags))) return(hr);
	if(FAILED(hr = GetCameraParameterRange(interface_type,Property,&min,&max,&steppingDelta,&defaultValue,&capsFlags)))
		return(hr);

	long newValue = min + (long)((dValue*((double)(max-min))) / (double)steppingDelta) * steppingDelta;
	if(newValue == oldValue) return(S_FALSE);
	return(SetCameraParameter(interface_type,Property,newValue,GetCameraPropertyAUTOFlag(interface_type,false)));
}

// ----------------------------------------------------------------------------------------------------------
HRESULT CGraphManager::Run()
{
	HRESULT hr;
	if(FAILED(hr =	mediaControl->Run())) return(hr);
	else return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------

HRESULT CGraphManager::Stop(bool forcedStop)
{
	HRESULT hr;

	if(FAILED(hr =	(forcedStop ? mediaControl->Stop() : mediaControl->StopWhenReady()))) return(hr);
	else return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------

HRESULT CGraphManager::Pause()
{
	HRESULT hr;
	if(FAILED(hr =	mediaControl->Pause())) return(hr);
	else return(S_OK);
}

// ----------------------------------------------------------------------------------------------------------

HRESULT CGraphManager::GetCurrentTimestamp(REFERENCE_TIME *Timestamp)
{
	return(rendererParam->getCurrentTimestamp(Timestamp));
}

REFERENCE_TIME CGraphManager::GetCurrentTimestamp()
{
	REFERENCE_TIME Timestamp;
	if(FAILED(rendererParam->getCurrentTimestamp(&Timestamp)))
		return(_INVALID_TIMESTAMP);
	else return(Timestamp);
}

// ----------------------------------------------------------------------------------------------------------

DWORD CGraphManager::WaitForNextSample(long dwMilliseconds)
{
	return(WaitForSingleObject(ESync,dwMilliseconds));
}