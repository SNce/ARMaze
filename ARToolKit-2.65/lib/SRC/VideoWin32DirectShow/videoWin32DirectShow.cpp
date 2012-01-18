 /* ========================================================================
  * PROJECT: DirectShow Video Processing Library
  * FILE:    ARToolkit 2.6x WIN32/DirectShow video input
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

#include <string.h>
#include <AR/video.h>
#include <stdlib.h>
#include "DsVideoLib.h"
#include "comutil.h"

// -----------------------------------------------------------------------------------------------------------------
static CGraphManager *graphManager = NULL;

static int     open_flag = 0;
static void    error_exit(void);

// -----------------------------------------------------------------------------------------------------------------
#ifdef FLIPPED // compatibility with videoLinx*
static const bool FLIPPED_defined =  true;
#else
static const bool FLIPPED_defined =  false;
#endif

// -----------------------------------------------------------------------------------------------------------------
void FlipImageRGB32(BYTE* pBuf, int width, int height, 
					bool flipImageH, bool flipImageV);


// -----------------------------------------------------------------------------------------------------------------
const int ret_error = (-1);
const int ret_success = (0);
const int ret_not_implemented = (0);
const long frame_timeout_ms = 10;   // set to INFINITE if arVideoGetImage()
                                    // is called from a separate worker thread

static bool flip_horizontal = false;
static bool flip_vertical = false;


// -----------------------------------------------------------------------------------------------------------------
char* strtok_prefix(char* str, const char* prefix)
{
	if(strstr(str,prefix) != 0)
		return(str+strlen(prefix));
	else return(NULL);
}

/*****************************************************************************/
int        DLL_API arVideoOpen( char *config )
{
	CoInitialize(NULL);
	graphManager = new CGraphManager();
	if(FAILED(graphManager->BuildGraph(config))) return(false);

	DS_MEDIA_FORMAT mf;
	graphManager->GetCurrentMediaFormat(&mf);
	
	flip_horizontal = mf.flipH;
	flip_vertical =   mf.flipV;

	if(FAILED(graphManager->EnableMemoryBuffer())) return(ret_error);
	
	open_flag = 1;
	return(ret_success);
}

// -----------------------------------------------------------------------------------------------------------------

int arVideoClose(void)
{
    if(open_flag == 0 ) return(ret_error);
	graphManager->CheckinMemoryBuffer(true);
	graphManager->Stop();
	delete graphManager;
    return(ret_success);
}

// -----------------------------------------------------------------------------------------------------------------

int arVideoDispOption(void)
{
	return(ret_not_implemented);
}

// -----------------------------------------------------------------------------------------------------------------

int arVideoInqSize( int *x, int *y )
{
    if(open_flag == 0) return(ret_error);

	DS_MEDIA_FORMAT mf;
	graphManager->GetCurrentMediaFormat(&mf);
	*x = (int) mf.biWidth;
	*y = (int) mf.biHeight;
    return(ret_success);
}

// -----------------------------------------------------------------------------------------------------------------

unsigned char *arVideoGetImage(void)
{
    if(open_flag == 0) return(NULL);

	DWORD wait_result = graphManager->WaitForNextSample(frame_timeout_ms);
	if(wait_result == WAIT_OBJECT_0)
	{
		unsigned char* pixelBuffer;
		if(FAILED(graphManager->CheckoutMemoryBuffer(&pixelBuffer))) return(NULL);
		if(flip_horizontal || (flip_vertical ^ FLIPPED_defined))
		{
			DS_MEDIA_FORMAT mf;
			graphManager->GetCurrentMediaFormat(&mf);
			FlipImageRGB32(pixelBuffer, mf.biWidth, mf.biHeight, 
			                 flip_horizontal, flip_vertical ^ FLIPPED_defined);
		}
		return(pixelBuffer);
	}
	else return(NULL);
}

// -----------------------------------------------------------------------------------------------------------------

int arVideoCapStart(void)
{
	if(FAILED(graphManager->Run())) return(ret_error);
	else return(ret_success);
}

// -----------------------------------------------------------------------------------------------------------------

int arVideoCapStop(void)
{
	graphManager->CheckinMemoryBuffer(true);
	if(FAILED(graphManager->Stop())) return(ret_error);
	else return(ret_success);
}

// -----------------------------------------------------------------------------------------------------------------

int arVideoCapNext( void )
{
	return(SUCCEEDED(graphManager->CheckinMemoryBuffer()) ? 
			ret_success : ret_error);
}

// -----------------------------------------------------------------------------------------------------------------

void FlipImageRGB32(BYTE* pBuf, int width, int height, 
					bool flipImageH, bool flipImageV)
{
	DWORD *ptr = (DWORD*)pBuf;
	int pixelCount = width * height;

	if (!pBuf)
		return;

    // Added code for more image manipulations
    // Gerhard Reitmayr <reitmayr@ims.tuwien.ac.at

    if( flipImageV )
    {
        if( flipImageH )
        {
            // both flips set -> rotation about 180 degree
	        for (int index = 0; index < pixelCount/2; index++)
	        {
		        ptr[index] = ptr[index] ^ ptr[pixelCount - index - 1];
		        ptr[pixelCount - index - 1] = ptr[index] ^ ptr[pixelCount - index - 1];
		        ptr[index] = ptr[index] ^ ptr[pixelCount - index - 1];
	        }
        } 
        else  
        {
            // only vertical flip 
            for( int line = 0; line < height/2; line++ )
                for( int pixel = 0; pixel < width; pixel ++ )
                {
                    ptr[line*width+pixel] = ptr[line*width+pixel] ^ ptr[pixelCount - line*width - (width - pixel )];
                    ptr[pixelCount - line*width - (width - pixel )] = ptr[line*width+pixel] ^ ptr[pixelCount - line*width - (width - pixel )];
                    ptr[line*width+pixel] = ptr[line*width+pixel] ^ ptr[pixelCount - line*width - (width - pixel )];
                }
        }
    }
    else 
    {
        if( flipImageH )
        {
            // only horizontal flip
            for( int line = 0; line < height; line++ )
                for( int pixel = 0; pixel < width/2; pixel ++ )
                {
                    ptr[line*width+pixel] = ptr[line*width+pixel] ^ ptr[line*width + (width - pixel )];
                    ptr[line*width + (width - pixel )] = ptr[line*width+pixel] ^ ptr[line*width + (width - pixel )];
                    ptr[line*width+pixel] = ptr[line*width+pixel] ^ ptr[line*width + (width - pixel )];
                }
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------
