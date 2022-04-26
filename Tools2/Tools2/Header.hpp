#ifndef Header_h
#define Header_h

// ----- Constants -----
#define PI 3.141592654f
#define E 2.718281828f
#define ROOT2 1.414213562f
#define I2D(size, x, y) ( (x) + (y) * (size) ) // arrays must be of size size^2
#define I3D(size, x, y, z) ( (x) + (y) * (size) + (z) * (size) * (size) ) // arrays must be of size size^3

#include <iostream>
#include "functions.hpp"
#include <stdlib.h>
#include <cstdarg>
#include "Vector2D.hpp"

// ----- Strucures and Classes -----
// structure that stores information about point between indices in an array
struct LerpedIndex {
	int i1;
	int i2;
	float lerpValue;
};
// structure that stores information about point between indices in a 2D array
struct LerpedIndex2D {
	int i1;
	int j1;
	int i2;
	int j2;
	float lerpValue;
};
// structure that stores one linked list item (a value, link pair)
template <typename T> struct ListItem {
	ListItem(T _value){ value = _value; }
	T value;
	ListItem<T> *link = nullptr;
};
// class that manages a linked list, storing the first list item
template <typename T> class LinkedList;

// -------------------------------------------------------
// -------------------- Function List --------------------
// -------------------------------------------------------

	// ----- Array Creation -----
// fill an array with linearly spaced values
template <typename T=float> void Linspace(int size, T toFill[], T start, T end);
// fill an array with logarithmically spaces values
template <typename T=float> void Logspace(int size, T toFill[], T start, T end);
// fill an array with values increasing in steps of 1
template <typename T=float> void Ramp(int size, T toFill[], int start=0);
// fill an array with a constant value
template <typename T=float> void Fill(int size, T toFill[], T value);
// fill an array with a constant function
template <typename T=float> void Fill(int size, T toFill[], T (*function)());
// fill an array with a constant function evaluated at a constant value
template <typename T=float, typename U=float> void Fill(int size, T toFill[], U value, T (*function)(U));

	// ----- Array Manipulation Per Element -----
// apply a function per element between two arrays
template <typename T=float> void PerElement(int size, T toFill[], T arrayOne[], T arrayTwo[], T (*function)(T, T));
// apply a function per element between an array and a constant value, in that order
template <typename T=float, typename U=float> void PerElement(int size, T toFill[], T array[], U value, T (*function)(T, U));
// apply a function per element between a constant value and an array, in that order
template <typename U=float, typename T=float> void PerElement(int size, T toFill[], U value, T array[], T (*function)(U, T));
// apply a function per element to an array
template <typename T=float> void PerElement(int size, T toFill[], T array[], T (*function)(T));

	// ----- 2D Array Functions ----
// apply a function between 2 arrays per element to a 2d array
template <typename T=float, typename U=float> void PerElement2D(int size, T toFill[], U xs[], U ys[], T (*function)(U, U));
// find number of sign changes across a 2d array, storing the indices beyond which they occur and the fractions beyond the values at the indices they occur in arrays
template <typename T=float> int FindChangeOfSign2D(int size, T array[], LerpedIndex2D indicesDest[]);
// find the grad field of a 2D array
template <typename T=float> void Grad(int size, Vector2D toFill[], T f[], T wrtX[], T wrtY[]);
// find the div field of a 2D array of vectors
template <typename T=float> void Div(int size, float toFill[], Vector2D f[], T wrtX[], T wrtY[]);
// find number of sign changes acriss a 2d array of 2d vectors, storing the indices beyond which they occur and the fractions beyond the values at the indices they occur in arrays
int Find2DChangeOfSign2D(int size, Vector2D array[], LerpedIndex2D indicesDest[]);

	// ----- Other Array Manipulation & Functions -----
// type cast all of the elements in an array
template <typename T, typename U> void ArrTypeCast(int size, T toFill[], U toCast[]);
// copy an array
template <typename T=float> void ArrCopy(int size, T toFill[], T array[]);
// print an array to the standard output
template <typename T=float> void ArrPrint(int size, T array[]);
// find the largest value in an array and store its index if you want
template <typename T=float> T FindMax(int size, T array[], int *index=nullptr);
// find the smallest value in an array and store its index if you want
template <typename T=float> T FindMin(int size, T array[], int *index=nullptr);
// find the dot product between two arrays
template <typename T=float> T ArrDot(int size, T arr1[], T arr2[]);

	// ----- Advanced Array Manipulation -----
// find number of sign changes along an array, storing the indices beyond which they occur and the fractions beyond the values at the indices they occur in arrays
template <typename T=float> int FindChangeOfSign(int size, T array[], LerpedIndex indicesDest[]);
// store the numerical derivative of 'y' with respect to 'wrt' in 'toFill'
template <typename T=float> void Differentiate(int size, T toFill[], T y[], T wrt[]);
// store the numerical integral of 'y' with respect to 'wrt' in 'toFill', using 'c' as the constant of integration
template <typename T=float> void Integrate(int size, T c, T toFill[], T y[], T wrt[]);

	// ----- Heap Array Functions -----
// insert a value into a heap structured array, note: the size of the array must be larger than 'initialSize', this function increments the heap size
template <typename T=float> void HeapInsert(int initialSize, T val, T heap[], bool (*compareFunction)(T, T)=&LessThan);
// remove the first value from a heap structured array, note: 'initialSize' must be larger than zero, this function decrements the heap size
template <typename T=float> T HeapRemove(int initialSize, T heap[], bool (*compareFunction)(T, T)=&LessThan);
// insert an array one by one into a heap, note: the size of 'heapDest' must be at least 'arraySize' + 'initialHeapSize'
template <typename T=float> void ArrIntoHeap(int arraySize, T array[], T heap[], bool (*compareFunction)(T, T)=&LessThan, int initialHeapSize=0);
// removes each value of a heap (from the top) one by one and puts them into an array in order
template <typename T=float> void HeapIntoArr(int heapSize, T heap[], T array[], bool (*compareFunction)(T, T)=&LessThan);

	// ----- Basic Linear Interpolation Functions -----
// find value at 'fraction' of way from 'start' to 'stop'
template <typename T=float> T Lerp(float fraction, T start, T stop);
// find fraction of way from 'start' to 'stop' that 'value' is
template <typename T=float> float LerpOf(T value, T start, T stop);
// find the mapping of 'x' from the range 'min1' to 'max1' to the range 'min2' to 'max2'
template <typename T=float> T Map(T x, T min1, T max1, T min2, T max2);

	// ----- String Manipulation -----
// reverse a string 'str' of length 'len'
void StrReverse(char* str, int len);
// convert an integer 'x' to string 'res', optionally with minimum 'd' digits
int IntToStr(int x, char* res, int d=0);
// convert a floating-point/double to a string 'res' with 'afterpoint' decimal places and returns its length
int FloatToStr(float x, char* res, int afterpoint);

	// ----- Statistics -----
// sum up the values in an array
template <typename T=float> T Sum(int size, T array[]);
// sum up the squares of the values in an array
template <typename T=float> T SumSquares(int size, T array[]);
// find the gradient of the least squares linear regression line fit to 'y' against 'x'
template <typename T=float> float LinearRegressionGradient(int size, T x[], T y[]);
// find the intercept of the least squares linear regression line fit to 'y' against 'x', optionally passing in the known gradient 'm' of the best fit line
template <typename T=float> T LinearRegressionIntercept(int size, T x[], T y[], float *m=nullptr);
// find the covariance (big S) of two arrays
template <typename T=float> T Covariance(int size, T arr1[], T arr2[]);
// find the Product-Moment Correlation Coefficient between two arrays
template <typename T=float> float PMCC(int size, T arr1[], T arr2[]);

	// ----- Other Functions -----
// initialise the random seed
void Init_RandomSeed();
// make a linked list from a number of values
template <typename T> LinkedList<T> MakeLinkedList(T value);
template <typename T, typename... Args> LinkedList<T> MakeLinkedList(T value, Args... values);

	// ----- 2D Vector Fields -----
// fill a 2D array with a function
template <typename T=float> void Fill2D(int size, T toFill[], T (*function)());
// class that holds a 2D Perlin Noise field
template <int size> class PerlinNoise2D;


// --------------------------------------------------------------
// -------------------- Function Definitions --------------------
// --------------------------------------------------------------

// type cast all of the elements in an array
template <typename T, typename U> void ArrTypeCast(int size, T toFill[], U toCast[]){
	for(int i=0; i<size; i++){
		toFill[i] = (T)toCast[i];
	}
}

// copy an array
template <typename T> void ArrCopy(int size, T toFill[], T array[]){
    for(int i=0; i<size; i++){
        toFill[i] = array[i];
    }
}

// apply a function per element between two arrays
template <typename T> void PerElement(int size, T toFill[], T arrayOne[], T arrayTwo[], T (*function)(T, T)){
    for(int i=0; i<size; i++){
        toFill[i] = function(arrayOne[i], arrayTwo[i]);
    }
}

// apply a function per element between an array and a constant value, in that order
template <typename T, typename U> void PerElement(int size, T toFill[], T array[], U value, T (*function)(T, U)){
    for(int i=0; i<size; i++){
        toFill[i] = function(array[i], value);
    }
}
// apply a function per element between a constant value and an array, in that order
template <typename U, typename T> void PerElement(int size, T toFill[], U value, T array[], T (*function)(U, T)){
    for(int i=0; i<size; i++){
        toFill[i] = function(value, array[i]);
    }
}

// apply a function per element to an array
template <typename T> void PerElement(int size, T toFill[], T array[], T (*function)(T)){
    for(int i=0; i<size; i++){
        toFill[i] = function(array[i]);
    }
}

// fill an array with linearly spaced values
template <typename T> void Linspace(int size, T toFill[], T start, T end) {
    T gap = (end - start)/size;
    toFill[0] = start;
    for(int i=1; i<size; i++){
        toFill[i] = toFill[i-1] + gap;
    }
}

// fill an array with logarithmically spaced values
template <typename T> void Logspace(int size, T toFill[], T start, T end){
	if(start <= 0.0f || end <= 0.0f){ std::cout << "Logspace range must be strictly positive." << std::endl; return; }
	T ratio = pow(end / start, 1 / (T)size);
    toFill[0] = start;
    for(int i=1; i<size; i++){
        toFill[i] = toFill[i-1] * ratio;
    }
}

// fill an array with values increasing in steps of 1
template <typename T> void Ramp(int size, T toFill[], int start){
    for(int i=0; i<size; i++){
        toFill[i] = start+i;
    }
}

// fill an array with a constant value
template <typename T> void Fill(int size, T toFill[], T value){
    for(int i=0; i<size; i++){
        toFill[i] = value;
    }
}

// fill an array with a constant function
template <typename T> void Fill(int size, T toFill[], T (*function)()){
    for(int i=0; i<size; i++){
        toFill[i] = function();
    }
}

// fill an array with a constant function evaluated at a constant value
template <typename T, typename U> void Fill(int size, T toFill[], U value, T (*function)(U)){
    for(int i=0; i<size; i++){
        toFill[i] = function(value);
    }
}

// print an array to the standard output
template <typename T> void ArrPrint(int size, T array[]){
    for(int i=0; i<size; i++){
        std::cout << array[i] << ", ";
    }
    std::cout << "\n";
}

// find the largest value in an array and store its index if you want
template <typename T> T FindMax(int size, T array[], int *index){
	int ind = 0;
	int i = 0;
	float max = array[i];
	for(i=1; i<size; i++){
		if(array[i] > max){
			max = array[i];
			ind = i;
		}
	}
	if(index != nullptr){
		*index = ind;
	}
	return max;
}

// find the smallest value in an array and store its index if you want
template <typename T> T FindMin(int size, T array[], int *index){
	int ind = 0;
	int i = 0;
	float min = array[i];
	for(i=1; i<size; i++){
		if(array[i] < min){
			min = array[i];
			ind = i;
		}
	}
	if(index != nullptr) *index = ind;
	return min;
}

// insert a value into a heap structured array, note: the size of the array must be larger than 'initialSize', this function increments the heap size
template <typename T> void HeapInsert(int initialSize, T val, T heap[], bool (*compareFunction)(T, T)){
	int index = initialSize;
	int parentIndex = (index-1)/2;
	while(index != 0 && compareFunction(val, heap[parentIndex])){
		heap[index] = heap[parentIndex]; // set child value to parent value
		index = parentIndex; // change index to parent index
		parentIndex = (index-1)/2; // calculate new (grand)parent index
	}
	heap[index] = val;
}

// remove the first value from a heap structured array, note: 'initialSize' must be larger than zero, this function decrements the heap size
template <typename T> T HeapRemove(int initialSize, T heap[], bool (*compareFunction)(T, T)){
	T remove = heap[0]; // save top value of heap
	int newSize = initialSize-1; // calculate new heap size
	T val = heap[newSize]; // get bottom heap value
	int index = 0;
	int childIndex;
	while(( (2*index + 2 >= newSize) ? false : compareFunction(heap[2*index + 2], val) ) ||
		  ( (2*index + 1 >= newSize) ? false : compareFunction(heap[2*index + 1], val) )){
		childIndex = 2*index + 1 + (int)( (2*index + 2 >= newSize) ? false : compareFunction(heap[2*index + 2], heap[2*index + 1]) ); // find index of best child
		heap[index] = heap[childIndex]; // set value at index to that of best child
		index = childIndex; // set index to that of best child
	}
	heap[index] = val; // set value at index to the value we have been cascading down the heap
	return remove; // return the value we saved from the top of the heap
}

// insert an array one by one into a heap, note: the size of 'heapDest' must be at least 'arraySize' + 'initialHeapSize'
template <typename T> void ArrIntoHeap(int arraySize, T array[], T heap[], bool (*compareFunction)(T, T), int initialHeapSize){
	for(int i=0; i<arraySize; i++){
		HeapInsert(initialHeapSize + i, array[i], heap, compareFunction);
	}
}

// removes each value of a heap (from the top) one by one and puts them into an array in order
template <typename T> void HeapIntoArr(int heapSize, T heap[], T array[], bool (*compareFunction)(T, T)){
	int originalSize = heapSize;
	for(int i=0; i<originalSize; i++){
		array[i] = HeapRemove(heapSize, heap, compareFunction);
		heapSize--;
	}
}

// find value at 'fraction' of way from 'start' to 'stop'
template <typename T> T Lerp(float fraction, T start, T stop){
	return (T)(start + fraction*(stop - start));
}

// find fraction of way from 'start' to 'stop' that 'value' is
template <typename T> float LerpOf(T value, T start, T stop){
	return (float)((value - start)/(stop - start));
}

// find number of sign changes along array, storing the indices beyond which they occur and the fractions beyond the values at the indices they occur in arrays
template <typename T> int FindChangeOfSign(int size, T array[], LerpedIndex indicesDest[]){
	bool sign = (array[0] >= 0);
	int num = 0;
	for(int i=1; i<size; i++){
		if((array[i] >= 0) != sign){
			sign = (array[i] >= 0);
			indicesDest[num].i1 = i-1;
			indicesDest[num].i2 = i;
			indicesDest[num].lerpValue = LerpOf<T>(0, array[i-1], array[i]);
			num++;
		}
	}
	return num;
}

// store the numerical derivative of 'y' with respect to 'wrt' in 'toFill'
template <typename T> void Differentiate(int size, T toFill[], T y[], T wrt[]){
	T duplAvoid[size];
	ArrCopy<T>(size, duplAvoid, y);
	
	for(int i=1; i<size-1; i++){
		toFill[i] = ((  (duplAvoid[i] - duplAvoid[i-1])/(wrt[i] - wrt[i-1])  ) + (  (duplAvoid[i+1] - duplAvoid[i])/(wrt[i+1] - wrt[i])  ))/2;
	}
	toFill[0] = toFill[1];
	toFill[size-1] = toFill[size-2];
}

// store the numerical integral of 'y' with respect to 'wrt' in 'toFill', using 'c' as the constant of integration
template <typename T> void Integrate(int size, T c, T toFill[], T y[], T wrt[]){
	T duplAvoid[size];
	ArrCopy<T>(size, duplAvoid, y);
	
	toFill[0] = c;
	for(int i=1; i<size; i++){
		toFill[i] = toFill[i-1] + (wrt[i] - wrt[i-1])*(y[i-1] + y[i])/2;
	}
}

// string manipulation stuff
// reverse a string 'str' of length 'len'
void StrReverse(char* str, int len){
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
// convert an integer 'x' to string 'res', optionally with minimum 'd' digits
int IntToStr(int x, char* res, int d){
    int i = 0;
    while (x) {
		res[i] = (x % 10) + '0'; i++;
        x = x / 10;
    }
    // If number of digits required is more, then
    // add 0s at the beginning
	while (i < d) { res[i] = '0'; i++; }
  
    StrReverse(res, i);
    res[i] = '\0';
    return i;
}
// convert a floating-point/double to a string 'res' with 'afterpoint' decimal places and returns its length
int FloatToStr(float x, char* res, int afterpoint){
    // copes with negativity
	int i=0;
	if(x < 0){
		x = -x;
		res[i] = '-';
		i++;
	}
	// Extract integer part
    int ipart = (int)x;
	if(ipart == 0){
		res[i] = '0';
		i++;
	} else {
		// convert integer part to string
		i += IntToStr(ipart, &res[i], 0);
	}
    // Extract floating part
    float fpart = x - (float)ipart;
    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot
		i++;
        // Get the value of fraction part up to given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
  
        i += IntToStr((int)fpart, &res[i], afterpoint);
    }
	return i;
}

// find the dot product between two arrays
template <typename T> T ArrDot(int size, T arr1[], T arr2[]){
	float sum = 0;
	for(int i=0; i<size; i++){
		sum += arr1[i]*arr2[i];
	}
	return sum;
}

// find the mapping of 'x' from the range 'min1' to 'max1' to the range 'min2' to 'max2'
template <typename T> T Map(T x, T min1, T max1, T min2, T max2){
	return min2 + (max2 - min2)*(x - min1)/(max1 - min1);
}

// initialise the random seed
void Init_RandomSeed(){
	srand(0);
}

// statistics stuff:
// sum up the values in an array
template <typename T> T Sum(int size, T array[]){
	T sum = 0;
	for(int i=0; i<size; i++){
		sum += array[i];
	}
	return sum;
}
// sum up the squares of the values in an array
template <typename T> T SumSquares(int size, T array[]){
	T sum = 0;
	for(int i=0; i<size; i++){
		sum += array[i]*array[i];
	}
	return sum;
}
// find the gradient of the least squares linear regression line fit to 'y' against 'x'
template <typename T> float LinearRegressionGradient(int size, T x[], T y[]){
	T SigmaX = Sum(size, x);
	T SigmaX2 = SumSquares(size, x);
	T SigmaY = Sum(size, y);
	T SigmaXY = ArrDot(size, x, y);
	float m = (SigmaXY - SigmaX*SigmaY/size)/(SigmaX2 - SigmaX*SigmaX/size);
	return m;
}
// find the intercept of the least squares linear regression line fit to 'y' against 'x'
template <typename T> T LinearRegressionIntercept(int size, T x[], T y[], float *m){
	T SigmaX = Sum(size, x);
	T SigmaY = Sum(size, y);
	if(m == nullptr){
		*m = LinearRegressionGradient(size, x, y);
	}
	T b = (SigmaY - (*m) * SigmaX)/size;
	return b;
}
// find the covariance (big S) of two arrays
template <typename T> T Covariance(int size, T arr1[], T arr2[]){
	return ArrDot(size, arr1, arr2) - (Sum(size, arr1)*Sum(size, arr2))/size;
}
// find the pmcc(Product-Moment Correlation Coefficient) between two arrays
template <typename T> float PMCC(int size, T arr1[], T arr2[]){
	return Covariance(size, arr1, arr2)/sqrt(Covariance(size, arr1, arr1) * Covariance(size, arr2, arr2));
}


// apply a function between 2 arrays per element to a 2d array
template <typename T, typename U> void PerElement2D(int size, T toFill[], U xs[], U ys[], T (*function)(U, U)) {
	for(int j=0; j<size; j++){
		for(int i=0; i<size; i++){
			toFill[I2D(size, i, j)] = function(xs[i], ys[j]);
		}
	}
}
// find number of sign changes along a 2d array, storing the indices beyond which they occur and optionally the fractions beyond the values at the indices they occur in arrays
template <typename T> int FindChangeOfSign2D(int size, T array[], LerpedIndex2D indicesDest[]){
	bool *signs = (bool*)calloc(size*size, sizeof(bool));
	for(int j=0; j<size; j++){
		for(int i=0; i<size; i++){
			signs[I2D(size, i, j)] = (array[I2D(size, i, j)] >= 0);
		}
	}
	
	int num = 0;
	for(int j=1; j<size; j++){
		for(int i=1; i<size; i++){
			if(signs[I2D(size, i-1, j)] != signs[I2D(size, i, j)]){
				indicesDest[num].i1 = i-1;
				indicesDest[num].j1 = j;
				indicesDest[num].i2 = i;
				indicesDest[num].j2 = j;
				indicesDest[num].lerpValue = LerpOf<T>(0, array[I2D(size, i-1, j)], array[I2D(size, i, j)]);
				num++;
			}
			if(signs[I2D(size, i, j-1)] != signs[I2D(size, i, j)]){
				indicesDest[num].i1 = i;
				indicesDest[num].j1 = j-1;
				indicesDest[num].i2 = i;
				indicesDest[num].j2 = j;
				indicesDest[num].lerpValue = LerpOf<T>(0, array[I2D(size, i, j-1)], array[I2D(size, i, j)]);
				num++;
			}
		}
	}
	free(signs);
	return num;
}

// find number of sign changes across a 2d array of 2d vectors, storing the indices beyond which they occur and optionally the fractions beyond the values at the indices they occur in arrays
int Find2DChangeOfSign2D(int size, Vector2D array[], LerpedIndex2D indicesDest[]){
	bool *signsX = (bool*)calloc(size*size, sizeof(bool));
	bool *signsY = (bool*)calloc(size*size, sizeof(bool));
	for(int j=0; j<size; j++){
		for(int i=0; i<size; i++){
			signsX[I2D(size, i, j)] = ( array[I2D(size, i, j)].x >= 0.0f );
			signsY[I2D(size, i, j)] = ( array[I2D(size, i, j)].y >= 0.0f );
		}
	}
	
	int num = 0;
	for(int j=1; j<size; j++){
		for(int i=1; i<size; i++){
			// hill or bowl
			if(signsX[I2D(size, i-1, j)] != signsX[I2D(size, i, j)] && signsY[I2D(size, i, j-1)] != signsY[I2D(size, i, j)]){
				indicesDest[num].i1 = i-1;
				indicesDest[num].j1 = j-1;
				indicesDest[num].i2 = i;
				indicesDest[num].j2 = j;
				indicesDest[num].lerpValue = ( LerpOf<float>(0.0f, array[I2D(size, i-1, j)].x, array[I2D(size, i, j)].x) + LerpOf<float>(0.0f, array[I2D(size, i, j-1)].y, array[I2D(size, i, j)].y))/2;
				num++;
			}
			
			// saddle
			if(signsX[I2D(size, i, j-1)] != signsX[I2D(size, i, j)] && signsY[I2D(size, i-1, j)] != signsY[I2D(size, i, j)]){
				indicesDest[num].i1 = i-1;
				indicesDest[num].j1 = j-1;
				indicesDest[num].i2 = i;
				indicesDest[num].j2 = j;
				indicesDest[num].lerpValue = ( LerpOf<float>(0.0f, array[I2D(size, i, j-1)].x, array[I2D(size, i, j)].x) + LerpOf<float>(0.0f, array[I2D(size, i-1, j)].y, array[I2D(size, i, j)].y))/2;
				num++;
			}
		}
	}
	free(signsX);
	free(signsY);
	return num;
}

// find the grad field of a 2D array
template <typename T> void Grad(int size, Vector2D toFill[], T f[], T wrtX[], T wrtY[]){
	for(int j=1; j<size-1; j++){
		// centre square
		for(int i=1; i<size-1; i++){
			toFill[I2D(size, i, j)].x = ( (f[I2D(size, i, j)] - f[I2D(size, i-1, j)])/(wrtX[i] - wrtX[i-1]) + (f[I2D(size, i+1, j)] - f[I2D(size, i, j)])/(wrtX[i+1] - wrtX[i]) )/2;
			toFill[I2D(size, i, j)].y = ( (f[I2D(size, i, j)] - f[I2D(size, i, j-1)])/(wrtY[j] - wrtY[j-1]) + (f[I2D(size, i, j+1)] - f[I2D(size, i, j)])/(wrtY[j+1] - wrtY[j]) )/2;
		}
		// centres of sides
		toFill[I2D(size, 0, j)].x = toFill[I2D(size, 1, j)].x;
		toFill[I2D(size, 0, j)].y = toFill[I2D(size, 1, j)].y;
		toFill[I2D(size, size-1, j)].x = toFill[I2D(size, size-2, j)].x;
		toFill[I2D(size, size-1, j)].y = toFill[I2D(size, size-2, j)].y;
	}
	// centres of top and bottom
	for(int i=1; i<size-1; i++){
		toFill[I2D(size, i, 0)].x = toFill[I2D(size, i, 1)].x;
		toFill[I2D(size, i, 0)].y = toFill[I2D(size, i, 1)].y;
		toFill[I2D(size, i, size-1)].x = toFill[I2D(size, i, size-2)].x;
		toFill[I2D(size, i, size-1)].y = toFill[I2D(size, i, size-2)].y;
	}
	// corners
	toFill[I2D(size, 0, 0)].x = (toFill[I2D(size, 1, 0)].x+toFill[I2D(size, 0, 1)].x)/2;
	toFill[I2D(size, size-1, 0)].x = (toFill[I2D(size, size-2, 0)].x+toFill[I2D(size, size-1, 1)].x)/2;
	toFill[I2D(size, 0, size-1)].x = (toFill[I2D(size, 0, size-2)].x+toFill[I2D(size, 1, size-1)].x)/2;
	toFill[I2D(size, size-1, size-1)].x = (toFill[I2D(size, size-2, size-1)].x+toFill[I2D(size, size-1, size-2)].x)/2;
}

// find the div field of a 2D array
template <typename T> void Div(int size, float toFill[], Vector2D f[], T wrtX[], T wrtY[]){
	for(int j=1; j<size-1; j++){
		// centre square
		for(int i=1; i<size-1; i++){
			toFill[I2D(size, i, j)] = ( (f[I2D(size, i, j)].x - f[I2D(size, i-1, j)].x)/(wrtX[i] - wrtX[i-1]) + (f[I2D(size, i+1, j)].x - f[I2D(size, i, j)].x)/(wrtX[i+1] - wrtX[i]) )/2 + ( (f[I2D(size, i, j)].y - f[I2D(size, i, j-1)].y)/(wrtY[j] - wrtY[j-1]) + (f[I2D(size, i, j+1)].y - f[I2D(size, i, j)].y)/(wrtY[j+1] - wrtY[j]) )/2;
		}
		// centres of sides
		toFill[I2D(size, 0, j)] = toFill[I2D(size, 1, j)];
		toFill[I2D(size, size-1, j)] = toFill[I2D(size, size-2, j)];
	}
	// centres of top and bottom
	for(int i=1; i<size-1; i++){
		toFill[I2D(size, i, 0)] = toFill[I2D(size, i, 1)];
		toFill[I2D(size, i, size-1)] = toFill[I2D(size, i, size-2)];
	}
	// corners
	toFill[I2D(size, 0, 0)] = (toFill[I2D(size, 1, 0)]+toFill[I2D(size, 0, 1)])/2;
	toFill[I2D(size, size-1, 0)] = (toFill[I2D(size, size-2, 0)]+toFill[I2D(size, size-1, 1)])/2;
	toFill[I2D(size, 0, size-1)] = (toFill[I2D(size, 0, size-2)]+toFill[I2D(size, 1, size-1)])/2;
	toFill[I2D(size, size-1, size-1)] = (toFill[I2D(size, size-2, size-1)]+toFill[I2D(size, size-1, size-2)])/2;
}


// ---------- 2D Vector Fields ----------
template <typename T> void Fill2D(int size, T toFill[], T (*function)()){
	for(int j=0; j<size; j++){
		for(int i=0; i<size; i++){
			toFill[I2D(size, i, j)] = function();
		}
	}
}

template <int size> class PerlinNoise2D {
public:
	PerlinNoise2D() {
		Fill2D<Vector2D>(size, field, &RandomUnitVector2D);
	}
	
	float GetVal(float x, float y){
		// Determine grid cell coordinates
		int x0 = (int)x;
		int x1 = x0 + 1;
		int y0 = (int)y;
		int y1 = y0 + 1;
		// Determine interpolation weights
		// Could also use higher order polynomial/s-curve here
		float sx = x - (float)x0;
		float sy = y - (float)y0;
		// Interpolate between grid point gradients
		float n0, n1, ix0, ix1, value;

		n0 = DotGridGradient(x0, y0, x, y);
		n1 = DotGridGradient(x1, y0, x, y);
		ix0 = Lerp(sx, n0, n1);

		n0 = DotGridGradient(x0, y1, x, y);
		n1 = DotGridGradient(x1, y1, x, y);
		ix1 = Lerp(sx, n0, n1);

		value = Lerp(sy, ix0, ix1);
		return value;
	}
	
private:
	// Computes the dot product of the distance and gradient vectors.
	float DotGridGradient(int ix, int iy, float x, float y) {
		// Compute the distance vector
		float dx = x - (float)ix;
		float dy = y - (float)iy;
		// Compute the dot-product
		return (dx*field[I2D(size, ix, iy)].x + dy*field[I2D(size, ix, iy)].y);
	}
	
	Vector2D field[size*size];
};


// ----- Classes -----

// class that manages a linked list, storing the first list item
template <typename T> class LinkedList {
public:
	LinkedList() {}
	LinkedList(T value){ firstItem = new ListItem<T>(value); }
	
	T &operator[](size_t index){
		ListItem<T> *item = firstItem;
		if(item == nullptr){
			std::cout << "ERROR: List index out of bounds. " << std::endl;
			return nullValue;
		}
		for(int i=0; i<index; i++){
			item = item->link;
			if(item == nullptr){
				std::cout << "ERROR: List index out of bounds. " << std::endl;
				return nullValue;
			}
		}
		return item->value;
	}
	
	void Append(T value){
		if(firstItem == nullptr){
			firstItem = new ListItem<T>(value);
			return;
		}
		ListItem<T> *item = firstItem;
		while(item->link != nullptr) item = item->link;
		item->link = new ListItem<T>(value);
	}
	
	void Append(LinkedList<T> list){
		if(firstItem == nullptr){
			firstItem = list.firstItem;
			return;
		}
		ListItem<T> *item = firstItem;
		while(item->link != nullptr) item = item->link;
		item->link = list.firstItem;
	}
	
	void Prepend(T value){
		ListItem<T> *temp = firstItem;
		firstItem = new ListItem<T>(value);
		firstItem->link = temp;
	}
	
	T Remove(unsigned int index){
		ListItem<T> *prevItem = nullptr;
		ListItem<T> *item = firstItem;
		if(item == nullptr){
			std::cout << "ERROR: List index out of bounds. " << std::endl;
			return nullValue;
		}
		for(int i=0; i<index; i++){
			prevItem = item;
			item = item->link;
			if(item == nullptr){
				std::cout << "ERROR: List index out of bounds. " << std::endl;
				return nullValue;
			}
		}
		if(prevItem != nullptr) prevItem->link = item->link;
		T ret = item->value;
		delete item;
		return ret;
	}
	
	T Pop(){
		ListItem<T> *prevItem = nullptr;
		ListItem<T> *item = firstItem;
		if(item == nullptr){
			std::cout << "ERROR: Cannot pop, list empty." << std::endl;
			return nullValue;
		}
		while(item->link != nullptr){
			prevItem = item;
			item = item->link;
		}
		if(prevItem != nullptr) prevItem->link = nullptr;
		T ret = item->value;
		delete item;
		return ret;
	}
	
	void Destroy(){
		ListItem<T> *item = firstItem;
		ListItem<T> *next;
		while(item->link != nullptr){
			next = item->link;
			delete item;
			item = next;
		}
		delete item;
		firstItem = nullptr;
	}
	
	void Print(){
		ListItem<T> *item = firstItem;
		while(item != nullptr){
			std::cout << item->value << " ";
			item = item->link;
		}
		std::cout << std::endl;
	}
	
	unsigned int Size(){
		unsigned int ret = 0;
		ListItem<T> *item = firstItem;
		while(item != nullptr){
			item = item->link;
			ret++;
		}
		return ret;
	}
	
	LinkedList<T> Reversed(){
		LinkedList<T> ret = LinkedList<T>();
		ListItem<T> *item = firstItem;
		while(item != nullptr){
			ret.Prepend(item->value);
			item = item->link;
		}
		return ret;
	}
	
	T Loop(){
		loopItem = firstItem;
		if(loopItem == nullptr) return nullValue;
		return loopItem->value;
	}
	
	T Next(){
		loopItem = loopItem->link;
		if(loopItem == nullptr) return nullValue;
		return loopItem->value;
	}
	
	bool End(){ return (loopItem != nullptr); }
	
	// access not generally needed
	ListItem<T> *firstItem = nullptr;
	ListItem<T> *loopItem;
	
	T nullValue;
};

template <typename T> LinkedList<T> MakeLinkedList(T value){
	return LinkedList<T>(value);
}
template <typename T, typename... Args> LinkedList<T> MakeLinkedList(T value, Args... values){
	LinkedList<T> ret = MakeLinkedList(values...);
	ret.Prepend(value);
	return ret;
}


#endif /* Header_h */
