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

#ifndef __VFX_BASE_PARAM__
#define __VFX_BASE_PARAM__

typedef enum _VFX_STATE {

	VFX_DISABLE  = 0,
	VFX_ENABLE   = 1,
	VFX_AUTO     = 2,
	VFX_INVALID  = 3  // do not use

} VFX_STATE;

const VFX_STATE default_VFX_STATE = VFX_DISABLE;

const unsigned int default_VFX_PRIORITY = 0xFF;

static const char *VFX_STATE_names[]=
{
	"off",
	"on",
	"auto",
	"invalid" // do not use
};


#endif
