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

#ifndef __RENDERER_VFX__
#define __RENDERER_VFX__

#ifdef __cplusplus
extern "C" {
#endif

#include "VFxBaseParam.h"
#include "VFxDeinterlaceParam.h"

#include <d3dx9.h>
#include <process.h>	 // Multi-threading support


// {20894EE8-8964-4745-B998-C21A5737CD07}
DEFINE_GUID(IID_IRendererVFx,
0x20894ee8, 0x8964, 0x4745, 0xb9, 0x98, 0xc2, 0x1a, 0x57, 0x37, 0xcd, 0x7);


    DECLARE_INTERFACE_(IRendererVFx, IUnknown)
    {
		// VFxDeinterlace -----------------------------
        STDMETHOD(set_VFxDeinterlaceState) (THIS_
					VFX_STATE _state
                 ) PURE;

        STDMETHOD(set_VFxDeinterlaceMethod) (THIS_
					DEINTERLACE_METHOD _method
                 ) PURE;

		STDMETHOD(get_VFxDeinterlaceParam) (THIS_
						VFX_STATE* _state,
						DEINTERLACE_METHOD* _method
                 ) PURE;
		// --------------------------------------------

	};


#ifdef __cplusplus
}
#endif

#endif // __RENDERER_VFX__