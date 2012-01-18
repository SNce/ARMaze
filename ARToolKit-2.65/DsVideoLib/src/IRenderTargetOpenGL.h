 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    CDsRenderer COM interface methods
  * Version: 0.0.3 (02/21/2003)
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

#ifndef __RENDERTARGET_OPENGL__
#define __RENDERTARGET_OPENGL__

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/gl.h>		 // OpenGL

const GLint INVALID_TEXTURE = (-1);

// {0B3FD7DF-2C97-4410-8CBF-F7201400A125}
DEFINE_GUID(IID_IRenderTargetOpenGL, 
0xb3fd7df, 0x2c97, 0x4410, 0x8c, 0xbf, 0xf7, 0x20, 0x14, 0x0, 0xa1, 0x25);

    DECLARE_INTERFACE_(IRenderTargetOpenGL, IUnknown)
    {
		STDMETHOD(setOpenGLContext) (THIS_
					const HGLRC _parentRC,
					const HDC _parentDC
                 ) PURE;

		STDMETHOD(setOpenGLTextureTarget) (THIS_
					const int _openGLTexture
                 ) PURE;

		STDMETHOD(enableOpenGLRenderTarget) (THIS_
					const bool _enableOpenGL
                 ) PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __RENDERTARGET_OPENGL__

