#ifndef _APPLICATION
#define _APPLICATION

#include<AR/ar.h>
#include<AR/gsub.h>
#include<AR/video.h>
#include<AR/param.h>

#include <stdlib.h>

#include "object.h"

class Game
{
private:
	static ARParam cparam;
	static char* vconf;
	static char* cparam_name;
	static char* patt_name;
	static int xsize;
	static int ysize;
	static int count;
	static const int threshold = 100;
	static int patt_id;
	static double patt_width;
	static double patt_centre[2];
	static double patt_trans[3][4];

	static Maze* maze;
	
public:
	static void init();
	static void mainLoop();
	static void keyEvent(unsigned char key, int x, int y);
	static void draw();
	static void cleanup();
};

#endif