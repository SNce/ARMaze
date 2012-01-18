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

#ifndef __RENDERER_PARAM__
#define __RENDERER_PARAM__

#ifdef __cplusplus
extern "C" {
#endif

#include <d3dx9.h>
#include <process.h>	 // Multi-threading support

const REFERENCE_TIME INVALID_TIMESTAMP = (0);

// {3C1D86DF-57D0-450f-9D30-345E7862A7CE}
DEFINE_GUID(IID_IRendererParam, 
0x3c1d86df, 0x57d0, 0x450f, 0x9d, 0x30, 0x34, 0x5e, 0x78, 0x62, 0xa7, 0xce);

    DECLARE_INTERFACE_(IRendererParam, IUnknown)
    {
        STDMETHOD(setPixelFormat) (THIS_
					const PIXELFORMAT _pixelFormat
                 ) PURE;

        STDMETHOD(getPixelFormat) (THIS_
					PIXELFORMAT *_PixelFormat
                 ) PURE;

		// Width & Height will be set to zero if the input pin is not connected
        STDMETHOD(getFrameSize) (THIS_
					unsigned int *_Width,
					unsigned int *_Height
                 ) PURE;

        STDMETHOD(getInterlacing) (THIS_
					bool *_isInterlaced
                 ) PURE;

        STDMETHOD(getCurrentTimestamp) (THIS_
					REFERENCE_TIME *_Timetamp
                 ) PURE;


	// make sure to assign the SAME critical section primitive to all instances
	// of this filter
		STDMETHOD(setSyncPrimitives) (THIS_
					CCritSec *_Lock,
					LPTSTR _ESyncName
                 ) PURE;

		STDMETHOD(setAllocatorBuffers) (THIS_
					unsigned int _num_buffers
                 ) PURE;

		STDMETHOD(getAllocatorBuffers) (THIS_
					unsigned int *_num_buffers
                 ) PURE;


    };

#ifdef __cplusplus
}
#endif

#endif // __RENDERER_PARAM__