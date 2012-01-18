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

#ifndef __RENDERTARGET_USERCALLBACK__
#define __RENDERTARGET_USERCALLBACK__

#ifdef __cplusplus
extern "C" {
#endif

#include "PixelFormat.h"

typedef void (*PTR_UserCallback)(IMediaSample*);
typedef void (*PTR_SimpleUserCallback)(BYTE*,int,int,PIXELFORMAT); 

// {DB18FE40-06F4-4774-AEBE-B7F6209B3E92}
DEFINE_GUID(IID_IRenderTargetUserCallback,
0xdb18fe40, 0x6f4, 0x4774, 0xae, 0xbe, 0xb7, 0xf6, 0x20, 0x9b, 0x3e, 0x92);

    DECLARE_INTERFACE_(IRenderTargetUserCallback, IUnknown)
    {
		STDMETHOD(setUserCallback) (THIS_
					PTR_UserCallback _userCallback
                 ) PURE;

		STDMETHOD(setUserCallback) (THIS_
					PTR_SimpleUserCallback _simpleUserCallback
                 ) PURE;

		STDMETHOD(enableUserCallback) (THIS_
					const bool _enableCallback
                 ) PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __RENDERTARGET_USERCALLBACK__