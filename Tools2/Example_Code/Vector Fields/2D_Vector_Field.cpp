#include "Header.hpp"
#include "Axes.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"
#include "Plane3D.hpp"
#include "Vector2D.hpp"
#include "HashTable.hpp"

const int size = 2048;
 
float x[size];
float y[size];
float f[size*size];
float fOffset[size*size];
Vector2D gradF[size*size];

LerpedIndex2D indices[size*size];
LerpedIndex2D gradIndices[size*size];

Axes<25, size*size> *axes;

RGB fieldColour = {50, 0, 180};
RGB contourColour = {150, 240, 240};

float Function(float x, float y){
	return cos(4*x) + cos(4*y);
}

int main() {
	 
	Init_RandomSeed();
	
	Linspace(size, x, -0.1f, 1.0f);
	Linspace(size, y, -0.1f, 1.0f);
	
	PerElement2D(size, f, x, y, &Function);
	
	axes = new Axes<25, size*size>({false, -0.1f, 1.0f, "x"}, {false, -0.1f, 1.0f, "y"});
	
	int n;
	float offset;
	for(int i=0; i<21; i++){
		offset = -2.0f + (float)i/5.0f;
		PerElement(size*size, fOffset, f, offset, &Subtract);
		n = FindChangeOfSign2D(size, fOffset, indices);
		axes->PlotIndices2D(n, x, y, indices, offset, contourColour);
	}
	
	Grad(size, gradF, f, x, y);
	
	axes->AddVectorField2D(size, 30, gradF, x, y, fieldColour);
	
	int stptsN = Find2DChangeOfSign2D(size, gradF, gradIndices);
	for(int i=0; i<stptsN; i++){
		axes->HighlightIndex2D(x, y, gradIndices[i]);
	}
	
	axes->Execute();
	
	return 0;
}
