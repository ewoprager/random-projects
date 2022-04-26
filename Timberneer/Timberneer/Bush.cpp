#include "Bush.hpp"
#include "Item.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Hud.hpp"

extern Player *player;

extern int hoveringId;

extern const uint8_t layers_worldObjects_in;
extern const float radius_bush;

Bush::Bush(float yaw, vec2 _position) : RenderedInstanced(ip_bush, true), Collider(layers_worldObjects_in, radius_bush, _position) {
	SetBaseMatrix(0.5f, yaw);
	TransformMatrixToWorldPosition(_position, 0.0f, true);
		
	growTimer = (float)(rand() % 60);
}

int Bush::infoHD, Bush::pickHD;
void Bush::Construct(){
	infoHD = Hud_AddText(infoText, hp_info);
	pickHD = Hud_AddText(pickText, hp_primary);
}

void Bush::Update(float deltaTime) {
	if(grown) return;
	growTimer -= deltaTime;
	if(growTimer <= 0){
		grown = true;
		ChangeParent(ip_berryBush);
		if(hoveringId == GetParentId()) Hud_Set(hp_primary, pickHD);
	}
}

void Bush::StartHover(){
	Hud_Set(hp_info, infoHD);
	if(grown){
		Hud_Set(hp_primary, pickHD);
	} else {
		Hud_Reset(hp_primary, pickHD);
	}
}
void Bush::StopHover(){
	Hud_Reset(hp_info, infoHD);
	Hud_Reset(hp_primary, pickHD);
}

void Bush::LeftClick() {
	if(!grown) return;
	int toGive = 2;
	toGive -= player->Give({it_leaves_and_fibres, toGive});
	if(toGive) DropItemWithoutIntersect({it_leaves_and_fibres, toGive}, position);
	grown = false;
	SetGrowTimer();
	ChangeParent(ip_bush);
	if(hoveringId == GetParentId()) Hud_Reset(hp_primary, pickHD);
}
