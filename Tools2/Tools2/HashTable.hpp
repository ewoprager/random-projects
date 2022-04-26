#ifndef HashTable_h
#define HashTable_h

#include "Header.hpp"

template <typename T> struct HashElement {
	T key;
	bool occupied = false;
};

// Hash table using open addressing for collision resolution
template <typename T, unsigned int size> class HashTable {
public:
	HashTable(unsigned int (*_hashFunction)(T), bool (*_equalityFunction)(T a, T b)){
		hashFunction = _hashFunction;
		equalityFunction = _equalityFunction;
	}
	
	T &operator[](size_t index){
		for(int i=0; i<size; i++){
			if(!array[i].occupied) continue;
			if(index){
				index--;
			} else {
				return array[i].key;
			}
		}
		std::cout << "ERROR: Hash index out of bounds." << std::endl;
		return nullValue;
	}
	
	// Add a new key into the table. Return true on success or if key already exists. Returns false if there is no more room in the table.
	bool Add(T key){
		unsigned int value = hashFunction(key) % size;
		unsigned int firstValue = value;
		while(array[value].occupied){
			if(equalityFunction(key, array[value].key)) return true;
			value = (value + 1) % size;
			if(value == firstValue) return false;
		}
		array[value] = {key, true};
		return true;
	}
	
	// Remove a key from the table. Returns true on success. Returns false if the key did not exist in the table already.
	bool Remove(T key){
		unsigned int value = hashFunction(key) % size;
		unsigned int firstValue = value;
		while(array[value].occupied){
			if(equalityFunction(key, array[value].key)){
				array[value].occupied = false;
				// managing for items that need to be moved back:
				unsigned int nextValue = (value + 1) % size;
				while(array[nextValue].occupied){
					unsigned int i;
					for(i = hashFunction(array[nextValue].key) % size; i != nextValue; i = (i + 1) % size) if(!array[i].occupied) break;
					if(i != nextValue){
						array[i] = array[nextValue];
						array[nextValue].occupied = false;
					}
					value = nextValue;
					nextValue = (value + 1) % size;
				}
				return true;
			}
			value = (value + 1) % size;
			if(value == firstValue) return false;
		}
		return false;
	}
	
	// Find if a key is present in the table.
	bool Find(T key){
		unsigned int value = hashFunction(key) % size;
		unsigned int firstValue = value;
		while(array[value].occupied){
			if(equalityFunction(key, array[value].key)) return true;
			value = (value + 1) % size;
			if(value == firstValue) return false;
		}
		return false;
	}
	
	// Print the table
	void Print(){
		for(int i=0; i<size; i++){
			if(!array[i].occupied) continue;
			std::cout << array[i].key << std::endl;
		}
	}
	
	unsigned int ItemNumber(){
		unsigned int num = 0;
		for(unsigned int i=0; i<size; i++) if(array[i].occupied) num++;
		return num;
	}
	
private:
	unsigned int (*hashFunction)(T);
	bool (*equalityFunction)(T a, T b);
	HashElement<T> array[size];
	
	T nullValue;
};

#endif /* HashTable_h */
