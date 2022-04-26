#ifndef Plane3D_h
#define Plane3D_h

#include "Line3D.hpp"

class Plane3D {
public:
	Plane3D(Vector3D _point, Vector3D _dir1, Vector3D _dir2){
		point = _point;
		dir1 = _dir1;
		dir2 = _dir2;
		if(Parallel3D(dir1, dir2)){
			std::cout << "Plane3D created with parallel direction vectors.\n";
		}
	}
	Plane3D(Line3D line, Vector3D _dir2){
		point = line.point;
		dir1 = line.direction;
		dir2 = _dir2;
		if(Parallel3D(dir1, dir2)){
			std::cout << "Plane3D created with parallel direction vectors.\n";
		}
	}
	Plane3D(){
		point = Vector3D();
		dir1 = Vector3D(1, 0, 0);
		dir2 = Vector3D(0, 1, 0);
	}
	
	Vector3D N(){
		return Cross3D(dir1, dir2);
	}
	
	friend std::ostream &operator<<(std::ostream &stream, const Plane3D &plane){
		stream << plane.point << plane.dir1 << plane.dir2;
		return stream;
	}
	
	Vector3D point;
	Vector3D dir1;
	Vector3D dir2;
};

bool Parallel3D(Vector3D vec, Plane3D plane){
	return Parallel3D(plane.N(), Cross3D(vec, plane.dir1));
}

bool Parallel3D(Plane3D plane1, Plane3D plane2){
	return Parallel3D(plane1.N(), plane2.N());
}

Vector3D Intersect3D(Line3D line, Plane3D plane){
	if(Parallel3D(line.direction, plane)){
		std::cout << "Line and plane do not intersect.\n";
		return Vector3D();
	}
	float t = Dot3D(plane.N(), line.point - plane.point)/Dot3D(Vector3D(0, 0, 0)-line.direction, plane.N());
	return line.point + line.direction*t;
}

Line3D Intersect3D(Plane3D plane1, Plane3D plane2){
	if(Parallel3D(plane1, plane2)){
		std::cout << "Planes do not intersect.\n";
		return Line3D();
	}
	Line3D line = Line3D();
	line.direction = Cross3D(plane1.N(), plane2.N());
	if(!Parallel3D(plane1.dir1, plane2)){
		line.point = Intersect3D(Line3D(plane1.point, plane1.dir1), plane2);
	} else {
		line.point = Intersect3D(Line3D(plane1.point, plane1.dir2), plane2);
	}
	return line;
}

#endif /* Plane3D_h */
