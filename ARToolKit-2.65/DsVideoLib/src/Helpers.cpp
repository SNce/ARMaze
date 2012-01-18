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

#include "Streams.h"
#include <comutil.h>
#include <atlcomcli.h>
#include "Helpers.h"
#include "Round.h"

#include <string.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------------------------------------------
double avg2fps(REFERENCE_TIME AvgTimePerFrame, int precision)
{ 
	return(RoundDouble(10000000.0 / (double)AvgTimePerFrame,precision)); 
}
// -----------------------------------------------------------------------------------------------------------------
REFERENCE_TIME fps2avg(double fps)
{
	return((REFERENCE_TIME)(1.0/fps*10000000.0));
}
// -----------------------------------------------------------------------------------------------------------------
IPin* getPin(IBaseFilter *flt, PIN_DIRECTION dir, int number)
{
	int n=0;
	IPin* Pin;
	IEnumPins*	EnumPins;
	ULONG		fetched;
	PIN_INFO	pinfo;

	flt->EnumPins(&EnumPins);
	EnumPins->Reset();
	EnumPins->Next(1, &Pin, &fetched);
	Pin->QueryPinInfo(&pinfo);
	//pinfo.pFilter->Release();

	do
	{
		if(pinfo.dir == dir)
		{
			n++;
			if(n==number) 
			{
				EnumPins->Release();
				return(Pin);
			}
			else
			{
				Pin->Release();
				EnumPins->Next(1, &Pin, &fetched);
				Pin->QueryPinInfo(&pinfo);
			}
		}
		else //if (pinfo.dir != dir)
		{
			Pin->Release();
			EnumPins->Next(1, &Pin, &fetched);
			Pin->QueryPinInfo(&pinfo);
			//pinfo.pFilter->Release();
		}
	} while(Pin != NULL);

	EnumPins->Release();
	return(NULL);
}

// -----------------------------------------------------------------------------------------------------------------
HRESULT ConnectFilters(IBaseFilter *filter_out, int out_pin_nr, 
					   IBaseFilter *in_filter, int in_pin_nr)
{
	HRESULT hr;
	CComPtr<IPin> OutPin = getPin(filter_out,PINDIR_OUTPUT,out_pin_nr);
	CComPtr<IPin> InPin =  getPin(in_filter,PINDIR_INPUT,in_pin_nr);
	if(OutPin == NULL || InPin== NULL) return(E_FAIL);
	
	if(FAILED(hr = OutPin->Connect(InPin,NULL))) return(E_FAIL);
	else return(S_OK);
}
// -----------------------------------------------------------------------------------------------------------------

HRESULT AutoConnectFilters(IBaseFilter *filter_out, int out_pin_nr, IBaseFilter *in_filter,
											  int in_pin_nr, IGraphBuilder *pGraphBuilder)
{
	HRESULT hr;
	CComPtr<IPin> OutPin = getPin(filter_out,PINDIR_OUTPUT,out_pin_nr);
	CComPtr<IPin> InPin =  getPin(in_filter,PINDIR_INPUT,in_pin_nr);
	if(OutPin == NULL || InPin== NULL) return(E_FAIL);
	
	if(FAILED(hr = pGraphBuilder->Connect(OutPin,InPin))) return(E_FAIL);
	else return(S_OK);
}
// -----------------------------------------------------------------------------------------------------------------

char* strtok_prefix(char* str, const char* prefix)
{
	if(strstr(str,prefix) != 0)
		return(str+strlen(prefix));
	else return(NULL);
}
// -----------------------------------------------------------------------------------------------------------------
