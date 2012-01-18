#include"graphics.h"

void print_string(char *string,float color[3],float pos[3],void *font)
{
  int     i;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glTranslatef(pos[0],pos[1],pos[2]);
  glColor3fv(color);
  glRasterPos2i(0.0, 0.0);
  for (i=0; i<(int)strlen(string); i++) {
      if(string[i] != '\n' )
		  glutBitmapCharacter(font, string[i]);
      
      else {
          glTranslatef(0.0, -0.07, 0.0);
          glRasterPos2i(0.0, 0.0);
      }
  }
}

void print_time(float time)
{
	static int min = 00;
	static int seconds = 00;
	static int ms = 0;
	char display_buffer[256];
	float color[3] = {0.75,0.75,0.0};
	float pos[3] = {0.5,0.25,0.0};

	ms += (int)(1000 * time);

	if(ms > 1000)
	{
		ms -= 1000;
		seconds++;
	}

	if(seconds > 60)
	{
		seconds -= 60;
		min++;
	}

	sprintf(display_buffer,"%d:%d:%d",min,seconds,ms);
	//print_string(display_buffer,color,pos,GLUT_BITMAP_HELVETICA_18);

}
