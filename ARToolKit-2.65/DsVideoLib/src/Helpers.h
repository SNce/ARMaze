 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * File:    Helper function(s)
  * Version: 0.0.3 (02/21/2003)
  * ========================================================================
  * Author:  Thomas Pintaric, Vienna University of Technology
  * Contact: pintaric@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
  * =======================================================================
  *
  * Copyright (C) 2003  Vienna University of Technology
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  *
  * For further information please contact Thomas Pintaric under
  * <pintaric@ims.tuwien.ac.at> or write to Thomas Pintaric,
  * Vienna University of Technology, Favoritenstr. 9-11/188, A1040 Vienna,
  * Austria.
  * ========================================================================*/

#ifndef __HELPERS__
#define __HELPERS__

IPin* getPin(IBaseFilter *flt, PIN_DIRECTION dir, int number);
HRESULT ConnectFilters(IBaseFilter *filter_out, int out_pin_nr,
					   IBaseFilter *in_filter, int in_pin_nr);
HRESULT AutoConnectFilters(IBaseFilter *filter_out, int out_pin_nr, IBaseFilter *in_filter,
											  int in_pin_nr, IGraphBuilder *pGraphBuilder);
char* strtok_prefix(char* str, const char* prefix);

#endif