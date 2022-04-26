#include "Graveyard.hpp"
#include "Building.hpp"
#include "World.hpp"
#include "CollisionHandler.hpp"
#include "Player.hpp"
#include "ItemDrop.hpp"

extern Player *player;

extern int staticBulidingsN;
extern StaticBuilding *staticBuildingArray[];

int graveyardsN = 0;
Graveyard *graveyardArray[MAX_GRAVEYARDS];

extern const uint8_t layers_undead_in;
extern const uint8_t layers_usedBy_undead;
extern const float radius_undead;

Undead::Undead(vec2 _position) : RenderedInstanced(ip_undead, true), Collider(layers_undead_in, radius_undead, _position){
	SetBaseMatrix(0.25f, 0.0f);
	TransformMatrixToWorldPosition(_position, 0.25f);
}
void Undead::Update(float deltaTime){
	vec2 delta = player->GetPos() - position;
	float mag = sqrt(delta.SqMag());
	float dir;
	static const float dist = 1.5f;
	if(fabs(mag - dist) < deltaTime * speed){
		dir = 0.0f;
	} else {
		bool b = mag > 1.5f;
		dir = (float)(b - !b);
	}
	vec2 tryDisplacement = deltaTime * speed * (delta / mag) * dir;
	position += CollisionHandler_MoveToCol({position, GetRad(), layers_usedBy_undead, GetColId()}, tryDisplacement, 0.05f);
	
	ResetWorldMatrix();
	TransformMatrixToWorldPosition(position, 0.5f);
}
void Undead::LeftClick(){
	if(!player->GetHolding().stackSize) return;
	if(player->GetHolding().type == it_spear){
		new ItemDrop({it_heart, 1}, RandomAngle(), position, false);
		delete this;
	}
}

Graveyard::Graveyard(float yaw, vec2 _position) : RenderedInstanced(ip_graveyard, false) {
	position = _position;
	
	SetBaseMatrix(0.25f, yaw);
	TransformMatrixToWorldPosition(position, 0.25f);
	
	new Undead(position);
}

void Graveyard::Update(float deltaTime){
	
}
void Graveyard::UpdateAnger(){
	new Undead(position);
	std::cout << "Undead spawned" << std::endl;
}
