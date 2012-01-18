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

#ifndef __RENDERTARGET_MEMORYBUFFER__
#define __RENDERTARGET_MEMORYBUFFER__

#ifdef __cplusplus
extern "C" {
#endif


// {2E2D220F-9F2B-4a28-A21B-85B6F73672EE}
DEFINE_GUID(IID_IRenderTargetMemoryBuffer, 
0x2e2d220f, 0x9f2b, 0x4a28, 0xa2, 0x1b, 0x85, 0xb6, 0xf7, 0x36, 0x72, 0xee);

    DECLARE_INTERFACE_(IRenderTargetMemoryBuffer, IUnknown)
    {
		STDMETHOD(checkoutMemoryBuffer) (THIS_
					BYTE** _memoryBuffer,			// can be NULL (=ignore)
					unsigned int* _Width,			// -"-
					unsigned int* _Height,			// -"-
					PIXELFORMAT* _PixelFormat		// -"-
                 ) PURE;

		STDMETHOD(checkoutQueuedMediaSample) (THIS_
					IMediaSample** _mediaSample
                 ) PURE;

		STDMETHOD(checkinQueuedMediaSample) (THIS_    	// use this method as checkoutMemoryBuffer()
					const bool _forceRelease
                 ) PURE;
		// WARNING : using _forceRelease is not thread-safe!

		STDMETHOD(enableMemoryBuffer) (THIS_
					const bool _enableMemoryBuffer
                 ) PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __RENDERTARGET_MEMORYBUFFER__