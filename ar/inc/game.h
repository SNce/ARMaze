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
	static double ref_trans[3][4];

	static float abs_gravity[4];
	static float grav_trans[3];

	static char* cap_msg;
	static char* start_msg;
	static float text_pos[3];
	static float text_color[3];

	static int patt_visible;
	static int ref_set;

	static float prev_time;

	static Maze* maze;
	
public:
	static void init();
	static void mainLoop();
	static void keyEvent(unsigned char key, int x, int y);
	static void draw();
	static void cleanup();
};

#endif