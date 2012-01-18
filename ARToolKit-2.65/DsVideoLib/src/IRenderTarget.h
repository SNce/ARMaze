 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * ========================================================================
  * Author:  Thomas Pintaric, Vienna University of Technology
  * Contact: mailto:thomas@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
 /* ======================================================================= */

#ifndef __RENDERER_PARAM__
#define __RENDERER_PARAM__

#ifdef __cplusplus
extern "C" {
#endif

#include <d3dx8.h>
#include <process.h>	 // Multi-threading support


// {3C1D86DF-57D0-450f-9D30-345E7862A7CE}
DEFINE_GUID(IID_IRendererParam, 
0x3c1d86df, 0x57d0, 0x450f, 0x9d, 0x30, 0x34, 0x5e, 0x78, 0x62, 0xa7, 0xce);

    DECLARE_INTERFACE_(IRendererParam, IUnknown)
    {
        STDMETHOD(set_PIXELFORMAT) (THIS_
					const PIXELFORMAT pixelFormat
                 ) PURE;

        STDMETHOD(get_PIXELFORMAT) (THIS_
					PIXELFORMAT *pixelFormat
                 ) PURE;

		// Width & Height will be set to zero if the input pin is not connected
        STDMETHOD(getFrameSize) (THIS_
					unsigned int *Width,
					unsigned int *Height
                 ) PURE;

	// make sure to assign the SAME critical section primitive to all instances
	// of this filter
		STDMETHOD(setSyncPrimitives) (THIS_
					CCritSec *_Lock,
					LPTSTR _ESyncName,
					LPCRITICAL_SECTION _CSGLRCSetup
                 ) PURE;

    };

#ifdef __cplusplus
}
#endif

#endif // __RENDERER_PARAM__