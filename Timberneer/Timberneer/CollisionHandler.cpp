#include "CollisionHandler.hpp"
#include "River.hpp"

extern const uint8_t layers_player_in = 0x1B;
extern const uint8_t layers_itemDrop_in = 0x1A;
extern const uint8_t layers_worldObjects_in = 0x1F;
extern const uint8_t layers_river_in = 0x0C;
extern const uint8_t layers_undead_in = 0x1B;

extern const uint8_t layers_usedBy_player = 0x01;
extern const uint8_t layers_usedBy_itemDrop = 0x02;
extern const uint8_t layers_usedBy_worldObjects = 0x04;
extern const uint8_t layers_usedBy_undead = 0x01;

extern const float radius_player = 0.25f;
extern const float radius_itemDrop = 0.25f;
extern const float radius_tree = 0.5f;
extern const float radius_bush = 0.3f;
extern const float radius_undead = 0.25f;

SmartArray<MAX_OBJECTS, Collider *> colliderArray = SmartArray<MAX_OBJECTS, Collider *>();
extern const float riverColHalfWidth;

bool CollisionHandler_CheckPos(const CheckCollider& collider){
	if(collider.layers & layers_river_in) if(River_MinDistance(collider.position) < collider.radius+riverColHalfWidth) return true;
	for(int i=0; i<colliderArray.GetN(); i++){
		Collider *col = colliderArray[i];
		if(col->GetColId() == collider.ignore) continue;
		if(!(collider.layers & col->GetLayers())) continue;
		float radSum = collider.radius + col->GetRad();
		if((collider.position - col->GetPos()).SqMag() < radSum*radSum) return true;
	}
	return false;
}

vec2 CollisionHandler_MoveToCol(const CheckCollider& collider, const vec2& displacement, const float& prescision){
	const int iterations = 1 + (int)log2(1.570796327f / prescision);
	float angle = 0.0f;
	float d = 0.7853981634f;
	vec2 tryLeft, tryRight;
	vec2 ret = (vec2){0.0f, 0.0f};
	for(int i=0; i<iterations; i++){
		tryLeft = displacement.ComponentRelativeAngle(angle);
		if(CollisionHandler_CheckPos({collider.position + tryLeft, collider.radius, collider.layers, collider.ignore})){
			tryRight = displacement.ComponentRelativeAngle(-angle);
			if(CollisionHandler_CheckPos({collider.position + tryRight, collider.radius, collider.layers, collider.ignore})){
				angle += d;
			} else {
				if(i == 0) return tryRight;
				ret = tryRight;
				angle -= d;
			}
		} else {
			if(i == 0) return tryLeft;
			ret = tryLeft;
			angle -= d;
		}
		d *= 0.5f;
	}
	return ret;
}
vec2 CollisionHandler_DoubleMoveToCol(const CheckCollider& collider1, const CheckCollider& collider2, const vec2& displacement, const float& prescision){
	const int iterations = 1 + (int)log2(1.570796327f / prescision);
	float angle = 0.0f;
	float d = 0.7853981634f;
	vec2 tryLeft, tryRight;
	vec2 ret = (vec2){0.0f, 0.0f};
	for(int i=0; i<iterations; i++){
		tryLeft = displacement.ComponentRelativeAngle(angle);
		if(CollisionHandler_CheckPos(collider1 + tryLeft) || CollisionHandler_CheckPos(collider2 + tryLeft)){
			tryRight = displacement.ComponentRelativeAngle(-angle);
			if(CollisionHandler_CheckPos(collider1 + tryRight) || CollisionHandler_CheckPos(collider2 + tryRight)){
				angle += d;
			} else {
				if(i == 0) return tryRight;
				ret = tryRight;
				angle -= d;
			}
		} else {
			if(i == 0) return tryLeft;
			ret = tryLeft;
			angle -= d;
		}
		d *= 0.5f;
	}
	return ret;
}

CheckCollider operator+(const CheckCollider& col, const vec2& vec){
	return {col.position + vec, col.radius, col.layers, col.ignore};
}
