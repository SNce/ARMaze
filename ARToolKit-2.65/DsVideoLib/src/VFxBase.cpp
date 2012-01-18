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

#include <streams.h>
#include "VFxBase.h"
// ----------------------------------------------------------------------------


const char* VfxStateToString(VFX_STATE state)
{
	if(state > VFX_INVALID) return(NULL);
	else return(VFX_STATE_names[state]);
}

VFX_STATE StringToVfxState(char *state_str)
{
	for(unsigned int i=0; i<VFX_INVALID; i++)
		if(strcmp(state_str,VFX_STATE_names[i]) == 0) return((VFX_STATE)i);
	return(VFX_INVALID);
}

// ----------------------------------------------------------------------------
CVFxBaseParameter::CVFxBaseParameter() :
	state(default_VFX_STATE),
	priority(default_VFX_PRIORITY)
{}
	
// ----------------------------------------------------------------------------
CVFxMediaSample::CVFxMediaSample(IMediaSample* _pSample, 
								 CMediaType* _pType) :
	pSample(_pSample), pType(_pType)
{}

// ----------------------------------------------------------------------------
CVFxBase::CVFxBase(CVFxBaseParameter* _parameter) :
  parameter(_parameter),
  isInitialized(SUCCEEDED(Initialize()))
{}

// ----------------------------------------------------------------------------
CVFxBase::~CVFxBase()
{
	isInitialized = FAILED(CleanUp());
}
// ----------------------------------------------------------------------------
HRESULT CVFxBase::ApplyEffect(CVFxMediaSample sample)
{
	if(parameter->state == VFX_DISABLE) return(S_OK);
	if(isInitialized) return(EffectProc(sample));
	else return(E_FAIL);
}