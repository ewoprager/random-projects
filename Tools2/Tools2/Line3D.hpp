#ifndef Line3D_h
#define Line3D_h

#include "Vector3D.hpp"

class Line3D {
public:
	Line3D(Vector3D _point, Vector3D _direction){
		point = _point;
		direction = _direction;
	}
	Line3D(){
		point = Vector3D();
		direction = Vector3D(1, 0, 0);
	}
	
	Line3D Normalise(){
		float sm = direction.SqMag();
		if(sm != 1){
			direction /= sqrt(sm);
		}
		return *this;
	}
	
	friend std::ostream &operator<<(std::ostream &stream, const Line3D &line){
		stream << line.point << line.direction;
		return stream;
	}
	
	Vector3D point;
	Vector3D direction;
};

float SqDist(Vector3D point, Line3D line){
	return Cross3D(point - line.point, line.direction).SqMag()/line.direction.SqMag();
}

float SqDist(Line3D line1, Line3D line2){
	return Dot3D(Cross3D(line1.direction, line2.direction), line1.point - line2.point);
}

#endif /* Line3D_h */
