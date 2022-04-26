#include "FancyArray.hpp"

bool Prime(int data){
	if(data < 2) return false;
	for(int i=2; i*i<=data; i++) if(data % i == 0) return false;
	return true;
}

int main(int argc, const char * argv[]) {
	
	// branch size is 40 bytes regardless of type T stored, so for infty copies, chunk size should be close to K*sizeof(T) = 40/ln2 bytes which is about K = 57.71/sizeof(T) data values
	// for T = int, this is about K = 14
	
	/*
	const uint8_t K = 16; // data values per chunk
	const int N = 100;
	static int data[N];
	for(int i=0; i<N; i++) data[i] = i;
	
	FancyArray<int> fancyArray = FancyArray<int>(N, K, data); // integers from 0 to 99 inclusive
	std::cout << fancyArray[69] << "\n"; // value at index 69 = 69
	
	FancyArray<int> sliced = fancyArray.Slice(26, 82); // slice from indices 26 to 81 inclusive
	std::cout << sliced[14] << "\n"; // value at index 26 + 14 = 40
	
	FancyArray<int> selected = fancyArray.Select(&Prime); // primes in the array
	std::cout << selected[9] << "\n"; // 10th prime in the original array = 29
	
	FancyArray<int> combined = Concat(sliced, selected);
	std::cout << combined[64] << "\n"; // there are 55 elements in the array slices, so this is the 9th element in the second array: 23
	
	int array[56];
	sliced.Contiguous(array);
	
	for(int i=0; i<56; i++) std::cout << array[i] << ", ";
	std::cout << "\n";
	*/
	
	
	
	return 0;
}
