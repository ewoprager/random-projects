#ifndef World_hpp
#define World_hpp

#include "App.hpp"
#include "Protocols.hpp"

void World_Construct();
void World_Destruct();
float World_GetGroundHeight(const vec2& pos);
vec3 World_GetGroundNormal(const vec2& pos);
vec2 World_RandomPosition();
void World_GroundAlignmentMatrix(const vec2& pos, float out[4][4]);

class Ground : public Rendered {
public:
	Ground(ObjectData terrain) : Rendered(terrain, true) {}
};
#endif /* World_hpp */
