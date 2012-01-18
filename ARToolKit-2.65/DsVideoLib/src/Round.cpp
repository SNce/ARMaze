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


// Rounds a number to a specified number of digits.
// Number is the number you want to round.
// Num_digits specifies the number of digits to which you want to round number.
// If num_digits is greater than 0, then number is rounded to the specified number of decimal places.
// If num_digits is 0, then number is rounded to the nearest integer.
// Examples
//		ROUND(2.15, 1)		equals 2.2
//		ROUND(2.149, 1)		equals 2.1
//		ROUND(-1.475, 2)	equals -1.48

#include "math.h"

float Round(const float &number, const int num_digits)
{
	float doComplete5i, doComplete5(number * powf(10.0f, (float) (num_digits + 1)));
	
	if(number < 0.0f)
		doComplete5 -= 5.0f;
	else
		doComplete5 += 5.0f;
	
	doComplete5 /= 10.0f;
	modff(doComplete5, &doComplete5i);
	
	return doComplete5i / powf(10.0f, (float) num_digits);
}

double RoundDouble(double doValue, int nPrecision)
{
	static const double doBase = 10.0f;
	double doComplete5, doComplete5i;
	
	doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));
	
	if(doValue < 0.0f)
		doComplete5 -= 5.0f;
	else
		doComplete5 += 5.0f;
	
	doComplete5 /= doBase;
	modf(doComplete5, &doComplete5i);
	
	return doComplete5i / pow(doBase, (double) nPrecision);
}
