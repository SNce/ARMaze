///////////////////////////////////////////////////////////////
//
//			Augmented Reality Game
//						-C Krishnakanth Mallik
//
//
///////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "game.h"

#include<AR/ar.h>


int main(int args, char** argv)
{
	Game* game = new Game();
	Game::init();
	arVideoCapStart();
	argMainLoop(NULL, &Game::keyEvent, &Game::mainLoop);
	return(0);
}