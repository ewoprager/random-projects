#include "Vector2D.hpp"

Vector2D::Vector2D(){
    x = 0.0f;
    y = 0.0f;
}
Vector2D::Vector2D(float _x, float _y){
    x = _x;
    y = _y;
}

Vector2D &Vector2D::Add(const Vector2D &v1, const Vector2D &v2) {
    Vector2D *outputVec = new Vector2D();
    outputVec->x = v1.x + v2.x;
    outputVec->y = v1.y + v2.y;
    return *outputVec;
}
Vector2D &Vector2D::Subtract(const Vector2D &v1, const Vector2D &v2) {
    Vector2D *outputVec = new Vector2D();
    outputVec->x = v1.x - v2.x;
    outputVec->y = v1.y - v2.y;
    return *outputVec;
}
Vector2D &Vector2D::Multiply(const Vector2D &v1, const Vector2D &v2) {
    Vector2D *outputVec = new Vector2D();
    outputVec->x = v1.x * v2.x;
    outputVec->y = v1.y * v2.y;
    return *outputVec;
}
Vector2D &Vector2D::Divide(const Vector2D &v1, const Vector2D &v2) {
    Vector2D *outputVec = new Vector2D();
    outputVec->x = v1.x / v2.x;
    outputVec->y = v1.y / v2.y;
    return *outputVec;
}

Vector2D &Vector2D::Multiply(const Vector2D &v1, const float &val) {
    Vector2D *outputVec = new Vector2D();
    outputVec->x = v1.x * val;
    outputVec->y = v1.y * val;
    return *outputVec;
}

Vector2D &operator+(Vector2D v1, const Vector2D v2){
    return Vector2D::Add(v1, v2);
}
Vector2D &operator-(Vector2D v1, const Vector2D v2){
    return Vector2D::Subtract(v1, v2);
}
Vector2D &operator*(Vector2D v1, const Vector2D v2){
    return Vector2D::Multiply(v1, v2);
}
Vector2D &operator/(Vector2D v1, const Vector2D v2){
    return Vector2D::Divide(v1, v2);
}

Vector2D &operator*(Vector2D v1, const float val){
	return Vector2D::Multiply(v1, val);
}

Vector2D &Vector2D::operator+=(const Vector2D &vec){
    *this = Add(*this, vec);
    return *this;
}
Vector2D &Vector2D::operator-=(const Vector2D &vec){
    *this = Subtract(*this, vec);
    return *this;
}
Vector2D &Vector2D::operator*=(const Vector2D &vec){
    *this = Multiply(*this, vec);
    return *this;
}
Vector2D &Vector2D::operator/=(const Vector2D &vec){
    *this = Divide(*this, vec);
    return *this;
}

Vector2D &Vector2D::operator*=(const float &val){
    *this = Multiply(*this, val);
    return *this;
}

std::ostream &operator<<(std::ostream &stream, const Vector2D &vec){
    stream << "(" << vec.x << ", " << vec.y << ")";
    return stream;
}

Vector2D Vector2D::IntVector(){
    return Vector2D((int)x, (int)y);
}

Vector2D Vector2D::Flip(){
    x = -x;
    return *this;
}

float Vector2D::SqMag(){
	return x*x + y*y;
}

Vector2D Vector2D::Normalise(){
	float mag = sqrt(SqMag());
	x = x/mag;
	y = y/mag;
	return *this;
}

double Vector2D::Angle(){
	return (x < 0) ? (PI + atan(y / x)) : (atan(y / x));
}
