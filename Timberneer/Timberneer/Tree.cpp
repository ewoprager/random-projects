#include "Tree.hpp"

#include "Item.hpp"
#include "Player.hpp"
#include "World.hpp"

extern Player *player;

extern int hoveringId;

extern const uint8_t layers_worldObjects_in;
extern const float radius_tree;

Tree::Tree(float yaw, vec2 _position) : RenderedInstanced(ip_tree, true), Collider(layers_worldObjects_in, radius_tree, _position) {
	SetBaseMatrix(0.5f, yaw);
	TransformMatrixToWorldPosition(_position, 0.0f);
	
	stickTimer = (float)(rand() % 10);
}

int Tree::infoHD, Tree::chopHD;
void Tree::Construct(){
	infoHD = Hud_AddText(infoText, hp_info);
	chopHD = Hud_AddText(chopText, hp_primary);
}

void Tree::Update(float deltaTime){
	if(grown){
		stickTimer -= deltaTime;
		if(stickTimer <= 0.0f){
			DropItemWithoutIntersect({it_stick, 1}, position);
			SetStickTimer();
		}
	} else {
		growTimer -= deltaTime;
		if(growTimer <= 0.0f){
			grown = true;
			ChangeParent(ip_tree);
			SetStickTimer();
			if(hoveringId == GetParentId()) Hud_Set(hp_primary, chopHD);
		}
	}
}

void Tree::StartHover(){
	Hud_Set(hp_info, infoHD);
	if(grown){
		Hud_Set(hp_primary, chopHD);
	} else {
		Hud_Reset(hp_primary, chopHD);
	}
}
void Tree::StopHover(){
	Hud_Reset(hp_info, infoHD);
	Hud_Reset(hp_primary, chopHD);
}

void Tree::LeftClick() {
	if(!grown) return;
	Item holding = player->GetHolding();
	if(holding.type != it_axe || !holding.stackSize){
		App_DisplayWarning(w_wrong_tool);
		return;
	}
	if(holding.stackSize != 1){
		App_DisplayWarning(w_wrong_tool_number);
		return;
	}
	DropItemWithoutIntersect({it_log, 1}, position);
	grown = false;
	SetGrowTimer();
	ChangeParent(ip_treeStump);
	if(hoveringId == GetParentId()) Hud_Reset(hp_primary, chopHD);
}
