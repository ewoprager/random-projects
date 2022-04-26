#ifndef Polygon_hpp
#define Polygon_hpp

#include "App.hpp"
#include "RigidbodyParent.hpp"

class Polygon : public RigidbodyParent {
public:
	Polygon(App* app, bool _fixed, float density, Vector2D initialPos, std::initializer_list<Vector2D> list, double initialAngle=0, float _mu=0, float _cor=1); // must define vertices clockwise
	// https://www.mathopenref.com/coordpolycalc.html - for finding locations of vertices of regular polygons
	~Polygon() {}
	
	void Update(float deltaT) override { RigidbodyParent::Update(deltaT); geometryCalculated = false; dirsNormalised = false; }
	void Render(SDL_Renderer *renderer) override;
	
	void CalculateGeometry();
	void NormaliseDirs();
	
	bool PointInside(Vector2D point, float radius=0) override;
	float PointInsideDistance(Vector2D point, Vector2D *dirDest, float radius=0) override; // returns dirDest as a unnit vector
	
	int GetN(){ return N; }
	Vector2D GetVertex(int which);
	
private:
	int N;
	
	RGB colour;
	
	float rads[MAX_VERTICES];
	double angles[MAX_VERTICES];
	
	bool geometryCalculated = false;
	bool dirsNormalised = false;
	Vector2D vertexPoss[MAX_VERTICES];
	Vector2D sideDirs[MAX_VERTICES]; // (are not necessarily normalised)
};

#endif /* Polygon_hpp */
