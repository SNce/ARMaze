 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    VFX (Video Effects) Base Classes
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

#ifndef __VFX_BASE__
#define __VFX_BASE__

#include "VFxBaseParam.h"
class CVFxBase;

const char* VfxStateToString(VFX_STATE state);
VFX_STATE StringToVfxState(char *state_str);

// ----------------------------------------------------------------------------
class CVFxBaseParameter
{
public:
	CVFxBaseParameter();

	VFX_STATE state;
	unsigned int priority; // higher-priority effects get applied first
};

// ----------------------------------------------------------------------------
class CVFxMediaSample
{
public:
	CVFxMediaSample(IMediaSample* _pSample,	CMediaType* _pType = NULL);

	IMediaSample*  pSample;
	CMediaType*    pType;
};

// ----------------------------------------------------------------------------
class CVFxBase
{
public:
	CVFxBase(CVFxBaseParameter* _parameter = NULL);
	~CVFxBase();

	bool operator == (const CVFxBase op)
	{
		if(parameter == NULL || op.parameter == NULL) return(false);
		else return(parameter->priority == op.parameter->priority);
	}

	bool operator >  (const CVFxBase op)
	{
		if(parameter == NULL || op.parameter == NULL) return(false);
		else return(parameter->priority > op.parameter->priority);
	}

	bool operator <  (const CVFxBase op)
	{
		if(parameter == NULL || op.parameter == NULL) return(false);
		else return(parameter->priority < op.parameter->priority);
	}

	int GetPriority()
	{
		if(parameter == NULL) return(0);
		else return(parameter->priority);
	}

	virtual const char* GetInternalName() = 0;

	virtual HRESULT Initialize() { return(S_OK); }
	virtual HRESULT CleanUp()	 { return(S_OK); }
	CVFxBaseParameter* GetParameters() { return(parameter); }
	HRESULT ApplyEffect(CVFxMediaSample sample);

protected:
	virtual HRESULT EffectProc(CVFxMediaSample sample) = 0;

	CVFxBaseParameter* parameter;
	bool isInitialized;
};

// ----------------------------------------------------------------------------

#endif
