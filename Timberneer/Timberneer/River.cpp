#include "River.hpp"
#include "App.hpp"
#include "Perlin.hpp"
#include "Hud.hpp"

#include <math.h>

struct RiverPoint {
	float x, y;
	
	int links[MAX_LINKS];
	int linksN;
	
	bool visited;
	int prev;
	
	operator vec2() const { return {x, y}; }
};

bool Private_Calculate();
void Private_SucceedCalculate();

extern const float mapSize;
static const int minConnections = 4;
static const int extraSamples = 30;
static const int pointsN = 80;
static RiverPoint list[pointsN];
int riverPointsN;
vec2 riverPointBuffer[1024];
extern const float riverHalfWidth = 10.0f;
extern const float riverColHalfWidth = 5.0f;
extern const float waterHeight = -2.5f;
extern const float reflectionMatrix[4][4] = {
	{1.0f, 0.0f, 0.0f, 0.0f},
	{0.0f, -1.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 1.0f, 0.0f},
	{0.0f, 2.0f * waterHeight, 0.0f, 1.0f}
};
extern const vec4 waterClipPlane = {0.0f, 1.0f, 0.0f, -waterHeight};
extern const int riverNormalMapSize = 128;
uint8_t riverNormalMap[riverNormalMapSize*riverNormalMapSize*3];
static const int riverHeightNoiseSizesN = 2;
static constexpr int riverHeightNoiseSizes[riverHeightNoiseSizesN] = {25, 70};
static const float riverHeightNoiseWeights[riverHeightNoiseSizesN] = {0.85f, 0.15f}; // must add up to 1.0f
extern GLuint riverNormalMapTexture;
static const float riverNormalHarshness = 0.2f;
static const float riverNormalValueScale = 0.03f;

static const char *infoText = "Water";
static const char *interactText = "Fill bucket";
static int infoHD, interactHD;
void River_Construct(){
	infoHD = Hud_AddText(infoText, hp_info);
	interactHD = Hud_AddText(interactText, hp_primary);
}
void River_StartHover(){
	Hud_Set(hp_info, infoHD);
	Hud_Set(hp_primary, interactHD);
}
void River_StopHover(){
	Hud_Reset(hp_info, infoHD);
	Hud_Reset(hp_primary, interactHD);
}

/*float Private_InvertibleS(float x){
	static const float harshness = 2.5f;
	return 1.0f - 1.0f/(1.0f + pow(1.0f/(1.0f - x) - 1.0f, harshness));
}
vec3 Private_InvertibleSV(vec3 v){
	return {Private_InvertibleS(v.x), Private_InvertibleS(v.y), Private_InvertibleS(v.z)};
}*/
void River_GenerateNormalMap(){
	static float riverHeightMap[riverNormalMapSize][riverNormalMapSize];
	static float perlinScales[riverHeightNoiseSizesN];
	PerlinNoise2D<riverHeightNoiseSizes[0]> noise0 = PerlinNoise2D<riverHeightNoiseSizes[0]>();
	PerlinNoise2D<riverHeightNoiseSizes[1]> noise1 = PerlinNoise2D<riverHeightNoiseSizes[1]>();
	for(int i=0; i<riverHeightNoiseSizesN; i++) perlinScales[i] = (float)riverHeightNoiseSizes[i] / (float)riverNormalMapSize;
	for(int j=0; j<riverNormalMapSize; j++){
		for(int i=0; i<riverNormalMapSize; i++){
			float sum = 0.0f;
			sum += riverHeightNoiseWeights[0] * noise0.GetVal((float)i * perlinScales[0], (float)j * perlinScales[0]);
			sum += riverHeightNoiseWeights[1] * noise1.GetVal((float)i * perlinScales[1], (float)j * perlinScales[1]);
			riverHeightMap[j][i] = sum; // between -0.5 and 0.5
		}
	}
	
	static const float doubleCellSize = 2.0f / riverNormalHarshness;
	for(unsigned int r=0; r<riverNormalMapSize; r++){
		for(unsigned int c=0; c<riverNormalMapSize; c++){
			vec3 v1 = {0.0f, riverHeightMap[r][(c + 1) % riverNormalMapSize] - riverHeightMap[r][(c - 1) % riverNormalMapSize], doubleCellSize};
			vec3 v2 = {doubleCellSize, riverHeightMap[(r + 1) % riverNormalMapSize][c] - riverHeightMap[(r - 1) % riverNormalMapSize][c], 0.0f};
			vec3 n = V3_Cross(v1, v2).Normalise();
			if(n.y < 0) n *= -1.0f;
			n.y = 1.0f - n.y;
			n = 256.0f*0.5f*(n + (vec3){riverNormalValueScale, riverNormalValueScale, riverNormalValueScale}) / riverNormalValueScale;
			int index = 3 * (r*riverNormalMapSize + c);
			riverNormalMap[index + 0] = (uint8_t)n.x;
			riverNormalMap[index + 1] = (uint8_t)n.y;
			riverNormalMap[index + 2] = (uint8_t)n.z;
		}
	}
	
	glGenTextures(1, &riverNormalMapTexture);
	glBindTexture(GL_TEXTURE_2D, riverNormalMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, riverNormalMapSize, riverNormalMapSize, 0, GL_RGB, GL_UNSIGNED_BYTE, riverNormalMap);
}

bool Private_Calculate(){
	// generate roughly evenly spaced points, randomly across map
	// start and end point:
	list[0].x = 0.0f;
	list[0].y = -mapSize*0.5f;
	list[1].x = 0.0f;
	list[1].y = mapSize*0.5f;
	// rest of points
	for(int p=2; p<pointsN; p++){
		float xMax = 0.0f;
		float yMax = 0.0f;
		float maxDist = 0.0f;
		for(int s=0; s<p+extraSamples; s++){ // more samples as more points have been placed down
			const float x = mapSize * (-0.5f + (float)(rand() % 10001) * 1.0e-4);
			const float y = mapSize * (-0.5f + (float)(rand() % 10001) * 1.0e-4);
			float xMin = 0.0f;
			float yMin = 0.0f;
			float minDist = 1.0e8;
			for(int i=0; i<p; i++){
				const float dx = x - list[i].x;
				const float dy = y - list[i].y;
				const float dist = dx*dx + dy*dy;
				if(dist < minDist){
					xMin = x;
					yMin = y;
					minDist = dist;
				}
			}
			if(minDist > maxDist){
				xMax = xMin;
				yMax = yMin;
				maxDist = minDist;
			}
		}
		list[p].x = xMax;
		list[p].y = yMax;
	}
	
	// arrange into two-may multi-linked list, with a minimum number of links per point
	for(int p=0; p<pointsN; p++) list[p].linksN = 0;
	for(int p=0; p<pointsN; p++){
		while(list[p].linksN < minConnections){
			int minIndex = 0;
			float minDist = 1.0e8;
			for(int i=0; i<pointsN; i++){
				if(i == p) continue;
				bool skip = false;
				for(int l=0; l<list[p].linksN && !skip; l++) if(i == list[p].links[l]) skip = true;
				if(skip) continue;
				const float dx = list[p].x - list[i].x;
				const float dy = list[p].y - list[i].y;
				const float dist = dx*dx + dy*dy;
				if(dist < minDist){
					minIndex = i;
					minDist = dist;
				}
			}
			list[p].links[list[p].linksN++] = minIndex;
			list[minIndex].links[list[minIndex].linksN++] = p;
		}
	}
	
	// find path through points
	for(int p=0; p<pointsN; p++) list[p].visited = false;
	int p = 0;
	list[0].prev = -1;
	while(p != 1){
		list[p].visited = true;
		int chooseN = 0;
		int chooseBuffer[MAX_LINKS];
		for(int l=0; l<list[p].linksN; l++){
			int link = list[p].links[l];
			if(list[link].visited) continue;
			float dot = ((vec2)list[p] - (vec2)list[list[p].prev]) * ((vec2)list[link] - (vec2)list[p]);
			if(dot < 0.0f) continue;
			chooseBuffer[chooseN++] = link;
		}
		if(chooseN){
			int newP = chooseBuffer[rand() % chooseN];
			list[newP].prev = p;
			p = newP;
		} else {
			if(list[p].prev == -1) return false;
			p = list[p].prev;
		}
	}
	return true;
}
void Private_SucceedCalculate(){
	int count = 0;
	while(!Private_Calculate()){ count++; }
	if(count > 10) std::cout << "Warning: River path finding requires excess iterations." << std::endl;
}
void River_GeneratePath(){
	Private_SucceedCalculate();
	riverPointsN = 0;
	int p = 1;
	do {
		riverPointBuffer[riverPointsN++] = (vec2)list[p];
		p = list[p].prev;
	} while(p != 0);
}
float Private_ShiftedEase(const float& x){
	return -1.0f - 2.0f*x*x*x + 3.0f*x*x;
}
float River_Profile(const float& x){
	static const float baseWidth = 0.6f;
	static const float inv = 1.0f / (1.0f - baseWidth);
	if(x < baseWidth) return -1.0f;
	return Private_ShiftedEase(inv * (x - baseWidth));
}
float River_MinDistance(const vec2& v){
	float minDist = 1.0e8;
	for(int p=0; p<riverPointsN-1; p++){
		vec2 a = riverPointBuffer[p];
		vec2 b = riverPointBuffer[p+1];
		vec2 d = b - a;
		if((v - a)*d < 0.0f) continue;
		if((v - b)*d > 0.0f) continue;
		d.Normalise().Cross();
		float dist = fabs((v - a)*d);
		if(dist < minDist) minDist = dist;
	}
	for(int p=0; p<riverPointsN; p++){
		float dist = sqrt((v - riverPointBuffer[p]).SqMag());
		if(dist < minDist) minDist = dist;
	}
	return minDist;
}
vec2 River_UnitNormalVector(const vec2& v){
	float minDist = 1.0e8;
	vec2 minVec = {0.0f, 0.0f};
	for(int p=0; p<riverPointsN-1; p++){
		vec2 a = riverPointBuffer[p];
		vec2 b = riverPointBuffer[p+1];
		vec2 d = b - a;
		if((v - a)*d < 0.0f) continue;
		if((v - b)*d > 0.0f) continue;
		d.Normalise().Cross();
		float dist = fabs((v - a)*d);
		if(dist < minDist){
			minDist = dist;
			minVec = d;
			if((v - a)*minVec < 0) minVec *= -1.0f;
		}
	}
	for(int p=0; p<riverPointsN; p++){
		vec2 d = v - riverPointBuffer[p];
		float dist = sqrt(d.SqMag());
		if(dist < minDist){
			minDist = dist;
			minVec = V2_Normalised(d);
		}
	}
	return minVec;
}
