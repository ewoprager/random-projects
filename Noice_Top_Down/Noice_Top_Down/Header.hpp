#ifndef Header_h
#define Header_h

#define MAX_OBJECTS 255
#define ONE_OVER_ROOT_TWO 0.7071067812
#define TILE_SIZE 32
#define MAP_WIDTH 40
#define MAP_HEIGHT 32
#define VIEW_WIDTH 24
#define VIEW_HEIGHT 20
#define SPRITE_SIZE 8

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <math.h>
#include "Vector2D.hpp"
#include "audio.hpp"

// other
class App;
class Game;
class TextureManager;
class Wall;

// event handling
class LinkParent;
template <typename T> class EventHandleLink;
class EventHandler;

// view
class View;

// updating/rendering
class UpdateRenderParent;
class DoUpdates;
class DoRenders;

// collisions
class CollisionHandler;
class Collider;
class CollisionMask;

// enemy handler
class EnemyHandler;

// game objects
class Player;
class Bullet;
class Grenade;

class Sprite;
class EnemyParent;
class Slime;

// particles
class Emitter;
class Particle;


// global stuff
int GetNewId(int size, bool ons[]);
// audio
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play
void my_audio_callback(void *userdata, Uint8 *stream, int len);
float findVolume(Vector2D pos1, Vector2D pos2, float sqMaxDist);
void DrawCircle(SDL_Renderer * renderer, Vector2D pos, int32_t radius);
void MoveWithoutCollision(Game *theGame, Vector2D *pos, Vector2D *vel, Vector2D*desiredVel, CollisionMask *mask, float acc, int except=-1);
float ExplosionFunction(float maxDmg, float sqRange, float sqDist);

#endif /* Header_h */
