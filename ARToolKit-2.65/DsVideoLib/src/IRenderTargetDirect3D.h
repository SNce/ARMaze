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

#ifndef __RENDERTARGET_DIRECT3D__
#define __RENDERTARGET_DIRECT3D__

#ifdef __cplusplus
extern "C" {
#endif

#include <d3dx9.h>

// {63CD6E87-D0B3-4815-9B66-DB5F5048A5B9}
DEFINE_GUID(IID_IRenderTargetDirect3D,
0x63cd6e87, 0xd0b3, 0x4815, 0x9b, 0x66, 0xdb, 0x5f, 0x50, 0x48, 0xa5, 0xb9);

    DECLARE_INTERFACE_(IRenderTargetDirect3D, IUnknown)
    {
		STDMETHOD(setDirect3DTextureTarget) (THIS_
					IDirect3DTexture9 *_d3DTexture
                 ) PURE;


		STDMETHOD(enableDirect3DRenderTarget) (THIS_
					const bool _enableDirect3D
                 ) PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __RENDERTARGET_DIRECT3D__