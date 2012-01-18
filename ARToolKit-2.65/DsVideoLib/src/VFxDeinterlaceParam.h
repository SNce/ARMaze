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

#ifndef __VFX_DEINTERLACE_PARAM__
#define __VFX_DEINTERLACE_PARAM__

// ----------------------------------------------------------------------------
typedef enum _DEINTERLACE_METHOD {

	DEINTERLACE_BLEND   = 0,
	DEINTERLACE_DUP1    = 1,
	DEINTERLACE_DUP2    = 2,
	DEINTERLACE_INVALID = 3 // do not use

} DEINTERLACE_METHOD;

// ----------------------------------------------------------------------------
const DEINTERLACE_METHOD default_DEINTERLACE_METHOD = DEINTERLACE_BLEND;

const unsigned int default_DEINTERLACE_PRIORITY = (default_VFX_PRIORITY + 0x04);

static const char *DEINTERLACE_METHOD_names[]=
{
	"blend",
	"duplicate1",
	"duplicate2",
	"invalid" // do not use
};

static const char *DEINTERLACE_internal_name= "VFxDeinterlace";

// ----------------------------------------------------------------------------
#endif
