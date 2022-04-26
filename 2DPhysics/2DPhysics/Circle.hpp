#ifndef Circle_hpp
#define Circle_hpp

#include "App.hpp"
#include "RigidbodyParent.hpp"

class Circle : public RigidbodyParent {
public:
	Circle(App* app, bool _fixed, float density, Vector2D initialPos, float _radius, float _mu=0, float _cor=1);
	~Circle() {}
	
	//void Update(float deltaT) override { RigidbodyParent::Update(deltaT); }
	void Render(SDL_Renderer *renderer) override;
	
	bool PointInside(Vector2D point, float _radius=0) override;
	float PointInsideDistance(Vector2D point, Vector2D *dirDest, float _radius=0) override; // returns dirDest as a unit vector
	
	float GetRadius(){ return radius; }
	
private:
	float radius;
	
	RGB colour;
};

#endif /* Circle_hpp */
