 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    Pixel format conversion methods
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

#include <streams.h>
#include <d3dx8.h>
#include <GL\gl.h>
#include "PixelFormat.h"

const char PIXELFORMAT_names[PIXELFORMAT_ENUM_MAX][32] =
{ "PIXELFORMAT_UNKNOWN",
  "PIXELFORMAT_UYVY",
  "PIXELFORMAT_YUY2",
  "PIXELFORMAT_RGB565",
  "PIXELFORMAT_RGB555",
  "PIXELFORMAT_RGB24",
  "PIXELFORMAT_RGB32",
  "PIXELFORMAT_INVALID",
  "PIXELFORMAT_QUERY" };

int PXBitsPerPixel(PIXELFORMAT format)
{
	switch(format)
	{
	case PIXELFORMAT_RGB565:return(16);
	case PIXELFORMAT_RGB555:return(16);
	case PIXELFORMAT_RGB24: return(24);
	case PIXELFORMAT_RGB32: return(32);
	};
	return(0);
}

int PXBitsPerPixel(D3DFORMAT  format)
{
	switch(format)
	{
	case D3DFMT_R5G6B5:		return(16);
	case D3DFMT_X1R5G5B5:	return(16);
	case D3DFMT_R8G8B8:		return(24);
	case D3DFMT_X8R8G8B8:	return(32);
	};
	return(0);
}

bool PXAcceptFormat(D3DFORMAT  format)
{
	switch(format)
	{
	case D3DFMT_R5G6B5:		
	case D3DFMT_X1R5G5B5:	
	case D3DFMT_R8G8B8:		
	case D3DFMT_X8R8G8B8:	return(true);
	};
	return(false);
}

D3DFORMAT PXtoD3D(PIXELFORMAT format)
{
	switch(format)
	{
	case PIXELFORMAT_UYVY:  return(D3DFMT_UYVY);
	case PIXELFORMAT_YUY2 : return(D3DFMT_YUY2);
	case PIXELFORMAT_RGB565:return(D3DFMT_R5G6B5);
	case PIXELFORMAT_RGB555:return(D3DFMT_X1R5G5B5);
	case PIXELFORMAT_RGB24: return(D3DFMT_R8G8B8);
	case PIXELFORMAT_RGB32: return(D3DFMT_X8R8G8B8);
	};
	return(D3DFMT_UNKNOWN);
}

WORD PXtoOpenGL(PIXELFORMAT format, bool bWIN32format)
{
	switch(format)
	{
	case PIXELFORMAT_RGB24: return(bWIN32format ? GL_BGR_EXT  : GL_RGB);
	case PIXELFORMAT_RGB32: return(bWIN32format ? GL_BGRA_EXT : GL_RGBA);
	};
	return(0);
}


GUID PXtoMEDIASUBTYPE(PIXELFORMAT format)
{
	switch(format)
	{
	case PIXELFORMAT_UYVY:  return(MEDIASUBTYPE_UYVY);
	case PIXELFORMAT_YUY2 : return(MEDIASUBTYPE_YUY2);
	case PIXELFORMAT_RGB565:return(MEDIASUBTYPE_RGB565);
	case PIXELFORMAT_RGB555:return(MEDIASUBTYPE_RGB555);
	case PIXELFORMAT_RGB24: return(MEDIASUBTYPE_RGB24);
	case PIXELFORMAT_RGB32: return(MEDIASUBTYPE_RGB32);
	};
	return(CLSID_NULL);
}


PIXELFORMAT MEDIASUBTYPEtoPX(GUID format)
{
	if(format == MEDIASUBTYPE_UYVY)   return(PIXELFORMAT_UYVY);
	if(format == MEDIASUBTYPE_YUY2)   return(PIXELFORMAT_YUY2);
	if(format == MEDIASUBTYPE_RGB565) return(PIXELFORMAT_RGB565);
	if(format == MEDIASUBTYPE_RGB555) return(PIXELFORMAT_RGB555);
	if(format == MEDIASUBTYPE_RGB24)  return(PIXELFORMAT_RGB24);
	if(format == MEDIASUBTYPE_RGB32)  return(PIXELFORMAT_RGB32);
	return(PIXELFORMAT_UNKNOWN);	
}

PIXELFORMAT D3DtoPX(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_UYVY:		return(PIXELFORMAT_UYVY);
	case D3DFMT_YUY2:		return(PIXELFORMAT_YUY2);
	case D3DFMT_R5G6B5:		return(PIXELFORMAT_RGB565);
	case D3DFMT_X1R5G5B5:	return(PIXELFORMAT_RGB555);
	case D3DFMT_R8G8B8:		return(PIXELFORMAT_RGB24);
	case D3DFMT_X8R8G8B8:	return(PIXELFORMAT_RGB32);
	}
	return(PIXELFORMAT_UNKNOWN);
}

PIXELFORMAT OpenGLtoPX(WORD format)
{
	switch(format)
	{
	case GL_BGR_EXT:	return(PIXELFORMAT_RGB24);
	case GL_BGRA_EXT:	return(PIXELFORMAT_RGB32);
	};

	return(PIXELFORMAT_UNKNOWN);
}

const char* PXtoString(PIXELFORMAT format)
{
	return(&(PIXELFORMAT_names[format][0]));
}

PIXELFORMAT StringToPX(char* formatName)
{
	for(unsigned int i=0; i<PIXELFORMAT_ENUM_MAX; i++)
		if(strcmp(formatName,PIXELFORMAT_names[i]) == 0) return((PIXELFORMAT)i);
	return(PIXELFORMAT_UNKNOWN);
}
