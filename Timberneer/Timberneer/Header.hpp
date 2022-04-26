#ifndef Header_hpp
#define Header_hpp

#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>

#include "lodepng.h"

#include "mat4x4.hpp"
#include "vec4.hpp"
#include "vec2.hpp"

#define MOUSE_SENSITIVITY_X 0.002f
#define MOUSE_SENSITIVITY_Y 0.002f

class EventHandler;

class Updated;
class Rendered;
class InstancedParent;
class RenderedInstanced;
class Interactable;

struct ObjectData;

class Player;
class Ground;
class Wheelbarrow;

double AngleDifference(const double& angle1, const double& angle2);
float RandomAngle();

GLchar *filetobuf(const char *file);
bool CompileShader(GLuint shader, const char *sourceFile);
bool LinkProgram(GLuint program);

GLuint LoadTexture(const char *file);
GLuint GetTextureIdFromMtl(const char *usemtl);
GLuint MakeTextTexture(const char *text);

float IntegerPow(const float& x, const unsigned int& p);
bool StringEquality(const char *string1, const char *string2);

ObjectData ReadProcessedOBJFile(const char *file);
//ObjectData ReadOBJFile(const char *file);
GLuint GetDefaultTextureId();
GLuint GetTextureIdFromMtl(const char *usemtl);

ObjectData MakePlane(unsigned int columns, unsigned int rows, float columnWidth, float rowHeight, const char *usemtl);

enum Align {al_raggedPositive = 0, al_centered = 1, al_raggedNegative = 2};

bool EventEquality(SDL_Event event1, SDL_Event event2);
template <class C, typename ret, typename... Types> struct MemberFunction {
	MemberFunction(){}
	MemberFunction(C *_owner, ret (C::*_function)(Types...)){
		owner = _owner;
		function = _function;
	}
	C *owner;
	ret (C::*function)(Types...);
	ret Call(Types... args){ return (owner->*function)(args...); }
};

template <int size, typename T> class ObjectArray {
public:
	ObjectArray(){
		for(int i=0; i<size; i++) objectsActive[i] = false;
	}
	~ObjectArray(){}
	
	int Add(T object){
		int newId = NewId();
		if(newId == -1){
			std::cout << "ERROR: No more room in array." << std::endl;
			return 0;
		}
		objects[newId] = object;
		objectsActive[newId] = true;
		return newId;
	}
	bool Remove(int& id){
		if(objectsActive[id]){
			objectsActive[id] = false;
			id = -1;
			return true;
		}
		id = -1;
		return false;
	}
	
	T objects[size];
	bool objectsActive[size];
	
private:
	int NewId(){
		for(int i=0; i<size; i++){
			if(objectsActive[i]) continue;
			return i;
		}
		return -1;
	}
};

template <typename T> struct SmartArrayStruct {
	T element;
	bool active;
};
template <int size, typename T> class SmartArray {
public:
	SmartArray(){
		for(int i=0; i<size; i++) elements[i].active = false;
	}
	
	int Add(const T& element){
		if(N >= size){
			std::cout << "ERROR: No more room in array." << std::endl;
			return -1;
		}
		int id = NewId();
		if(id == -1) return -1;
		elements[id] = {element, true};
		indices[N++] = id;
		return id;
	}
	bool Remove(int& id){
		if(elements[id].active){
			elements[id].active = false;
			for(int i=0; i<N; i++){
				if(indices[i] == id){
					for(int j=i; j<N-1; j++) indices[j] = indices[j+1];
					N--;
					id = -1;
					return true;
				}
			}
		}
		id = -1;
		return false;
	}
	
	T& operator[](const int& index){
		return elements[indices[index]].element;
	}
	T operator()(const int& id) const {
		if(id < 0) return (T)0;
		if(id > size) return (T)0;
		if(!elements[id].active) return (T)0;
		return elements[id].element;
	}
	
	int GetN(){ return N; }
	
private:
	int N = 0;
	SmartArrayStruct<T> elements[size];
	int indices[size];
	
	int NewId() const {
		if(N >= size){
			std::cout << "ERROR: No ids left." << std::endl;
			return -1;
		}
		for(int i=0; i<size; i++){
			if(elements[i].active) continue;
			return i;
		}
		std::cout << "ERROR: No ids left, and N is not equal to size." << std::endl;
		return -1;
	}
};

struct FileObjectDivisionData {
	int32_t start;
	size_t count;
	uint8_t usemtl[512];
};
struct ObjectDivisionData {
	GLint start;
	GLsizei count;
	GLuint texture;
};
struct ObjectData {
	unsigned int vertices_n;
	GLfloat *vertices;
	
	ObjectDivisionData *divisionData;
	unsigned int divisionsN;
};

template <unsigned int d, typename T> struct obj_array_struct {
	T array[d];
	T &operator[](unsigned int index) {
		return array[index];
	}
	obj_array_struct<d, T> &operator=(obj_array_struct<d, T> other){
		for(unsigned int i=0; i<d; i++) array[i] = other[i];
		return *this;
	}
};
struct obj_v : obj_array_struct<3, GLfloat> {};
struct obj_vt : obj_array_struct<2, GLfloat> {};
struct obj_vn : obj_array_struct<3, GLfloat> {};
vec3 OBJVToVector(obj_v OBJvertex);
obj_v VectorToOBJV(vec3 vec);
obj_vn VectorToOBJVN(vec3 vec);

template <class C> class SwitchedBool {
public:
	SwitchedBool(C *owner, void (C::*switchFunction)(bool), bool initialValue=false){
		SwitchFunction = MemberFunction<C, void, bool>(owner, switchFunction);
		value = initialValue;
	}
	
	bool Get(){ return value; }
	void Set(const bool& val){
		if(value == val) return;
		Switch();
	}
	void Switch(){
		value = !value;
		SwitchFunction.Call(value);
	}
private:
	MemberFunction<C, void, bool> SwitchFunction;
	bool value;
};


#endif /* Header_hpp */
