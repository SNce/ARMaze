 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    Video Deinterlacing Effect
  * Version: 0.0.3 (02/21/2003)
  * ========================================================================
  * Author:  Thomas Pintaric, Vienna University of Technology
  *          assembly code (c) 1998-2001 by Avery Lee (VirtualDub)
  * Contact: pintaric@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
  * =======================================================================
  *
  * Copyright (C) 2003  Vienna University of Technology
  * Portions Copyright (C) 1998-2001 Avery Lee (http://www.virtualdub.org)
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

#include "streams.h"
#include "VFxDeinterlace.h"
#include "PixelFormat.h"
#include "CPU.h"
#include "MemCopy.h"

#include "DVDMedia.h" // reqired for VIDEOINFOHEADER2

#pragma warning(disable : 4799)
// warning C4799: No EMMS at end of function '...'

typedef unsigned long	Pixel;
typedef unsigned long	Pixel32;
typedef unsigned char	Pixel8;
typedef long			PixCoord;
typedef	long			PixDim;
typedef	long			PixOffset;


HRESULT deinterlaceFrame(BYTE* srcBuffer, BYTE* destBuffer, 
						 unsigned int frameWidth, unsigned int frameHeight,
						 int mode = DEINTERLACE_BLEND, PIXELFORMAT pixelFormat = PIXELFORMAT_RGB32);
// NOTE: only PIXELFORMAT_RGB32 is supported. Function will return E_INVALIDARG on any other format.

const char* DeinterlaceModeToString(DEINTERLACE_METHOD mode)
{
	if(mode > DEINTERLACE_INVALID) return(NULL);
	else return(DEINTERLACE_METHOD_names[mode]);
}

DEINTERLACE_METHOD StringToDeinterlaceMode(char *mode_str)
{
	for(unsigned int i=0; i<DEINTERLACE_INVALID; i++)
		if(strcmp(mode_str,DEINTERLACE_METHOD_names[i]) == 0) return((DEINTERLACE_METHOD)i);
	return(DEINTERLACE_INVALID);
}

//-------------------------------------------------------------------------------------------------

bool IsInterlaced(CMediaType* mediaType)
{
	if(*(mediaType->FormatType()) != FORMAT_VideoInfo2) return(false);
    VIDEOINFOHEADER2 *pvi2 = (VIDEOINFOHEADER2*) mediaType->Format();
    return(pvi2->dwInterlaceFlags & AMINTERLACE_IsInterlaced);
}
// ----------------------------------------------------------------------------
CVFxDeinterlaceParameter::CVFxDeinterlaceParameter() :
	CVFxBaseParameter(),
	method(default_DEINTERLACE_METHOD)
{
	priority = default_DEINTERLACE_PRIORITY;
}

// ----------------------------------------------------------------------------
CVFxDeinterlace::CVFxDeinterlace(CVFxDeinterlaceParameter* _parameter) :
	parameter(_parameter),
	CVFxBase(_parameter)
{}

// ----------------------------------------------------------------------------
CVFxDeinterlace::~CVFxDeinterlace()
{}

// ----------------------------------------------------------------------------
HRESULT CVFxDeinterlace::Initialize() { return(S_OK); }
HRESULT CVFxDeinterlace::CleanUp()    { return(S_OK); }

// ----------------------------------------------------------------------------
HRESULT CVFxDeinterlace::EffectProc(CVFxMediaSample sample)
{
	if(sample.pSample == NULL || sample.pType == NULL) return(E_INVALIDARG);
	if(parameter->state == VFX_AUTO && !IsInterlaced(sample.pType)) return(S_OK);

	BYTE* pixelBuffer = NULL;
	sample.pSample->GetPointer(&pixelBuffer);
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*) sample.pType->pbFormat;
		// VIDEOINFOHEADER2 is backwards compatible with VIDEOINFOHEADER, so it's safe to typecast
    DWORD width    = pvi->bmiHeader.biWidth;
    DWORD height   = pvi->bmiHeader.biHeight;

	HRESULT hr = deinterlaceFrame(pixelBuffer,pixelBuffer,width,height,parameter->method,
                                    MEDIASUBTYPEtoPX(sample.pType->subtype));
	return(hr);
}

// ----------------------------------------------------------------------------
// (c) 1998-2001 by Avery Lee (VirtualDub)
static void __declspec(naked) asm_blend_row_clipped(Pixel *dst, Pixel *src, PixDim w, PixOffset srcpitch) {
	__asm {
		push	ebp
		push	edi
		push	esi
		push	ebx

		mov		edi,[esp+20]
		mov		esi,[esp+24]
		sub		edi,esi
		mov		ebp,[esp+28]
		mov		edx,[esp+32]

xloop:
		mov		ecx,[esi]
		mov		eax,0fefefefeh

		mov		ebx,[esi+edx]
		and		eax,ecx

		shr		eax,1
		and		ebx,0fefefefeh

		shr		ebx,1
		add		esi,4

		add		eax,ebx
		dec		ebp

		mov		[edi+esi-4],eax
		jnz		xloop

		pop		ebx
		pop		esi
		pop		edi
		pop		ebp
		ret
	};
}

// ----------------------------------------------------------------------------
// (c) 1998-2001 by Avery Lee (VirtualDub)
static void __declspec(naked) asm_blend_row(Pixel *dst, Pixel *src, PixDim w, PixOffset srcpitch) {
	__asm {
		push	ebp
		push	edi
		push	esi
		push	ebx

		mov		edi,[esp+20]
		mov		esi,[esp+24]
		sub		edi,esi
		mov		ebp,[esp+28]
		mov		edx,[esp+32]

xloop:
		mov		ecx,[esi]
		mov		eax,0fcfcfcfch

		mov		ebx,[esi+edx]
		and		eax,ecx

		shr		ebx,1
		mov		ecx,[esi+edx*2]

		shr		ecx,2
		and		ebx,07f7f7f7fh

		shr		eax,2
		and		ecx,03f3f3f3fh

		add		eax,ebx
		add		esi,4

		add		eax,ecx
		dec		ebp

		mov		[edi+esi-4],eax
		jnz		xloop

		pop		ebx
		pop		esi
		pop		edi
		pop		ebp
		ret
	};
}

// ----------------------------------------------------------------------------
// (c) 1998-2001 by Avery Lee (VirtualDub)
static void __declspec(naked) asm_blend_row_MMX(Pixel *dst, Pixel *src, PixDim w, PixOffset srcpitch) {
	static const __int64 mask0 = 0xfcfcfcfcfcfcfcfci64;
	static const __int64 mask1 = 0x7f7f7f7f7f7f7f7fi64;
	static const __int64 mask2 = 0x3f3f3f3f3f3f3f3fi64;
	__asm {
		push	ebp
		push	edi
		push	esi
		push	ebx

		mov		edi,[esp+20]
		mov		esi,[esp+24]
		sub		edi,esi
		mov		ebp,[esp+28]
		mov		edx,[esp+32]

		movq	mm5,mask0
		movq	mm6,mask1
		movq	mm7,mask2
		shr		ebp,1
		jz		oddpart

xloop:
		movq	mm2,[esi]
		movq	mm0,mm5

		movq	mm1,[esi+edx]
		pand	mm0,mm2

		psrlq	mm1,1
		movq	mm2,[esi+edx*2]

		psrlq	mm2,2
		pand	mm1,mm6

		psrlq	mm0,2
		pand	mm2,mm7

		paddb	mm0,mm1
		add		esi,8

		paddb	mm0,mm2
		dec		ebp

		movq	[edi+esi-8],mm0
		jne		xloop

oddpart:
		test	byte ptr [esp+28],1
		jz		nooddpart

		mov		ecx,[esi]
		mov		eax,0fcfcfcfch
		mov		ebx,[esi+edx]
		and		eax,ecx
		shr		ebx,1
		mov		ecx,[esi+edx*2]
		shr		ecx,2
		and		ebx,07f7f7f7fh
		shr		eax,2
		and		ecx,03f3f3f3fh
		add		eax,ebx
		add		eax,ecx
		mov		[edi+esi],eax

nooddpart:

		pop		ebx
		pop		esi
		pop		edi
		pop		ebp
		ret
	};
}


// ----------------------------------------------------------------------------
HRESULT deinterlaceFrame(BYTE* srcBuffer, BYTE* destBuffer, 
						 unsigned int frameWidth, unsigned int frameHeight,
						 int mode, PIXELFORMAT pixelFormat)
{	
	if(pixelFormat != PIXELFORMAT_RGB32) return(E_INVALIDARG);
		// we can only deinterlace PIXELFORMAT_RGB32

	Pixel32 *src = (Pixel32*)srcBuffer;
	Pixel32 *dst = (Pixel32*)destBuffer;
	PixDim w = frameWidth;
	PixDim h = frameHeight;
	const unsigned int bytes_per_pixel = PXBitsPerPixel(pixelFormat)/8;
	PixOffset pitch = frameWidth * bytes_per_pixel;
	void (*blend_func)(Pixel32 *, Pixel32 *, PixDim, PixOffset);

	switch(mode)
	{
	case(DEINTERLACE_BLEND):
			if(h < 2) return(E_FAIL);
			blend_func = (CpuFeatureFlags & FEATURE_MMX) ? asm_blend_row_MMX : asm_blend_row;

			asm_blend_row_clipped(dst, src, frameWidth, pitch);
			if (h-=2)
				do {
					dst = (Pixel *)((char *)dst + pitch);	
					blend_func(dst, src, frameWidth, pitch);
					src = (Pixel *)((char *)src + pitch);
				} while(--h);

			asm_blend_row_clipped((Pixel *)((char *)dst + pitch), src, frameWidth, pitch);

			if (CpuFeatureFlags & FEATURE_MMX)
				__asm emms
			break;

	case(DEINTERLACE_DUP1):
			src = (Pixel *)((char *)src + pitch);
			dst = (Pixel *)((char *)src - pitch);
			goto start_dup;

	case(DEINTERLACE_DUP2):
			dst = (Pixel *)((char *)src + pitch);

			start_dup:

			if (h>>=1)
				do 
				{
					if (CpuFeatureFlags & FEATURE_SSE) 
						memcpySSE(dst,src,frameWidth*bytes_per_pixel);
					else if(CpuFeatureFlags & FEATURE_MMX) 
						memcpyMMX(dst,src,frameWidth*bytes_per_pixel);
					else 
						memcpy(dst,src,frameWidth*bytes_per_pixel);

					src = (Pixel *)((char *)src + pitch*2);
					dst = (Pixel *)((char *)dst + pitch*2);
				} while(--h);
			break;

	default:				return(E_INVALIDARG);
		break;
	};

	return(S_OK);
}
// ----------------------------------------------------------------------------
