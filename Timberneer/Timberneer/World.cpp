#include "World.hpp"

#include "Perlin.hpp"
#include "Item.hpp"
#include "ItemDrop.hpp"
#include "Player.hpp"
#include "Cube.hpp"
#include "Bush.hpp"
#include "Tree.hpp"
#include "CollisionHandler.hpp"
#include "River.hpp"
#include "Graveyard.hpp"

static float Private_ProbabilityFunction(const float& x, const float& zero, const float& max);
static void Private_SpawnTree(const vec2& pos);
static void Private_SpawnBush(const vec2& pos);
static void Private_MakeRiverTrench();

// objects
static Ground *ground;
Player *player;

// map in general
static const int mapSizeCells = 150;
static const float mapCellSize = 0.5f;
static const float mapCellSizeInv = 1.0f / mapCellSize;
extern const float mapSize = (float)mapSizeCells * mapCellSize;
// height map
static const float heightScale = 2.0f;
static const int heightNoiseSize = mapSizeCells / 17;
static float heightMap[mapSizeCells][mapSizeCells];
static vec3 normalMap[mapSizeCells][mapSizeCells];
extern const float riverHalfWidth;
// world objects
struct WorldObject {
	float radius, probZero, probMult;
	void (*SpawnFunc)(const vec2& position);
};
extern const float radius_tree;
extern const float radius_bush;
static const WorldObject bushWorldObject = {radius_bush, 0.8f, 0.7f, &Private_SpawnBush};
static const WorldObject treeWorldObject = {radius_tree, 0.7f, 1.0f, &Private_SpawnTree};
static const float rockDensity = 0.01f;
static const float graveyardDensity = 0.002f;
extern int graveyardsN;
extern Graveyard *graveyardArray[];

extern const uint8_t layers_usedBy_worldObjects;

template <int spawnCellsN, int noiseCellsN> void Private_GenerateWorldObject(const WorldObject& wo){
	const float cellSize = mapSize / (float)spawnCellsN;
	const float probMax = wo.probMult * 0.5f * cellSize * cellSize / wo.radius / wo.radius;
	const float c0 = -0.5f * mapSize;
	float probMap[spawnCellsN][spawnCellsN];
	PerlinNoise2D<noiseCellsN> *perlin = new PerlinNoise2D<noiseCellsN>();
	MakeHeightMap<noiseCellsN>(perlin, 1.0f, probMap);
	bool spawnMap[spawnCellsN][spawnCellsN];
	for(int j=0; j<spawnCellsN; j++) for(int i=0; i<spawnCellsN; i++) spawnMap[j][i] = false;
	for(int j=0; j<spawnCellsN; j++){
		for(int i=0; i<spawnCellsN; i++){
			const float randVal = (float)(rand() % 1001) * 0.001f;
			spawnMap[j][i] = (randVal < Private_ProbabilityFunction(0.5f + probMap[j][i], wo.probZero, probMax));
			if(!spawnMap[j][i]) continue;
			const vec2 posTry = {c0 + cellSize*(float)i, c0 + cellSize*(float)j};
			if(CollisionHandler_CheckPos({posTry, wo.radius, layers_usedBy_worldObjects, -1})) continue;
			wo.SpawnFunc(posTry);
		}
	}
}

void World_Construct(){
	// height map
	River_GeneratePath();
	PerlinNoise2D<heightNoiseSize> *perlin = new PerlinNoise2D<heightNoiseSize>();
	MakeHeightMap<heightNoiseSize>(perlin, heightScale, heightMap);
	delete perlin;
	Private_MakeRiverTrench();
	
	River_GenerateNormalMap();
	
	ground = new Ground(MakeTerrain(heightMap, mapCellSize, "grass 3", 4, normalMap)); // must be the first interactable object created
	player = new Player({0.0f, 0.0f});
	
	new Cube(0.0f, {1.0f, 1.0f});
	
	Private_GenerateWorldObject<mapSizeCells * 6, mapSizeCells / 19>(bushWorldObject);
	Private_GenerateWorldObject<mapSizeCells * 3, mapSizeCells / 10>(treeWorldObject);
	
	const int rockNumber = rockDensity * mapSize * mapSize;
	for(int i=0; i<rockNumber; i++) new ItemDrop({it_rock, 1}, RandomAngle(), World_RandomPosition());
	
	const int graveyardNumber = graveyardDensity * mapSize * mapSize;
	for(int i=0; i<graveyardNumber; i++){
		graveyardArray[graveyardsN++] = new Graveyard(RandomAngle(), World_RandomPosition());
	}
}

void World_Destruct(){
	delete player;
	delete ground;
}

float World_GetGroundHeight(const vec2& pos){
	const float sx = pos.x * mapCellSizeInv + (float)mapSizeCells*0.5f;
	const float sz = pos.y * mapCellSizeInv + (float)mapSizeCells*0.5f;
	const int i0 = (int)sx;
	const int j0 = (int)sz;
	if(i0 < 0 || i0 > mapSizeCells ||
	   j0 < 0 || j0 > mapSizeCells){
		std::cout << "ERROR: Ground position out of range." << std::endl;
		return 0.0f;
	}
	const int i1 = (i0 + 1) % mapSizeCells;
	const int j1 = (j0 + 1) % mapSizeCells;
	const float fx = sx - (float)i0;
	const float fz = sz - (float)j0;
	const float xLerp0 = heightMap[j0][i0] + fx*(heightMap[j0][i1] - heightMap[j0][i0]);
	const float xLerp1 = heightMap[j1][i0] + fx*(heightMap[j1][i1] - heightMap[j1][i0]);
	return xLerp0 + fz*(xLerp1 - xLerp0);
}

vec3 World_GetGroundNormal(const vec2& pos){
	const float sx = pos.x * mapCellSizeInv + (float)mapSizeCells*0.5f;
	const float sz = pos.y * mapCellSizeInv + (float)mapSizeCells*0.5f;
	const int i0 = (int)sx;
	const int i1 = i0 + 1;
	const int j0 = (int)sz;
	const int j1 = j0 + 1;
	if(i0 < 0 || i0 > mapSizeCells ||
	   i1 < 0 || i1 > mapSizeCells ||
	   j0 < 0 || j0 > mapSizeCells ||
	   j1 < 0 || j1 > mapSizeCells){
		std::cout << "ERROR: Ground position out of range." << std::endl;
		return {0.0f, 1.0f, 0.0f};
	}
	const float fx = sx - (float)i0;
	const float fz = sz - (float)j0;
	const vec3 xLerp0 = (normalMap[j0][i0] + fx*(normalMap[j0][i1] - normalMap[j0][i0])).Normalise();
	const vec3 xLerp1 = (normalMap[j1][i0] + fx*(normalMap[j1][i1] - normalMap[j1][i0])).Normalise();
	return V3_Normalised(xLerp0 + fz*(xLerp1 - xLerp0));
}

static float Private_ProbabilityFunction(const float& x, const float& zero, const float& max){
	float ret = max*(x - zero)/(1.0f - zero);
	if(ret <= 0.0f) return 0.0f;
	return ret;
}

static void Private_SpawnTree(const vec2& pos){
	new Tree(RandomAngle(), pos);
}
static void Private_SpawnBush(const vec2& pos){
	new Bush(RandomAngle(), pos);
}

// doesn't seem to do much very useful
/*template <int perlinSize> static void Private_MakeRiverTrench(PerlinNoise2D<perlinSize> *perlin){
	float scale = mapSize / (float)(perlinSize - 1);
	float offset = - mapSize * 0.5f;
	for(int j=0; j<perlinSize; j++){
		float y = offset + scale*(float)j;
		for(int i=0; i<perlinSize; i++){
			float x = offset + scale*(float)i;
			vec2 v = {x, y};
			float dist = World_DistanceFromRiver(v);
			if(dist > scale) continue;
			perlin->field[j][i] = World_PerpendicularToRiver(v);
		}
	}
}*/
// actually makes a good trench
static void Private_MakeRiverTrench(){
	float offset = - mapSize * 0.5f;
	for(int j=0; j<mapSizeCells+1; j++){
		float y = offset + mapCellSize*(float)j;
		for(int i=0; i<mapSizeCells+1; i++){
			float x = offset + mapCellSize*(float)i;
			vec2 v = {x, y};
			float dist = River_MinDistance(v);
			if(dist > riverHalfWidth) continue;
			float o = 2.3f*dist/riverHalfWidth;
			float weight = exp(-o*o);
			heightMap[j][i] = (1.0f - weight)*heightMap[j][i] + weight*(heightScale + 1.5f)*River_Profile(dist/riverHalfWidth);
		}
	}
}

vec2 World_RandomPosition(){
	return mapSize * (vec2){-0.5f + 0.0001f*(float)(rand() % 10001), -0.5f + 0.0001f*(float)(rand() % 10001)};
}

void World_GroundAlignmentMatrix(const vec2& pos, float out[4][4]){
	static const vec3 up = {0.0f, 1.0f, 0.0f};
	const vec3 norm = World_GetGroundNormal(pos);
	M4x4_AxisRotation(up * norm, V3_Cross(up, norm).Normalise(), out);
}
