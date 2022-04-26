#ifndef River_hpp
#define River_hpp

#include "Header.hpp"

#define MAX_LINKS 10

void River_Construct();
void River_StartHover();
void River_StopHover();

void River_GeneratePath();
float River_Profile(const float& x);
float River_MinDistance(const vec2& v);
vec2 River_UnitNormalVector(const vec2& v);
void River_GenerateNormalMap();

#endif /* River_hpp */
