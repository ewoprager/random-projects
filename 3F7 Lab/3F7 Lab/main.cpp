#include <iostream>
#include <math.h>
#include <fstream>

// struct for use in sorting algorithm to store data alongside the sorted-by value
template <typename T> struct SortStruct {
	float sortBy;
	T data;
};
// for quicksort - not my own work; adapted from https://www.geeksforgeeks.org/quick-sort/
template <typename T> int PartitionArray(SortStruct<T> array[], const int& startIndex, const int& endIndex){
	float pivot = array[endIndex].sortBy;
	int i = (startIndex - 1);

	for(int j=startIndex; j <= endIndex-1; j++){
		if(array[j].sortBy > pivot){
			i++;
			SortStruct<T> temp = array[j];
			array[j] = array[i];
			array[i] = temp;
		}
	}
	i++;
	SortStruct<T> temp = array[i];
	array[i] = array[endIndex];
	array[endIndex] = temp;
	return i;
}
// quicksort - not my own work; adapted from https://www.geeksforgeeks.org/quick-sort/
template <typename T> void Quicksort(SortStruct<T> array[], const int& startIndex, const int& endIndex){
	if(startIndex >= endIndex) return;
	int pi = PartitionArray<T>(array, startIndex, endIndex);
	Quicksort(array, startIndex, pi - 1);
	Quicksort(array, pi + 1, endIndex);
}

// for-fun converter from floats between 0.0 and 1.0f to binary representations for use in ShannonFano
uint32_t FloatToCode(const float *fltPtr){
	//if(*fltPtr >= 1.0f) return 0x00000000;
	uint32_t csInt = *(uint32_t *)fltPtr;
	if(!csInt) return 0x00000000;
	unsigned short oneLessNegativeExponent = 126 - (short)(uint8_t)(csInt >> 23);
	return ((0x80000000 | (csInt << 8)) >> oneLessNegativeExponent);
}

// struct to hold code information
struct CodeWord {
	char symbol;
	uint32_t codeWord;
	short wordLength;
};

// takes pmf and symbols and outputs array of code word structs
template <int N> void ShannonFano(const float pArray[N], const char symbols[N], CodeWord codeWords[N]){
	for(int i=0; i<N; i++) if(pArray[i] <= 0.0f) codeWords[i] = {symbols[i], 0, 0};
	// sorting in descending order
	SortStruct<int> sortArray[N];
	for(int i=0; i<N; i++) sortArray[i] = {pArray[i], i};
	Quicksort(sortArray, 0, N-1);
	
	float cumulativeSum = 0.0f;
	float *csPtr = &cumulativeSum; // just for passing into FloatToCode
	for(int i=0; i<N; i++){
		// eliminating 0s and negative values
		if(sortArray[i].sortBy <= 0.0f) continue;
		short wordLength = (short)ceil(-log2(sortArray[i].sortBy));
		codeWords[sortArray[i].data] = {symbols[sortArray[i].data], (FloatToCode(csPtr) & ~(0xFFFFFFFF >> wordLength)), wordLength}; // the bit manipulation just grabs the right length of the code
		cumulativeSum += sortArray[i].sortBy;
	}
}

// struct for storing the combined leaves in the Huffman algorithm
template <int N> struct HuffmanCellStruct {
	int childrenN;
	int children[N];
};
// takes pmf and symbols and outputs array of code word structs
template <int N> void Huffman(const float pArray[N], const char symbols[N], CodeWord codeWords[N]){
	for(int i=0; i<N; i++) codeWords[i] = {symbols[i], 0x00000000, 0};
	// sorting in descending order
	SortStruct<HuffmanCellStruct<N>> sortArray[N];
	for(int i=0; i<N; i++) sortArray[i] = {pArray[i], {1, {i/*, 0, 0, 0, ...*/}}};
	// initally sorts just once to make it easier to subsequently find two smallest values
	Quicksort(sortArray, 0, N-1);
	
	// ignores symbols with p = 0
	int arrayLength = N; while(sortArray[arrayLength-1].sortBy <= 0.0f) arrayLength--;
	
	while(arrayLength > 1){
		// grabs info about branches with smallest two probabilities (the last two in the array)
		HuffmanCellStruct<N> data0, data1;
		memcpy(&data0, &sortArray[arrayLength-2].data, sizeof(HuffmanCellStruct<N>));
		memcpy(&data1, &sortArray[arrayLength-1].data, sizeof(HuffmanCellStruct<N>));
		// prepends a 0 on all children of one of them
		for(int i=0; i<data0.childrenN; i++){
			codeWords[data0.children[i]].codeWord >>= 1;
			codeWords[data0.children[i]].wordLength++;
		}
		// prepends a 1 on all children of the one of them
		for(int i=0; i<data1.childrenN; i++){
			codeWords[data1.children[i]].codeWord >>= 1;
			codeWords[data1.children[i]].codeWord |= 0x80000000;
			codeWords[data1.children[i]].wordLength++;
		}
		// combines probabilities
		float newVal = sortArray[arrayLength-2].sortBy + sortArray[arrayLength-1].sortBy;
		// inserts the new combined branch into the array, keeping the array sorted without having to sort the whole thing again
		int insertPos;
		for(insertPos=0; insertPos<arrayLength-2; insertPos++) if(newVal > sortArray[insertPos].sortBy) break;
		for(int j=arrayLength-2; j>insertPos; j--) sortArray[j] = sortArray[j-1];
		sortArray[insertPos].sortBy = newVal;
		sortArray[insertPos].data.childrenN = data0.childrenN + data1.childrenN;
		memcpy(sortArray[insertPos].data.children, data0.children, data0.childrenN * sizeof(int));
		memcpy(sortArray[insertPos].data.children + data0.childrenN, data1.children, data1.childrenN * sizeof(int));
		// have comined two branches into one, so arary length is reduced by 1
		arrayLength--;
	}
}

// I tried to do the Arithmetic coding with manual bit manipulation so that I had control of the rounding to avoid the floating-point rounding errors discussed in the lab sheet, but I couldn't get my code to work. Here is the code I had anyway; it doesn't work though.
/*
 int32_t scaledCumulativeA = (int32_t)(FloatToCode(&cumulative) >> 1);
 const float hi = cumulative + pArray[i];
 int32_t scaledCumulativeB = (int32_t)(FloatToCode(&hi) >> 1);
 while(scaledCumulativeA >= 0 || scaledCumulativeB > 0){
	 codeWords[i].codeWord <<= 1;
	 codeWords[i].wordLength++;
	 int32_t mid = (scaledCumulativeA + scaledCumulativeB) >> 1;
	 if(mid >= 0x01000000 || mid < 0){
		 scaledCumulativeA -= 0x01000000;
		 scaledCumulativeB -= 0x01000000;
		 codeWords[i].codeWord |= 0x00000001;
	 }
	 scaledCumulativeA <<= 1;
	 scaledCumulativeB <<= 1;
 }
 */

// here is my successful code that just uses floats
// takes pmf and symbols and outputs array of code word structs
template <int N> void Arithmetic(const float pArray[N], const char symbols[N], CodeWord codeWords[N]){
	float cumulative = 0.0f;
	for(int i=0; i<N; i++){
		codeWords[i] = {symbols[i], 0x00000000, 0};
		if(pArray[i] <= 0.0f) continue;
		// makes copies of 'lo' and 'hi'
		float scaledCumulativeA = cumulative;
		float scaledCumulativeB = scaledCumulativeA + pArray[i];
		// repeates while the range doesn't fully enclose the range 0.0f to 1.0f
		while(scaledCumulativeA > 0.0f || scaledCumulativeB < 1.0f){
			// append either a 0 or 1 on the beginning of the code word
			codeWords[i].codeWord <<= 1;
			codeWords[i].wordLength++;
			// the append bit is a 1 if the centre of the range is above 0.5f
			if(0.5f*(scaledCumulativeA + scaledCumulativeB) >= 0.5f){
				// the copies of 'lo' and 'hi are shifted down by a half in this case also
				scaledCumulativeA -= 0.5f;
				scaledCumulativeB -= 0.5f;
				codeWords[i].codeWord |= 0x00000001;
			}
			// the copies of 'lo' and 'hi' are scaled up as we go deeper
			scaledCumulativeA *= 2.0f;
			scaledCumulativeB *= 2.0f;
		}
		// currently the word is at the right-hand end of the 32-bit unsigned integer, this shifts it to sit at the left end
		codeWords[i].codeWord <<= (32 - codeWords[i].wordLength);
		// the code word for this symbol is now complete
		// adding to the cumulative probaility
		cumulative += pArray[i];
	}
}

// encodes a char array into a bit array in the form of a byte array using the given code
long Encode(const char *data, int alphabetSize, CodeWord codeWords[], uint8_t outputBuffer[]){
	long inputPosition = 0;
	long outputPosition = 0;
	short bitPosition = 3; // follows the padding bits convention as stated in the lab handout
	int translate[256];
	// produces a lookup array for the code for each symbol
	for(int i=0; i<alphabetSize; i++) translate[(uint8_t)codeWords[i].symbol] = i;
	while(data[inputPosition] != '\0'){
		// finds the code word for the next symbol
		CodeWord s = codeWords[translate[(uint8_t)data[inputPosition]]];
		uint32_t wordLeft = s.codeWord;
		short lengthLeft = s.wordLength;
		// fits the codeword into the byte array, filling up the current byte and putting the rest of the word into subsequent bytes 8 bits at a time
		while(true){
			short toFill = 8 - bitPosition;
			outputBuffer[outputPosition] |= (uint8_t)(wordLeft >> (bitPosition + 24));
			bitPosition += lengthLeft;
			if(bitPosition >= 8){
				bitPosition = 0;
				outputPosition++;
			}
			lengthLeft -= toFill;
			if(lengthLeft <= 0) break;
			wordLeft <<= toFill;
		}
		inputPosition++;
	}
	// follows the padding bits convention as stated in the lab handout
	uint8_t paddingBits;
	if(bitPosition){
		outputPosition++;
		paddingBits = 8 - bitPosition;
	} else paddingBits = 0;
	outputBuffer[0] |= paddingBits << 5;
	// returns the length of the encoded message
	return outputPosition;
}

// crude algorithm to decode a bit array in the form of a byte array into a char arry using the given code
void Decode(long bytesN, uint8_t code[], int alphabetSize, CodeWord codeWords[], char outputBuffer[]){
	long bitsN = 8*bytesN - (long)(code[0] >> 5);
	long inputPosition = 0;
	short bitPosition = 3;
	long outputPosition = 0;
	while(inputPosition*8 + bitPosition < bitsN){
		// the maximum code word length is 32, so finds the next 32 bits in the byte array and stores them in a uint32_t
		uint32_t next32 = ((uint32_t)(code[inputPosition] << bitPosition) << 24);
		long forwardPos = inputPosition + 1;
		int j = 0;
		while(forwardPos*8 < bitsN && j < 3){
			next32 |= (uint32_t)code[forwardPos] << (bitPosition + 8*(2 - j));
			forwardPos++;
			j++;
		}
		// loops through all code words until it finds a match with the next 32 bits in the byte array
		bool found = false;
		for(int i=0; i<alphabetSize; i++){
			if(!codeWords[i].wordLength) continue;
			if(inputPosition*8 + bitPosition + codeWords[i].wordLength > bitsN) continue;
			// the bit manipulation just sets all bits of the next 32 beyond the code length we are comparing with to 0, and uses XOR to see if the uin32_ts are the same
			if((next32 ^ codeWords[i].codeWord) & (0xFFFFFFFF << (32 - codeWords[i].wordLength))) continue;
			outputBuffer[outputPosition++] = codeWords[i].symbol;
			bitPosition += codeWords[i].wordLength;
			found = true;
			break;
		}
		if(!found){
			// the next 32 bits do not match any code word
			std::cout << "ERROR: Unrecognised symbol." << std::endl;
			bitPosition++;
		}
		inputPosition += bitPosition / 8;
		bitPosition = bitPosition % 8;
	}
	// null terminator
	outputBuffer[outputPosition++] = '\0';
}

//const int size = 26;
//float pmf[size] = {0.084966, 0.020720f, 0.045388f, 0.033844f, 0.111607, 0.018121f, 0.024705f, 0.030034f, 0.075448f, 0.001965f, 0.011016f, 0.054893f, 0.030129f, 0.066544f, 0.071635f, 0.031671f, 0.001962f, 0.075809f, 0.057351f, 0.069509f, 0.036308f, 0.010074f, 0.012899f, 0.002902f, 0.017779f, 0.002722f};
//char symbols[size] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

// ASCII has 256 symbols
const int size = 256;

int main(int argc, const char *argv[]){
	char textBuffer[262144];
	
	std::ifstream inputFile;
	inputFile.open("<path to directory>/hamlet.txt", std::ios::in);
	inputFile.read(textBuffer, 262144);
	inputFile.close();
	textBuffer[262143] = '\0';
	
	// for debugging
	//const char *myText = "Hellooo";
	//memcpy(textBuffer, myText, strlen(myText));
	
	long inputLength = 0; while(textBuffer[inputLength] != '\0') inputLength++;
	
	// finds the pmf
	int frequencies[size];
	for(int i=0; i<size; i++) frequencies[i] = 0;
	int i = 0;
	char c = textBuffer[i];
	while(c != '\0'){
		frequencies[(int)c]++;
		c = textBuffer[++i];
	}
	const float fTotal = (float)i;
	float pmf[size];
	for(int i=0; i<size; i++) pmf[i] = (float)frequencies[i] / fTotal;
	
	// to store the code in
	CodeWord codes[size];
	char symbols[size]; for(int i=0; i<size; i++) symbols[i] = (char)i;
	
	// produces a code
	//ShannonFano<size>(pmf, symbols, codes);
	Huffman<size>(pmf, symbols, codes);
	//Arithmetic<size>(pmf, symbols, codes);
	
	// print code words
	std::cout << std::endl;
	for(int i=0; i<size; i++){
		if(!codes[i].wordLength) continue;
		std::cout << codes[i].symbol << " (" << (short)codes[i].symbol << "): " << codes[i].wordLength << "; " << std::bitset<32>(codes[i].codeWord) << std::endl;
	}
	
	// buffer for the output of the encoder
	uint8_t outputBuffer[2097152];
	// encodes the text with the code
	long length = Encode(textBuffer, size, codes, outputBuffer);
	// adds some incorrect bits
	//outputBuffer[20] = !outputBuffer[20];
	
	// print code
	for(long i=0; i < 1 + length/8; i++) std::cout << std::bitset<8>(outputBuffer[i]);
	std::cout << std::endl;
	
	// write to binary file
	std::ofstream outputFile;
	outputFile.open("<path to directory>/hamletZipped.bin", std::ios::out | std::ios::binary);
	outputFile.write((const char *)outputBuffer, length);
	outputFile.close();
	
	std::cout << "Compression ratio = " << (float)(length*8) / (float)inputLength << std::endl;
	
	float entropy = 0.0; for(int i=0; i<size; i++) if(pmf[i]) entropy -= pmf[i]*log2(pmf[i]);
	std::cout << "Source entropy = " << entropy << std::endl;
	
	// buffer for the decoded message
	char decodeBuffer[2097152];
	// decodes the message with the code
	Decode(length, outputBuffer, size, codes, decodeBuffer);
	std::cout << decodeBuffer << std::endl;
	
	return 0;
}
