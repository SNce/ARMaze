// interface functions between ARToolkit and OpenCV
// C Krishnakanth Mallik

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include<AR/ar.h>
//#include<cv.h>
//#include<highgui.h>

IplImage* convertToCV(ARUint8* input,int xsize,int ysize);

#endif