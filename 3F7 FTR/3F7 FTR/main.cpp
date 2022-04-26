#include <iostream>
#include <math.h>
#include <fstream>
#include <map>
#include "Axes.hpp" // my own numpy / matplotlib like c++ library

int main(int argc, const char *argv[]) {
	char textBuffer[262144];
	
	std::ifstream inputFile;
	inputFile.open("<path to text documents>/hamlet.txt", std::ios::in);
	inputFile.read(textBuffer, 262144);
	inputFile.close();
	textBuffer[262143] = '\0';
	long inputLength = 0; while(textBuffer[inputLength] != '\0') inputLength++;
	
	// 1.1: Entropy rate vs block size
	float x[8];
	float h[8];
	for(unsigned int blockSize=1; blockSize<=8; blockSize++){
		std::map<uint64_t, int> map;
		const long blocksN = inputLength-blockSize+1;
		for(int p=0; p<blocksN; p++){
			uint64_t block = 0x0000000000000000;
			for(int i=0; i<blockSize; i++) block |= (uint64_t)textBuffer[p + i] << (8*i);
			if(map.count(block)){
				map[block]++;
			} else {
				map[block] = 1;
			}
		}
		float fblocksN = (float)blocksN;
		float frequencyEntropy = 0.0f;
		// iterates through all the elements of the map:
		for(std::map<uint64_t, int>::iterator mi = map.begin(); mi != map.end(); mi++){
			int f = mi->second;
			frequencyEntropy -= log2((float)f) * (float)f;
		}
		x[blockSize-1] = (float)blockSize;
		h[blockSize-1] = (log2(fblocksN) + frequencyEntropy / fblocksN)/(float)blockSize;
	}
	// my own graph plotting stuff (using sdl) - used similarly to matplotlib
	Axes<1, 8> axes = Axes<1, 8>({false, NAN, NAN, "block size"}, {false, NAN, NAN, "entropy rate (bits/symbol)"});
	axes.AddPlot(8, x, h);
	axes.Execute();
	axes.Save();
	
	// 1.2
	// L/n vs n
	const int N = 200000;
	float LOverN[N];
	float delta = 1.0f;
	int k = 256;
	int freq[k];
	Fill(k, freq, 0); // my own populates an array with a common value
	float L = 0.0f;
	for(int n=1; n<=N; n++){
		L -= log2(((float)freq[textBuffer[n-1]] + delta)/((float)n + (float)k*delta));
		LOverN[n-1] = L/(float)n;
		freq[textBuffer[n-1]]++;
	}
	float n[N];
	Ramp(N, n, 1); // my own; fills the array like this: {1, 2, 3, 4, 5, 6, 7, 8, 9, ...}
	Axes<1, N> axes = Axes<1, N>({true, NAN, NAN, "n"}, {false, NAN, NAN, "L/n"});
	axes.AddPlot(N, n, LOverN);
	axes.Execute();
	axes.Save();
	
	// L/n vs delta (repeated for different text files):
	const int I = 200;
	float delta[I];
	int k = 256;
	Logspace(I, delta, 1.0e-5f, 2.0f); // my own; works like numpy.logspace
	Axes<20, I> axes = Axes<20, I>({true, NAN, NAN, "delta"}, {false, NAN, NAN, "L/n"});
	for(int N=40; N<=5120; N*=2){
		float LOverN[I];
		int freq[k];
		Fill(k, freq, 0);
		float L[I];
		Fill(I, L, 0.0f);
		for(int n=1; n<=N; n++){
			for(int i=0; i<I; i++) L[i] -= log2(((float)freq[textBuffer[n-1]] + delta[i])/((float)n + (float)k*delta[i]));
			freq[textBuffer[n-1]] += 1;
		}
		for(int i=0; i<I; i++) LOverN[i] = L[i]/(float)N;
		unsigned char r = (unsigned char)((log2((float)N) - 5.0f)*20.0f);
		axes.AddPlot(I, delta, LOverN, line, {r, (unsigned char)(255 - r), 0});
		if(N == 40){
			float derivative[I];
			Differentiate(I, derivative, LOverN, delta); // my own; numerical differentiation
			LerpedIndex indices[10]; // my own; stores interpolation information (a start point, end point and fraction). in conjunction with derivative find stationary points
			int spn = FindChangeOfSign(I, derivative, indices); // my own; does what it says on the tin, outputting interpolation values
			axes.HighlightIndices(0, spn, indices); // my own; does what it says on the tin, taking in interpolation values
		}
	}
	axes.Execute();
	axes.Save();
	
	// optimum delta vs n (repeated for different text files):
	const int I = 200;
	const int M = 1500;
	float delta[I];
	const int k = 256;
	Logspace(I, delta, 1.0e-5f, 2.0f);
	float oDelta[M];
	float Ns[M];
	Axes<1, M> axes = Axes<1, M>({true, NAN, NAN, "n"}, {false, NAN, NAN, "optimum delta"});
	for(int m=0; m<M; m++){
		const int N = 3*m;
		Ns[m] = N;
		float LOverN[I];
		int freq[k];
		Fill(k, freq, 0);
		float L[I];
		Fill(I, L, 0.0f);
		for(int n=1; n<=N; n++){
			for(int i=0; i<I; i++) L[i] -= log2(((float)freq[textBuffer[n-1]] + delta[i])/((float)n + (float)k*delta[i]));
			freq[textBuffer[n-1]] += 1;
		}
		for(int i=0; i<I; i++) LOverN[i] = L[i]/(float)N;
		float derivative[I];
		Differentiate(I, derivative, LOverN, delta);
		LerpedIndex indices[10];
		const int spn = FindChangeOfSign(I, derivative, indices);
		if(spn == 1){
			oDelta[m] = delta[indices[0].i1] + indices[0].lerpValue*(delta[indices[0].i2] - delta[indices[0].i1]);
		} else {
			oDelta[m] = 0.0f;
		}
	}
	axes.AddPlot(M, Ns, oDelta, line);
	axes.Execute();
	axes.Save();
	
	// L/n vs n for adaptive and nonadaptive
	const int N = 20000;
	float LOverN_adaptive[N];
	float LOverN_nonadaptive[N];
	int k = 256;
	int freq[k];
	
	float n[N];
	Ramp(N, n, 1);
	Axes<2, N> axes = Axes<2, N>({true, NAN, NAN, "n"}, {false, NAN, NAN, "L/n"});
	
	float delta = 0.04f;
	Fill(k, freq, 0);
	float L = 0.0f;
	for(int n=1; n<=N; n++){
		L -= log2(((float)freq[textBuffer[n-1]] + delta)/((float)n + (float)k*delta));
		LOverN_adaptive[n-1] = L/(float)n;
		freq[textBuffer[n-1]]++;
	}
	axes.AddPlot(N, n, LOverN_adaptive, line, {255, 0, 0});
	
	Fill(k, freq, 0);
	for(int n=1; n<=N; n++){
		freq[textBuffer[n-1]]++;
		float L = 0.0f;
		for(int x=0; x<k; x++){
			if(!freq[x]) continue;
			L -= (float)freq[x] * log2((float)freq[x] / (float)n);
		}
		LOverN_nonadaptive[n-1] = L/(float)n;
	}
	axes.AddPlot(N, n, LOverN_nonadaptive, line, {0, 255, 0});
	
	axes.Execute();
	axes.Save();
	
	// bits/n vs n for adaptive and nonadaptive
	const int N = 200000;
	float LOverN_adaptive[N];
	float LOverN_nonadaptive[N];
	int k = 256;
	int freq[k];
	
	float n[N];
	Ramp(N, n, 1);
	Axes<2, N> axes = Axes<2, N>({false, NAN, NAN, "n"}, {false, 4.61f, 4.67f, "L/n"});
	
	float delta = 0.04f;
	Fill(k, freq, 0);
	float L = 0.0f;
	for(int n=1; n<=N; n++){
		L -= log2(((float)freq[textBuffer[n-1]] + delta)/((float)n + (float)k*delta));
		LOverN_adaptive[n-1] = L/(float)n;
		freq[textBuffer[n-1]]++;
	}
	axes.AddPlot(N, n, LOverN_adaptive, line, {255, 0, 0});
	
	Fill(k, freq, 0);
	for(int n=1; n<=N; n++){
		freq[textBuffer[n-1]]++;
		float L = 0.0f;
		for(int x=0; x<k; x++){
			if(!freq[x]) continue;
			L -= (float)(freq[x] + 1) * log2((float)freq[x] / (float)n);
			L += 1; // estimate for if storing words with dividing bits. this is achived by this line and '(freq[x] + 1)' instead of just 'freq[x]' in the above line
			//L += sizeof(float) * 8; // estimate for if storing non-zero probabilities only
		}
		//L += k * sizeof(float) * 8; // estimate for if storing all probabilities
		LOverN_nonadaptive[n-1] = L/(float)n;
	}
	axes.AddPlot(N, n, LOverN_nonadaptive, line, {0, 255, 0});
	
	axes.Execute();
	axes.Save();
	
	return 0;
}
