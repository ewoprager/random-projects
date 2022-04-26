#include "Vector2D.hpp"

Vector2D RandomUnitVector2D(){
	Vector2D output;
	output.x = -1 + (float)(rand() % 100)/50.0f;
	output.y = sqrt(1 - output.x*output.x);
	
	return output;
}

float Dot(Vector2D vec1, Vector2D vec2){
	return vec1.x*vec2.x + vec1.y*vec2.y;
}

Vector2D RadAngle(float rad, double angle, double angle2){
	return Vector2D(rad*cos(angle + angle2), -rad*sin(angle + angle2));
}

Vector2D UnitInDir(double angle){
	return Vector2D(cos(angle), -sin(angle));
}
