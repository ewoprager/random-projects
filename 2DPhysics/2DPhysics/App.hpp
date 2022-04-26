#ifndef App_hpp
#define App_hpp

#define MAX_OBJECTS 63
#define METRES_PER_PIXEL 0.01
#define PI 3.141592654
#define MAX_VERTICES 32
#define GRAVITY 90.0f

#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include "Vector2D.hpp"

class App;
class ObjectParent;
class RigidbodyParent;
class CollisionHandler;
class Polygon;
class Circle;

enum Shape {polygon, circle};

struct RBPivot {
	RigidbodyParent *with;
	float rad;
	double angle;
};

struct RGB {
	short unsigned int red;
	short unsigned int green;
	short unsigned int blue;
};

int GetNewId(int size, bool bools[]);
template <typename T=float, typename U=float> void Sort(int size, T array[], U another[]=nullptr){
	T temp;
	U atemp;
	int i, j;
	for(i=1; i<size; i++){
		j=i;
		while(j > 0 ? array[j] < array[j-1] : false){
			temp = array[j];
			array[j] = array[j-1];
			array[j-1] = temp;
			if(another != nullptr){
				atemp = another[j];
				another[j] = another[j-1];
				another[j-1] = atemp;
			}
			j--;
		}
	}
}
void DrawArrow(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float headRatio=0.3);
void DrawCircle(SDL_Renderer *renderer, Vector2D pos, int32_t radius);


class App {
public:
	App();
	
	bool Init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
	void Clean();
	bool Running(){ return isRunning; }
	
	void HandleEvents();
	void LoopStart(int deltaT);
	void LoopEnd(int deltaT);
	void Render();
	
	int AddToRender(ObjectParent *object);
	int AddToUpdate(RigidbodyParent *object);
	
	SDL_Renderer *renderer;
	
	CollisionHandler *collisionHandler;
	
private:
	float gameSpeed = 0.05f;
	float latestDeltaT = 1;
	
	bool isRunning = false;
	
	SDL_Window *window;
	
	SDL_Event event;
	
	bool render[MAX_OBJECTS];
	ObjectParent *toRender[MAX_OBJECTS];
	
	bool update[MAX_OBJECTS];
	RigidbodyParent *toUpdate[MAX_OBJECTS];
	
	bool leftButtonDown = false;
	bool rightButtonDown = false;
	void LeftButtonDown(Vector2D pos);
	void LeftButtonUp(Vector2D pos);
	void RightButtonDown(Vector2D pos, int time);
	void RightButtonUp(Vector2D pos, int time);
	// lmb:
	bool dragging = false;
	float dragStartRad;
	double dragStartAngle;
	RigidbodyParent *bodyDragging;
	// rmb:
	bool firing = false;
	float fireStartRad;
	double fireStartAngle;
	int fireStartTime;
	RigidbodyParent *bodyFiring;
	
	// entities
	Polygon *wall1;
	Polygon *wall2;
	Polygon *wall3;
	Polygon *wall4;
	Polygon *poly1;
	Polygon *poly2;
	Circle *circ1;
	Circle *circ2;
	bool rightDown = false;
	bool leftDown = false;
};

#endif /* App_hpp */
