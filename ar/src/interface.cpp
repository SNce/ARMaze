// Definitions for interface.h
#include"interface.h"

IplImage* convertToCV(ARUint8* input,int xsize,int ysize)
{
	IplImage * out;
	CvSize size;

	size.width = xsize;
	size.height = ysize;

	out = cvCreateImage(size,IPL_DEPTH_8U,4);

	out->imageData = (char*)input;
	
	cvNamedWindow("CV",CV_WINDOW_AUTOSIZE);
	cvShowImage("CV",out);

	return out;
}