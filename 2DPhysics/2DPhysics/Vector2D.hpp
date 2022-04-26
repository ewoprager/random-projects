#ifndef Vector2D_hpp
#define Vector2D_hpp

#include <iostream>
#include <cmath>

struct Vector2D {
	Vector2D(float _x, float _y){
		x = _x;
		y = _y;
	}
	Vector2D(){
		x = 0;
		y = 0;
	}
	
	friend Vector2D &operator+(const Vector2D vec1, const Vector2D vec2) {
		Vector2D *output = new Vector2D();
		output->x = vec1.x + vec2.x;
		output->y = vec1.y + vec2.y;
		return *output;
	}
	friend Vector2D &operator-(const Vector2D vec1, const Vector2D vec2) {
		Vector2D *output = new Vector2D();
		output->x = vec1.x - vec2.x;
		output->y = vec1.y - vec2.y;
		return *output;
	}
	friend Vector2D &operator*(const Vector2D vec, const float fl){
		Vector2D *output = new Vector2D();
		output->x = vec.x*fl;
		output->y = vec.y*fl;
		return *output;
	}
	friend Vector2D &operator*(const float fl, const Vector2D vec){
		Vector2D *output = new Vector2D();
		output->x = vec.x*fl;
		output->y = vec.y*fl;
		return *output;
	}
	friend Vector2D &operator/(const Vector2D vec, const float fl){
		Vector2D *output = new Vector2D();
		output->x = vec.x/fl;
		output->y = vec.y/fl;
		return *output;
	}
	Vector2D &operator+=(const Vector2D vec){
		*this = *this + vec;
		return *this;
	}
	Vector2D &operator-=(const Vector2D vec){
		*this = *this - vec;
		return *this;
	}
	Vector2D &operator*=(const float fl){
		*this = *this * fl;
		return *this;
	}
	Vector2D &operator/=(const float fl){
		*this = *this / fl;
		return *this;
	}
	friend std::ostream &operator<<(std::ostream &stream, const Vector2D &vec){
		stream << "(" << vec.x << ", " << vec.y << ")";
		return stream;
	}
	
	Vector2D Crossed(){
		return Vector2D(this->y, -this->x);
	}
	Vector2D Normalised(){
		return *this / sqrt(SqMag());
	}
	float SqMag(){
		return x*x + y*y;
	}
	Vector2D Squared(){
		return *this * sqrt(SqMag());
	}
	
	float x;
	float y;
};

Vector2D RandomUnitVector2D();
float Dot(Vector2D vec1, Vector2D vec2);
Vector2D RadAngle(float rad, double angle, double angle2=0);
Vector2D UnitInDir(double angle);

#endif /* Vector2D_hpp */
