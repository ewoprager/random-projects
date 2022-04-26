#ifndef FancyArray_hpp
#define FancyArray_hpp

#include <iostream>


// -----								-----
// ----- Structure declarations			-----
// -----								-----

// The class for the user to interact with - holds all functionality
template <typename T> class FancyArray;

template <typename T> class FancyBranch; // for internal use
template <typename T> struct FancyElement; // for internal use
template <typename T> class FancyBranch; // for internal use



// -----								-----
// ----- User function declarations		-----
// -----								-----

// Returns a pointer to a newly created array on the heap which contains a copy of the given slice of the given array
template <typename T> T *ArraySlice(T array[], const unsigned int &start, const unsigned int &stop);

// Returns a pointer to a newly created array on the heap which contains a copy of each element of the given array that returns true when passed to the given function
template <typename T> T *ArraySelect(T array[], bool (*const &function)(T), const unsigned int &arrayLength, unsigned int *newLength);

// Returns a fancy array which represents the two given fancy arrays concatenated
template <typename T> FancyArray<T> Concat(const FancyArray<T> &array1, const FancyArray<T> &array2);



// -----								-----
// ----- User function definitions		-----
// -----								-----

template <typename T> T *ArraySlice(T array[], const unsigned int &start, const unsigned int &stop){
	// checks
	T *ret = new T[stop - start];
	memcpy(ret, array + start, (stop - start) * sizeof(T));
	return ret;
}
template <typename T> T *ArraySelect(T array[], bool (*const &function)(T), const unsigned int &arrayLength, unsigned int *newLength){
	// checks
	T buf[arrayLength];
	*newLength = 0;
	for(int i=0; i<arrayLength; i++) if((*function)(array[i])) buf[(*newLength)++] = array[i];
	if(!(*newLength)) return nullptr;
	if(*newLength == arrayLength) return array;
	T *ret = new T[*newLength];
	memcpy(ret, buf, (*newLength) * sizeof(T));
	return ret;
}



// -----								-----
// ----- Stricture definitions			-----
// -----								-----

template <typename T> struct FancyElement {
	uint8_t leaf;
	union {
		FancyBranch<T> *branch;
		T *data;
	};
};
template <typename T> class FancyBranch {
private:
	FancyBranch(const unsigned int &_leftLength, const uint8_t &_K, const FancyElement<T> &_left, const FancyElement<T> &_right) : leftLength(_leftLength), K(_K), left(_left), right(_right) {}
	FancyBranch(const unsigned int &length, const uint8_t &_K, T *const &data) : K(_K) {
		const unsigned int N = length / _K + (bool)(length % _K);
		if(N < 2){
			std::cout << "Error: Fancy array initilised with fewer than 2 chunks.\n";
			return;
		}
		
		const unsigned int rightN = N / 2;
		const unsigned int leftN = N - rightN;
		leftLength = _K*leftN;
		const unsigned int rightLength = length - leftLength;
		
		if(leftN > 1){
			//std::cout << "Creating a left branch of length " << leftLength << ": \n";
			left.branch = new FancyBranch<T>(leftLength, _K, data);
			left.leaf = 0;
		} else {
			//std::cout << "Creating a left leaf: ";
			//for(int i=0; i<leftLength; i++) std::cout << *(data + i) << ", ";
			//std::cout << "\n";
			left.data = new T[_K];
			memcpy(left.data, data, _K * sizeof(T));
			left.leaf = _K;
		}
		if(rightN > 1){
			//std::cout << "Creating a right branch of length " << rightLength << ": \n";
			right.branch = new FancyBranch<T>(rightLength, _K, data + leftLength);
			right.leaf = 0;
		} else {
			//std::cout << "Creating a right leaf: ";
			//for(int i=0; i<rightLength; i++) std::cout << *(data + leftLength + i) << ", ";
			//std::cout << "\n";
			right.data = new T[rightLength];
			memcpy(right.data, data + leftLength, rightLength * sizeof(T));
			right.leaf = rightLength;
		}
	}
	
	T operator[](const unsigned int &index) const {
		if(index < leftLength){
			if(left.leaf){
				return left.data[index];
			} else {
				return (*left.branch)[index];
			}
		}
		if(right.leaf){
			return right.data[index - leftLength];
		} else {
			return (*right.branch)[index - leftLength];
		}
	}
	
	void Contiguous(T * &array) const {
		//std::cout << "Copying left ";
		if(left.leaf){
			//std::cout << "leaf: ";
			//for(int i=0; i<left.leaf; i++) std::cout << left.data[i] << ", ";
			//std::cout << "\n";
			memcpy(array, left.data, left.leaf * sizeof(T));
			array += left.leaf;
		} else {
			//std::cout << "branch:\n";
			left.branch->Contiguous(array);
		}
		//std::cout << "Copying right ";
		if(right.leaf){
			//std::cout << "leaf: ";
			//for(int i=0; i<right.leaf; i++) std::cout << right.data[i] << ", ";
			//std::cout << "\n";
			memcpy(array, right.data, right.leaf * sizeof(T));
			array += right.leaf;
		} else {
			//std::cout << "branch:\n";
			right.branch->Contiguous(array);
		}
	}
	
	FancyElement<T> Slice(const unsigned int &start, const unsigned int &stop) const {
		if(start < leftLength){
			if(stop-1 < leftLength){
				//std::cout << "All in the left: ";
				if(left.leaf){
					//std::cout << "leaf: " << (*left.data) << "\n";
					if(start > 0 || stop < leftLength){
						return (FancyElement<T>){(uint8_t)(stop - start), .data=ArraySlice<T>(left.data, start, stop)};
					} else {
						return left;
					}
				} else {
					//std::cout << "branch: \n";
					return left.branch->Slice(start, stop);
				}
			} else {
				//std::cout << "Some in the left: ";
				FancyElement<T> newLeft, newRight;
				if(left.leaf){
					//std::cout << "leaf: " << (*left.data);
					if(start > 0){
						newLeft = (FancyElement<T>){(uint8_t)(leftLength - start), .data=ArraySlice<T>(left.data, start, leftLength)};
					} else {
						newLeft = left;
					}
				} else {
					//std::cout << "branch: \n";
					newLeft = left.branch->Slice(start, leftLength);
				}
				//std::cout << ", and some in the right: ";
				if(right.leaf){
					//std::cout << "leaf: " << (*right.data) << "\n";
					if(stop - leftLength < K){
						newRight = (FancyElement<T>){(uint8_t)(stop - leftLength), .data=ArraySlice<T>(right.data, 0, stop - leftLength)};
					} else {
						newRight = right;
					}
				} else {
					//std::cout << "branch: \n";
					newRight = right.branch->Slice(0, stop - leftLength);
				}
				return (FancyElement<T>){0, .branch=new FancyBranch<T>(leftLength - start, K, newLeft, newRight)};
			}
		} else {
			//std::cout << "All in the right: ";
			if(right.leaf) {
				//std::cout << "leaf: " << (*right.data) << "\n";
				if(start > leftLength || stop - leftLength < K){
					return (FancyElement<T>){(uint8_t)(stop - start), .data=ArraySlice<T>(right.data, start - leftLength, stop - leftLength)};
				} else {
					return right;
				}
			} else {
				//std::cout << "branch: \n";
				return right.branch->Slice(start - leftLength, stop - leftLength);
			}
		}
	}
	
	FancyElement<T> Select(bool (*const &function)(T), unsigned int *myLength) const {
		FancyElement<T> newLeft, newRight;
		unsigned int newLeftLength, newRightLength;
		if(left.leaf){
			//std::cout << "Selecting from left leaf: ";
			T *const newLeftData = ArraySelect<T>(left.data, function, (unsigned int)left.leaf, &newLeftLength);
			newLeft = (FancyElement<T>){(uint8_t)newLeftLength, .data=newLeftData};
			//for(int i=0; i<newLeftLength; i++) std::cout << *(newLeftData + i) << ", ";
			//std::cout << "\n";
		} else {
			//std::cout << "Selecting from left branch:\n";
			newLeft = left.branch->Select(function, &newLeftLength);
		}
		if(right.leaf){
			//std::cout << "Selecting from right leaf: ";
			T *const newRightData = ArraySelect<T>(right.data, function, (unsigned int)right.leaf, &newRightLength);
			newRight = (FancyElement<T>){(uint8_t)newRightLength, .data=newRightData};
			//for(int i=0; i<newRightLength; i++) std::cout << *(newRightData + i) << ", ";
			//std::cout << "\n";
		} else {
			//std::cout << "Selecting from right branch:\n";
			newRight = right.branch->Select(function, &newRightLength);
		}
		*myLength = newLeftLength + newRightLength;
		if(!newLeft.data) return newRight;
		if(!newRight.data) return newLeft;
		return (FancyElement<T>){0, .branch=new FancyBranch<T>(newLeftLength, K, newLeft, newRight)};
	}
	
	uint8_t K;
	unsigned int leftLength;
	FancyElement<T> left, right;
	
	friend class FancyArray<T>;
};

template <typename T> class FancyArray : public FancyElement<T> {
public:
	FancyArray() {}
	FancyArray(const long &_length, const uint8_t &_K, T *const &data) : K(_K) {
		if(_length < 0){
			std::cout << "Error: Invalid fancy array length given.\n";
			return;
		}
		
		length = (unsigned int)_length;
		FancyElement<T>::leaf = length <= _K ? (uint8_t)length : 0;
		if(FancyElement<T>::leaf){
			FancyElement<T>::data = new T[length];
			memcpy(FancyElement<T>::data, data, length * sizeof(T));
		} else {
			FancyElement<T>::branch = new FancyBranch<T>(length, _K, data);
		}
	}
	
	T operator[](const long &index) const {
		if(index < 0 || index >= length){
			std::cout << "Error: Fancy array index out of bounds.\n";
			return 0;
		}
		
		if(FancyElement<T>::leaf){
			return FancyElement<T>::data[index];
		} else {
			return (*FancyElement<T>::branch)[(unsigned int)index];
		}
	}
	
	void Contiguous(T *const &array) const {
		if(FancyElement<T>::leaf){
			memcpy(array, FancyElement<T>::data, FancyElement<T>::leaf * sizeof(T));
		} else {
			T *pointer = array;
			FancyElement<T>::branch->Contiguous(pointer);
		}
	}
	
	FancyArray<T> Slice(const long &start, const long &stop) const {
		if(stop <= start || start < 0 || stop > length){
			std::cout << "Error: Fancy array slices indices out of bounds.\n";
			return FancyArray<T>();
		}
		
		if(FancyElement<T>::leaf){
			return FancyArray<T>(stop - start, K, ArraySlice<T>(FancyElement<T>::data, (unsigned int)start, (unsigned int)stop));
		} else {
			return FancyArray<T>((unsigned int)(stop - start), K, FancyElement<T>::branch->Slice((unsigned int)start, (unsigned int)stop));
		}
	}
	
	FancyArray<T> Select(bool (*const &function)(T)) const {
		if(FancyElement<T>::leaf){
			unsigned int newLength;
			T *const newData = ArraySelect<T>(FancyElement<T>::data, function, (unsigned int)FancyElement<T>::leaf, &newLength);
			return FancyArray<T>((long)newLength, K, (FancyElement<T>){(uint8_t)newLength, .data=newData});
		} else {
			unsigned int retLength;
			const FancyElement<T> ret = FancyElement<T>::branch->Select(function, &retLength);
			return FancyArray<T>((long)retLength, K, ret);
		}
	}
	
	friend FancyArray<T> Concat(const FancyArray<T> &array1, const FancyArray<T> &array2){
		const uint8_t newK = array1.K >= array2.K ? array1.K : array2.K;
		return FancyArray<T>(array1.length + array2.length, newK, (FancyElement<T>){0, .branch=new FancyBranch<T>(array1.length, newK, (FancyElement<T>)array1, (FancyElement<T>)array2)});
	}
	
	uint8_t GetK() const { return K; }
	unsigned int Length() const { return length; }
	
private:
	FancyArray(const long &_length, const uint8_t &_K, FancyElement<T> parent) : K(_K), FancyElement<T>(parent){
		if(_length < 0){
			std::cout << "Error: Invalid fancy array length given.\n";
			return;
		}
		
		length = (unsigned int)_length;
	}
	
	uint8_t K;
	unsigned int length;
};

#endif /* FancyArray_hpp */
