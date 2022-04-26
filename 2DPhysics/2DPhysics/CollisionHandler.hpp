#ifndef CollisionHandler_hpp
#define CollisionHandler_hpp

#include "App.hpp"

struct Vertex {
	//int index;
	bool inInotJ;
	Vector2D pos;
	float distIn;
	Vector2D dir;
};

struct Collision {
	Vector2D position;
	bool staticFric;
	
	int whosIn;
	Vector2D jumpN_whosIn;
	Vector2D impN_whosIn;
	Vector2D jumpT_whosIn;
	Vector2D impT_whosIn;
	Vector2D fricForce_whosIn;
	
	int inWho;
	Vector2D jumpN_inWho;
	Vector2D impN_inWho;
	Vector2D jumpT_inWho;
	Vector2D impT_inWho;
	Vector2D fricForce_inWho;
};

struct Pivot {
	int object1;
	float rad1;
	double angle1;
	int object2;
	float rad2;
	double angle2;
};

struct Correction {
	int object1;
	Vector2D point1;
	Vector2D jump1;
	Vector2D imp1;
	
	int object2;
	Vector2D point2;
	Vector2D jump2;
	Vector2D imp2;
};

struct ShapedRB {
	RigidbodyParent *rbp;
	Shape shape;
	int index; // position in array of rigibodies of this shape
};

class CollisionHandler {
public:
	CollisionHandler(App* app) {
		theApp = app;
		for(int i=0; i<MAX_OBJECTS; i++){
			handle[i] = false;
			cantCollideN[i] = 0;
			_polygon[i] = false;
			_circle[i] = false;
		}
	}
	
	void DetectCollisions();
	void CorrectCollisions(float deltaT);
	
	void AddCantCollide(int obj1, int obj2);
	
	void AddPivot(int obj1, int obj2, Vector2D position, bool cantCollide=true);
	
	RigidbodyParent *CollisionPoint(Vector2D pos);
	
	int AddToHandle(RigidbodyParent *object);
	
private:
	App* theApp;
	
	bool handle[MAX_OBJECTS];
	ShapedRB toHandle[MAX_OBJECTS];
	
	bool _polygon[MAX_OBJECTS];
	Polygon *polygons[MAX_OBJECTS];
	bool _circle[MAX_OBJECTS];
	Circle *circles[MAX_OBJECTS];
	
	int cantCollideN[MAX_OBJECTS];
	int cantCollide[MAX_OBJECTS][MAX_OBJECTS];
	
	int colsN = 0;
	Collision cols[MAX_OBJECTS*MAX_OBJECTS];
	
	int pivotsN = 0;
	Pivot pivots[MAX_OBJECTS*MAX_OBJECTS];
};

#endif /* CollisionHandler_hpp */
