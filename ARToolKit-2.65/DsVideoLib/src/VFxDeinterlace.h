 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    Video Deinterlacing Effect
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

#ifndef __VFX_DEINTERLACE__
#define __VFX_DEINTERLACE__

#include "VFxBase.h"
#include "VFxDeinterlaceParam.h"

bool IsInterlaced(CMediaType* mediaType);

const char* DeinterlaceModeToString(DEINTERLACE_METHOD mode);
DEINTERLACE_METHOD StringToDeinterlaceMode(char *mode_str);

// ----------------------------------------------------------------------------
class CVFxDeinterlaceParameter : public CVFxBaseParameter
{
public:
	CVFxDeinterlaceParameter();
	DEINTERLACE_METHOD method;
};

// ----------------------------------------------------------------------------
class CVFxDeinterlace : public CVFxBase
{
public:
	CVFxDeinterlace(CVFxDeinterlaceParameter* _parameter = NULL);
	~CVFxDeinterlace();

	const char* GetInternalName() { return(DEINTERLACE_internal_name); }
	HRESULT Initialize();
	HRESULT CleanUp();

	HRESULT EffectProc(CVFxMediaSample sample);
	CVFxDeinterlaceParameter* parameter;
};

// ----------------------------------------------------------------------------


#endif
