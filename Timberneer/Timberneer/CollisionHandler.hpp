#ifndef CollisionHandler_hpp
#define CollisionHandler_hpp

#include "Header.hpp"
#include "Protocols.hpp"

// 8 layers, 1 for each bit
// (starting from the right/l.s.b.), layers are used by:
// 0: player, most mobile buildings
// 1: item drops
// 2: world objects (trees, bushes etc...)
// 3: most static buildings
// 4: water-friendly static buildings
// 5:
// 6:
// 7:

struct CheckCollider {
	vec2 position;
	float radius;
	uint8_t layers;
	int ignore;
};
CheckCollider operator+(const CheckCollider& col, const vec2& vec);

bool CollisionHandler_CheckPos(const CheckCollider& collider);
vec2 CollisionHandler_MoveToCol(const CheckCollider& collider, const vec2& displacement, const float& prescision);
vec2 CollisionHandler_DoubleMoveToCol(const CheckCollider& collider1, const CheckCollider& collider2, const vec2& displacement, const float& prescision);

#endif /* CollisionHandler_hpp */
