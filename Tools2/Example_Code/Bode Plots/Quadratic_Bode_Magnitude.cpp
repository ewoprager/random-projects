#include "Header.hpp"
#include "Axes.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"
#include "Plane3D.hpp"
#include "Vector2D.hpp"
#include "HashTable.hpp"

const int size = 1000;
const int damps = 20;
float x[size];
float y[damps][size];
float damp[damps];

Axes<damps, size> *axes;

float Func(float _x, float _damp){
	return 1 / sqrt((1 - _x*_x)*(1 - _x*_x) + 4*_damp*_damp*_x*_x);
}

int main() {
	
	axes = new Axes<damps, size>();
	
	Logspace(size, x, 0.1f, 10.0f);
	Logspace(damps, damp, 0.05f, 2.0f);
	
	for(int i=0; i<damps; i++){
		PerElement(size, y[i], x, damp[i], &Func);
		axes->AddPlot(size, x, y[i]);
	}
	
	axes->Execute();
	
	return 0;
}
