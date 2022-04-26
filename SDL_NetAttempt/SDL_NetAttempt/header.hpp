#ifndef header_h
#define header_h

#define MAX_PACKETS 0xFF
#define MAX_SOCKETS 0x10

#define WOOD_WAIT_TIME 5000

#define FLAG_QUIT 0x0000
#define FLAG_NUMBER 0x0010
#define FLAG_NEWPLAYER 0x0011
#define FLAG_PLAYERQUIT 0x0012
#define FLAG_HELLO 0x0013
#define FLAG_SNUMBER 0x0014

#include <iostream>
#include "/Library/Frameworks/SDL2.framework/Headers/SDL.h"
#include "/Library/Frameworks/SDL2_net.framework/Headers/SDL_net.h"

typedef struct {
	int in_use = 0;
	
	float x;
	float y;
	short int id;
} OtherClient;

#endif /* header_h */
