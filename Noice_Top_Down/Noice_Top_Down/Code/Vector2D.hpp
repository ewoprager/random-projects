#ifndef Vector2D_hpp
#define Vector2D_hpp

#define PI 3.141592654

#include <stdio.h>
#include <iostream>
#include <cmath>

class Vector2D {
public:
    Vector2D();
    Vector2D(float _x, float _y);
    
    static Vector2D &Add(const Vector2D &v1, const Vector2D &v2);
    static Vector2D &Subtract(const Vector2D &v1, const Vector2D &v2);
    static Vector2D &Multiply(const Vector2D &v1, const Vector2D &v2);
    static Vector2D &Divide(const Vector2D &v1, const Vector2D &v2);
	
	static Vector2D &Multiply(const Vector2D &v1, const float &val);
    
    friend Vector2D &operator+(Vector2D v1, const Vector2D v2);
    friend Vector2D &operator-(Vector2D v1, const Vector2D v2);
    friend Vector2D &operator*(Vector2D v1, const Vector2D v2);
    friend Vector2D &operator/(Vector2D v1, const Vector2D v2);
	
	friend Vector2D &operator*(Vector2D v1, const float val);
    
    Vector2D &operator+=(const Vector2D &vec);
    Vector2D &operator-=(const Vector2D &vec);
    Vector2D &operator*=(const Vector2D &vec);
    Vector2D &operator/=(const Vector2D &vec);
	
	Vector2D &operator*=(const float &val);
    
    friend std::ostream &operator<<(std::ostream &stream, const Vector2D &vec);
    
    Vector2D IntVector();
    
    Vector2D Flip();
	
	float SqMag();
    
	Vector2D Normalise();
	
	double Angle();
	
    float x;
    float y;
    
};

#endif /* Vector2D_hpp */
