 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * ========================================================================
  * Author:  Thomas Pintaric, Vienna University of Technology
  * Contact: pintaric@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
  * =======================================================================
  *
  * Copyright (C) 2003  Vienna University of Technology
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  *
  * For further information please contact Thomas Pintaric under
  * <pintaric@ims.tuwien.ac.at> or write to Thomas Pintaric,
  * Vienna University of Technology, Favoritenstr. 9-11/188, A1040 Vienna,
  * Austria.
  * ========================================================================*/

#ifndef __DS_VIDEO_RENDERER__
#define __DS_VIDEO_RENDERER__

#include <D3DX9.h>		 // i love blueberry muffins (include muffin.h)
#include <streams.h>     // DirectShow (includes windows.h)
#include <comutil.h>
#include <atlcomcli.h>
#include <process.h>	 // Multi-threading support
#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const

#include <GL/gl.h>		 // OpenGL

#include "PixelFormat.h"
#include "IRendererParam.h"
#include "IRenderTargetOpenGL.h"
#include "IRenderTargetDirect3D.h"
#include "IRenderTargetUserCallback.h"
#include "IRenderTargetMemoryBuffer.h"
#include "IRendererVFx.h"

#include "VFxBase.h"
#include "VFxDeinterlace.h"

#include "resource.h"

#include <list>
using namespace std;
//--------------------------------------------------------------------
// {1C0F6F40-0D88-4ba1-B536-084830625D51}
DEFINE_GUID(CLSID_DsRenderer, 
0x1c0f6f40, 0xd88, 0x4ba1, 0xb5, 0x36, 0x8, 0x48, 0x30, 0x62, 0x5d, 0x51);

const char* DsRendererFileName = "DsRenderer.ax";

#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x) = NULL; }}
#define SAFE_DELETE(x) { if(x) { delete (x); }}

const PIXELFORMAT defaultPixelformat = PIXELFORMAT_RGB32;

bool IsInterlaced(AM_MEDIA_TYPE* mediaType);
//*****************************************************************************

class CDsRendererInputPin : public CRendererInputPin
{
public:
    CDsRendererInputPin(CBaseRenderer *pRenderer,
						 HRESULT *phr,
						 LPCWSTR Name) :
	CRendererInputPin(pRenderer,phr,Name)
	{}


	CMediaType& CurrentMediaType(void)
	{
		return CBasePin::m_mt;
	}

	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
	{
		return(S_OK);
	}
};

//**************************************************************************************************

class CRenderTargetOpenGL
{
public:
	CRenderTargetOpenGL() :
		OpenGLTargetEnabled(false),
		OpenGLTexture(INVALID_TEXTURE),
		CurrentThread(0),
		ParentDC(NULL),
		ParentRC(NULL),
		LocalDC(NULL),
		LocalRC(NULL)
	{}

	~CRenderTargetOpenGL()
	{
		if(LocalRC != NULL)
		{
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(LocalRC);
		}

	}

	HRESULT RenderToOpenGLTexture(BYTE* _PixelBuffer,
								  unsigned int _Width, 
								  unsigned int _Height,
								  PIXELFORMAT _PixelFormat);
	HRESULT EstablishSharedOpenGLContext(CCritSec *Lock, HANDLE syncEvent);


//  IRendererTargetOpenGL methods:
//		STDMETHODIMP setOpenGLContext(const HGLRC _parentRC, const HDC _parentRC);
//		STDMETHODIMP setOpenGLTextureTarget(const int _openGLTexture);
//		STDMETHODIMP enableOpenGLRenderTarget(const bool _enableOpenGL);

	int   OpenGLTexture;
	DWORD CurrentThread;
	HDC   ParentDC;			// main thread's device context
	HGLRC ParentRC;			// main thread's rendering context
	HDC   LocalDC;			// local thread's (QUARTZ.DLL) device context
	HGLRC LocalRC;			// local thread's (QUARTZ.DLL) rendering context
	bool  OpenGLTargetEnabled;

};

//**************************************************************************************************

class CRenderTargetDirect3D
{
public:
	CRenderTargetDirect3D() :
		Direct3DTargetEnabled(false),
		D3DTexture(NULL)
	{}

	~CRenderTargetDirect3D()
	{}
	
	HRESULT RenderToDirect3DTexture(BYTE* _PixelBuffer,
								    unsigned int _Width, 
								    unsigned int _Height,
								    PIXELFORMAT _PixelFormat);

//	IRenderTargetDirect3D Interface Methods
//		STDMETHODIMP setDirect3DTextureTarget(const IDirect3DTexture8 *_d3DTexture);
//		STDMETHODIMP enableDirect3DRenderTarget(const bool _enableDirect3D);

	IDirect3DTexture9 *D3DTexture;
	bool  Direct3DTargetEnabled;

};

//**************************************************************************************************

class CRenderTargetUserCallback
{
public:
	CRenderTargetUserCallback() :
		UserCallbackEnabled(false),
		userCallback(NULL),
		simpleUserCallback(NULL)
	{}

	~CRenderTargetUserCallback()
	{}

	HRESULT RenderToUserCallback(IMediaSample *pMediaSample,
								BYTE* _PixelBuffer,
								 unsigned int _Width, 
								 unsigned int _Height,
								 PIXELFORMAT _PixelFormat);

//  IRenderTargetUserCallback Interface Methods
//      STDMETHODIMP setUserCallback(PTR_UserCallback _userCallback);
//      STDMETHODIMP setUserCallback(PTR_SimpleUserCallback _simpleUserCallback);
//      STDMETHODIMP enableUserCallback(const bool _enableCallback);

	PTR_UserCallback		userCallback;
	PTR_SimpleUserCallback	simpleUserCallback;
	bool                    UserCallbackEnabled;

};

//**************************************************************************************************

class CRenderTargetMemoryBuffer
{
public:
		CRenderTargetMemoryBuffer() :
			QueuedSample(NULL),
			QueuedSampleData(NULL),
			QueuedSampleFormat(defaultPixelformat),
			QueuedSampleWidth(0),
			QueuedSampleHeight(0),
			QueuedSampleInstances(0),
		    MemoryBufferEnabled(false)
	{
		qsLock = new CCritSec();
	}

	~CRenderTargetMemoryBuffer()
	{
		if(QueuedSample != NULL) QueuedSample->Release();
		if(qsLock) delete qsLock;
	}

	HRESULT RenderToMemoryBuffer(IMediaSample* pMediaSample,
								 BYTE* _PixelBuffer,
								 unsigned int _Width, 
								 unsigned int _Height,
								 PIXELFORMAT _PixelFormat);

//  IRenderTargetMemoryBuffer Interface Methods
//		STDMETHODIMP setMemoryBuffer(const BYTE* _memoryBuffer);
//		STDMETHODIMP getMemoryBuffer(const BYTE** _memoryBuffer);
//		STDMETHODIMP enableMemoryBuffer(const bool _enableMemoryBuffer);

	IMediaSample*	QueuedSample;
	CCritSec*		qsLock;

	BYTE*			QueuedSampleData;
	PIXELFORMAT		QueuedSampleFormat;
	unsigned int	QueuedSampleWidth;
	unsigned int	QueuedSampleHeight;

	unsigned int	QueuedSampleInstances;
	bool			MemoryBufferEnabled;

};

//**************************************************************************************************

class CVFxList
{
public:
	CVFxList();
	~CVFxList();

	void AddVFx(CVFxBase* vfx);
	HRESULT ApplyVFx(CVFxMediaSample pVFxSample);
	CVFxBase* FindVFx(const char* internal_name); 
		// returns NULL if empty/nothing found

protected:
	void ClearVFxList();
	std::list<CVFxBase*> vfxList;

};

//**************************************************************************************************

class CDsRenderer : public CBaseVideoRenderer,
                    public CPersistStream,
                    public IRendererParam,
                    public IRenderTargetOpenGL,
                    public IRenderTargetDirect3D,
                    public IRenderTargetUserCallback,
					public IRenderTargetMemoryBuffer,
					private CRenderTargetOpenGL,
                    private CRenderTargetDirect3D,
                    private CRenderTargetUserCallback,
					private CRenderTargetMemoryBuffer,
					public IRendererVFx,
					private CVFxList

{
protected:
      friend class CDsRendererInputPin;
      friend class CRendererInputPin;
public:

    // filter object registration information
    static const AMOVIESETUP_FILTER sudFilter;
        // called to create the COM filter oject
    static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    // CPersistStream Members
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    int SizeMax() {return sizeof(PIXELFORMAT);}

    // CPersistStream override
    STDMETHODIMP GetClassID(CLSID *pClsid);

    DECLARE_IUNKNOWN;
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	// CBaseRenderer Members

	void OnRenderStart(IMediaSample *pMediaSample);
	HRESULT DoRenderSample(IMediaSample *pMediaSample);
	HRESULT CheckMediaType(const CMediaType *mtIn);
                        // enforces media format compatibility with (pixel_format)
	BOOL CheckPixelFormat(const CMediaType *pMediaType) const;
    CBasePin *GetPin(int n);

	// IRendererParam Interface Methods

	STDMETHODIMP setPixelFormat(const PIXELFORMAT _pixelFormat);
    STDMETHODIMP getPixelFormat(PIXELFORMAT *_PixelFormat);
	STDMETHODIMP getFrameSize(unsigned int *_Width, unsigned int *_Height);
	                          // will return 0x0 if not connected
    STDMETHODIMP getInterlacing(bool *_isInterlaced);
    STDMETHODIMP getCurrentTimestamp(REFERENCE_TIME *_Timestamp);
	STDMETHODIMP setSyncPrimitives(CCritSec *_Lock,LPTSTR _ESyncName);

	STDMETHODIMP setAllocatorBuffers(unsigned int _num_alloc);
	STDMETHODIMP getAllocatorBuffers(unsigned int *_num_alloc);


	// IRenderTargetOpenGL Interface Methods

	STDMETHODIMP setOpenGLContext(const HGLRC _parentRC, const HDC _parentDC);
	STDMETHODIMP setOpenGLTextureTarget(const int _openGLTexture);
	STDMETHODIMP enableOpenGLRenderTarget(const bool _enableOpenGL);

	// IRenderTargetDirect3D Interface Methods

	STDMETHODIMP setDirect3DTextureTarget(IDirect3DTexture9 *_d3DTexture);
	STDMETHODIMP enableDirect3DRenderTarget(const bool _enableDirect3D);

	// IRenderTargetUserCallback Interface Methods

	STDMETHODIMP setUserCallback(PTR_UserCallback _userCallback);
	STDMETHODIMP setUserCallback(PTR_SimpleUserCallback _simpleUserCallback);
	STDMETHODIMP enableUserCallback(const bool _enableCallback);

	// IRenderTargetMemoryBuffer Interface Methods

	STDMETHODIMP checkoutMemoryBuffer(BYTE** _memoryBuffer,			// can be NULL (=ignore)
									  unsigned int* _Width,			// -"-
									  unsigned int* _Height,		// -"-
									  PIXELFORMAT* _PixelFormat);	// -"-
	STDMETHODIMP checkinMemoryBuffer(const bool _forceRelease);
							 // WARNING : using _forceRelease is not thread-safe!

	STDMETHODIMP checkoutQueuedMediaSample(IMediaSample** _mediaSample);
	STDMETHODIMP checkinQueuedMediaSample(const bool _forceRelease);
							 // WARNING : using _forceRelease is not thread-safe!

	STDMETHODIMP enableMemoryBuffer(const bool _enableMemoryBuffer);

	// IRendererVideoProc Interface Methods

	STDMETHODIMP set_VFxDeinterlaceState(VFX_STATE _state);
	STDMETHODIMP set_VFxDeinterlaceMethod(DEINTERLACE_METHOD _method);
	STDMETHODIMP get_VFxDeinterlaceParam(VFX_STATE* _state, DEINTERLACE_METHOD* _method);


/*
private:
	HRESULT RenderToOpenGLTexture(IMediaSample *pMediaSample);
	HRESULT EstablishSharedOpenGLContext(HANDLE syncEvent);

	HRESULT RenderToDirect3DTexture(IMediaSample *pMediaSample);
	HRESULT RenderToUserCallback(IMediaSample *pMediaSample);
	HRESULT RenderToMemoryBuffer(IMediaSample *pMediaSample);
*/

public:

//**************************************************************************************************
	CDsRenderer(REFCLSID RenderClass, TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr);
    ~CDsRenderer();
//**************************************************************************************************

private:

	// IRendererParam related members
	PIXELFORMAT PixelFormat;
	REFERENCE_TIME Timestamp;

	CCritSec *m_pLock;
	LPTSTR	m_ESyncName;
	HANDLE	m_hEvent;
	CDsRendererInputPin* m_pRendererInputPin;

};

HRESULT LoadSurfaceFromMemory(LPDIRECT3DTEXTURE9 texture,BYTE* pData,int width, int height, D3DFORMAT format);

//**************************************************************************************************

#endif