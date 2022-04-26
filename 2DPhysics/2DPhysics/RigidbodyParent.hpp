#ifndef RigidbodyParent_h
#define RigidbodyParent_h

#include "App.hpp"
#include "CollisionHandler.hpp"
#include "ObjectParent.hpp"

class RigidbodyParent : public ObjectParent {
public:
	RigidbodyParent(App* app, bool _fixed, float _mass, Vector2D initialPos, double initialAngle=0, float _mu=0, float _cor=1, float _smoa=1);
	// children should define maxRadius and shape in constructor, as well as add themselves to be handled, updated and rendered if necessary
	
	virtual void Render(SDL_Renderer *renderer) override {}
	virtual void Update(float deltaT) override; // children should override with RigidbodyParent::Update(deltaT)
	
	virtual bool PointInside(Vector2D point, float radius=0) { return false; }
	virtual float PointInsideDistance(Vector2D point, Vector2D *dirDest, float radius=0){ return 0; } // should return dirDest as a unit vector
	
	// read only variables
	bool GetFixed(){ return fixed; }
	float GetMaxRadius(){ return maxRadius; }
	Vector2D GetPosition(){ return position; }
	double GetAngle(){ return angle; }
	float GetMu(){ return mu; }
	float GetCor(){ return cor; }
	int GetUpdateId(){ return updateId; }
	int GetHandleId(){ return handleId; }
	Shape GetShape(){ return shape; }
	
	Vector2D GetPointPos(float radius, double relAngle);
	Vector2D GetPointVel(Vector2D point);
	
	void SetPivot(RigidbodyParent *with, float rad, double angle);
	
	void ApplyMoment(double moment); // [/s])
	void ApplyForce(Vector2D point, Vector2D force, bool local=false); // [metres/s]
	void ApplyImpulse(Vector2D point, Vector2D impulse, bool piv=false, bool local=false); // [metres/s]
	void ApplyJump(Vector2D point, Vector2D jump, bool piv=false, bool local=false); // [pixels/dt]
	
	float GetRecipEffMass(Vector2D point, Vector2D dir, float *mulLinear=nullptr, double *mulRot=nullptr, bool piv=false, bool local=false);
	
protected:
	Shape shape;
	
	bool fixed;
	App *theApp;
	int updateId;
	int handleId;
	int maxRadius;
	float mu;
	float cor;
	
	float mass;	
	Vector2D position;
	Vector2D velocity;
	Vector2D newAccel;
	Vector2D avgAccel;
	Vector2D accel;
	Vector2D totalForce = Vector2D();
	float linearDamping = 2;
	
	double smoa;
	double angle;
	double angVel;
	double newAngAcc;
	double avgAngAcc;
	double angAcc;
	double totalMoment = 0;
	double angularDamping = 0.01;
	
	// not in use
	bool pivot = false;
	RBPivot thePivot;
};

#endif /* RigidbodyParent_h */
