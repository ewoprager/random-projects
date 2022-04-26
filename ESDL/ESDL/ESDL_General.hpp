#ifndef ESDL_General_hpp
#define ESDL_General_hpp

#include <SDL2/SDL.h>
#include <iostream>

/* A simple function that will read a file into an allocated char pointer buffer */
char* filetobuf(const char *const &file);


template <class C, typename returnType, typename... parameterTypes> struct MemberFunction {
	C *owner;
	returnType (C::*function)(parameterTypes...);
	
	returnType Call(parameterTypes... args){ return (owner->*function)(args...); }
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

#endif /* ESDL_General_hpp */
