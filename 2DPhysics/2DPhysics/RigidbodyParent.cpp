#include "RigidbodyParent.hpp"

RigidbodyParent::RigidbodyParent(App* app, bool _fixed, float _mass, Vector2D initialPos, double initialAngle, float _mu, float _cor, float _smoa) {
	theApp = app;
	fixed = _fixed;
	mass = _mass;
	position = initialPos;
	velocity = Vector2D();
	accel = Vector2D();
	newAccel = Vector2D();
	avgAccel = Vector2D();
	angle = initialAngle;
	smoa = _smoa;
	mu = _mu;
	cor = _cor;
}

void RigidbodyParent::Update(float deltaT) {	
	if(fixed){ return; }
	
	//std::cout << position << ", " << velocity << ", " << accel << "\n";
	
	// translation
	position += velocity * deltaT + ( 0.5f * deltaT*deltaT * accel ); // [pixels/dt]
	newAccel = ( totalForce/mass ) / METRES_PER_PIXEL; // [pixels/s^2]
	avgAccel = ( accel + newAccel ) / 2.0f;
	velocity += avgAccel * deltaT; // [pixels/s]
	accel = newAccel;
	// resetting force
	Vector2D avgVel = velocity + ( 0.5f * accel * deltaT );
	totalForce = mass * Vector2D(0.0f, GRAVITY) - avgVel * METRES_PER_PIXEL * linearDamping; // [metres/s]
	
	// rotation
	angle += angVel * deltaT + ( 0.5f * deltaT*deltaT * angAcc ); // [/dt]
	newAngAcc = ( totalMoment/(smoa*mass) ); // [/s^2]
	avgAngAcc = ( angAcc + newAngAcc ) / 2.0f;
	angVel += avgAngAcc * deltaT; // [/s]
	angAcc = newAngAcc;
	// resetting moment
	double avgAngVel = angVel + ( 0.5f * angAcc * deltaT );
	totalMoment = - avgAngVel * angularDamping; // [/s]
}

Vector2D RigidbodyParent::GetPointPos(float radius, double relAngle) {
	return Vector2D(position.x + radius*cos(angle + relAngle), position.y - radius*sin(angle + relAngle));
}
Vector2D RigidbodyParent::GetPointVel(Vector2D point){ // [pixels/s]
	if(fixed){ return Vector2D(); }
	Vector2D relPos = point - position;
	return velocity + angVel * relPos.Crossed();
}

void RigidbodyParent::SetPivot(RigidbodyParent *with, float rad, double angle){
	thePivot.with = with;
	thePivot.rad = rad;
	thePivot.angle = angle;
	pivot = true;
}

void RigidbodyParent::ApplyMoment(double moment){ // [/s])
	totalMoment += moment;
}
void RigidbodyParent::ApplyForce(Vector2D point, Vector2D force, bool local){ // [metres/s]
	if(fixed){ return; }
	if(force.SqMag() == 0){ return; }
	Vector2D relPoint = point;
	if(!local){ relPoint -= position; }
	
	float forceMag = sqrt(force.SqMag());
	Vector2D unitRightAngle = -1 * force.Crossed()/forceMag;
	float d = METRES_PER_PIXEL * Dot(relPoint, unitRightAngle);
	
	totalForce += force;
	
	totalMoment += forceMag*d;
}

void RigidbodyParent::ApplyImpulse(Vector2D point, Vector2D impulse, bool piv, bool local){ // [metres/s]
	if(fixed){ return; }
	if(impulse.SqMag() == 0){ return; }
	
	float mulLinear;
	double mulRot;
	GetRecipEffMass(point, impulse, &mulLinear, &mulRot);
	
	Vector2D velJ = impulse * mulLinear / METRES_PER_PIXEL;
	velocity += velJ;
	double avelJ = sqrt(impulse.SqMag()) * mulRot;
	angVel += avelJ;
	
	// not in use
	/*
	if(pivot && piv){
		Vector2D local_pLoc = RadAngle(thePivot.rad, angle, thePivot.angle);
		thePivot.with->ApplyImpulse(deltaT, position + local_pLoc, velJ + avelJ * (local_pLoc.Crossed()));
	}
	*/
}

void RigidbodyParent::ApplyJump(Vector2D point, Vector2D jump, bool piv, bool local){ // [pixels/dt]
	if(fixed){ return; }
	if(jump.SqMag() == 0){ return; }
	
	float mulLinear;
	double mulRot;
	GetRecipEffMass(point, jump, &mulLinear, &mulRot);
	
	Vector2D jumpL = jump * mulLinear;
	position += jumpL;
	double jumpR = sqrt(jump.SqMag()) * mulRot * METRES_PER_PIXEL;
	angle += jumpR;
	
	// not in use
	/*
	if(pivot && piv){
		Vector2D local_pLoc = RadAngle(thePivot.rad, angle, thePivot.angle);
		thePivot.with->ApplyJump(position + local_pLoc, jumpL + jumpR * (local_pLoc.Crossed()));
	}
	*/
}

float RigidbodyParent::GetRecipEffMass(Vector2D point, Vector2D dir, float *mulLinear, double *mulRot, bool piv, bool local){
	// returning
	if(fixed){
		if(mulLinear != nullptr){ *mulLinear = 0; }
		if(mulRot != nullptr){ *mulRot = 0; }
		//std::cout << "Fixed object asked for it's mass.\n";
		return 0;
	}
	// returning
	
	// returning
	if(dir.SqMag() == 0){
		if(mulLinear != nullptr){ *mulLinear = 1; }
		if(mulRot != nullptr){ *mulRot = 0; }
		return 1/mass;
	}
	// returning
	
	Vector2D relPoint;
	Vector2D unitRightAngle = -1 * dir.Crossed().Normalised();
	
	if(!(pivot && piv) || ( (pivot && piv) ? thePivot.rad == 0 : true )){
		relPoint = point;
		if(!local){	relPoint -= position; }
		float d = METRES_PER_PIXEL * Dot(relPoint, unitRightAngle);
		float divLin = 1 + d*d/smoa;
		// returning
		if(mulLinear != nullptr){ *mulLinear = 1/divLin; }
		if(mulRot != nullptr){ *mulRot = d/(divLin*smoa); }
		return divLin/mass;
		// returning
	} else {
		// not in use
		/*
		double I;
		Vector2D local_pLoc = RadAngle(thePivot.rad, angle, thePivot.angle);
		Vector2D pLoc = position + local_pLoc;
		if(thePivot.with->GetFixed()){
			// pivot is on fixed body
			I = mass*(smoa + local_pLoc.SqMag());
			relPoint = point - local_pLoc;
			if(!local){ relPoint -= position; }
		} else {
			// pivot is not on fixed body
			float oomR = thePivot.with->GetRecipEffMass(pLoc, local_pLoc.Crossed());
			float oofac1 = (1 + mass*oomR);
			float oofac2 = (1 + 1/(mass*oomR));
			I = mass*(smoa + local_pLoc.SqMag()/( oofac1*oofac1 )) + local_pLoc.SqMag()/( oomR * oofac2*oofac2 );
			relPoint = point - local_pLoc/oofac1;
			if(!local){ relPoint -= position; }
		}
		float d = METRES_PER_PIXEL * Dot(relPoint, unitRightAngle);
		float divLinoM = thePivot.with->GetRecipEffMass(pLoc, dir) + d*d/I;
		// returning
		if(mulLinear != nullptr){ *mulLinear = 1/(divLinoM*mass); }
		if(mulRot != nullptr){ *mulRot = d/(divLinoM*I); }
		return divLinoM;
		// returning
		*/
		std::cout << "Reached not in use zone.\n";
		if(mulLinear != nullptr){ *mulLinear = 1; }
		if(mulRot != nullptr){ *mulRot = 0; }
		return 1/mass;
	}
}
