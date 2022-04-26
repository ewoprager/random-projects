#include "Wheelbarrow.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Hud.hpp"

extern Player *player;

extern int buildingHoverTextHDs[];

extern const ItemData itemData[ITEMS_N];

const vec3 wbStackRelativePositions[WB_MAX_STACKS] = {
	{	0.0f,	0.2f,	0.0f},
	{	1.5f,	0.2f,	0.0f},
	{	-1.5f,	0.2f,	0.0f},
	{	1.5f,	0.8f,	1.0f},
	{	-1.5f,	0.8f,	1.0f},
	{	1.5f,	0.8f,	-1.0f},
	{	-1.5f,	0.8f,	-1.0f},
	{	0.0f,	1.4f,	0.0f},
	{	1.5f,	1.4f,	0.0f},
	{	-1.5f,	1.4f,	0.0f}
};

Wheelbarrow::Wheelbarrow(float _yaw, vec2 _position) : Building(bu_wheelbarrow, true, _position){
	yaw = _yaw;
	
	SetBaseMatrix(0.25f, PI);
	UpdateMatrices();
}

int Wheelbarrow::useHD, Wheelbarrow::stopHD, Wheelbarrow::insertHD;
void Wheelbarrow::Construct(){
	useHD = Hud_AddText(useText, hp_primary);
	stopHD = Hud_AddText(stopText, hp_primary);
	insertHD = Hud_AddText(insertText, hp_secondary);
}

void Wheelbarrow::UpdateMatrices(){
	float m[4][4];
	ResetWorldMatrix();
	M4x4_yRotation(-yaw, m);
	M4x4_PreMultiply(worldMatrix, m);
	TransformMatrixToWorldPosition(position, 0.25f, true);
	
	for(int i=0; i<stacksN; i++){
		M4x4_Identity(stacks[i]->worldMatrix);
		
		M4x4_Scaling(0.7f, m);
		M4x4_PreMultiply(stacks[i]->worldMatrix, m);
		
		M4x4_yRotation(stacks[i]->yaw, m);
		M4x4_PreMultiply(stacks[i]->worldMatrix, m);
		
		M4x4_Translation(wbStackRelativePositions[i], m);
		M4x4_PreMultiply(stacks[i]->worldMatrix, m);
		
		M4x4_PreMultiply(stacks[i]->worldMatrix, worldMatrix);
		
		stacks[i]->SetPos(position); // for the collision mask
	}
}
void Wheelbarrow::SetYaw(const float& _yaw){
	yaw = _yaw;
	UpdateMatrices();
}
void Wheelbarrow::MovePos(const vec2 &displacement){
	position += displacement;
	UpdateMatrices();
}
void Wheelbarrow::StartHover(){
	Hud_Set(hp_info, buildingHoverTextHDs[bht_wb_placeHolder]);
	Hud_Set(hp_secondary, insertHD);
	Hud_Set(hp_primary, player->GetWBPushing() == this ? stopHD : useHD);
}
void Wheelbarrow::StopHover(){
	Hud_Reset(hp_info, buildingHoverTextHDs[bht_wb_placeHolder]);
	Hud_Reset(hp_secondary, insertHD);
	Hud_Reset(hp_primary, stopHD);
	Hud_Reset(hp_primary, useHD);
}
void Wheelbarrow::LeftClick(){
	if(EventHandler_GetKeyDown(ib_modifier)){
		if(player->GetWBPushing() == this) Empty();
	} else {
		if(player->TryWheelbarrow(this)) Hud_Set(hp_primary, player->GetWBPushing() == this ? stopHD : useHD);
	}
}
void Wheelbarrow::RightClick(){
	if(stacksN >= WB_MAX_STACKS) return;
	if(!player->GetHolding().stackSize) return;
	stacks[stacksN] = new WBItemStack(this, {it_stick, 0}, RandomAngle(), wbStackRelativePositions[stacksN]);
	player->DropInto(stacks[stacksN]);
	stacksN++;
	UpdateMatrices();
}
void Wheelbarrow::StackRemove(WBItemStack *stack){
	int i;
	for(i=0; i<stacksN; i++) if(stacks[i] == stack) break;
	if(i >= stacksN){
		std::cout << "ERROR: Unknown stack removed." << std::endl;
		return;
	}
	for(int j=i; j<stacksN-1; j++) stacks[j] = stacks[j+1];
	stacksN--;
	UpdateMatrices();
}

void Wheelbarrow::Empty(){
	const vec2 dropPos = position + position - player->GetPos();
	for(int i=stacksN-1; i>=0; i--){
		DropItemWithoutIntersect(stacks[i]->GetItem(), dropPos);
		delete stacks[i];
	}
	stacksN = 0;
	UpdateMatrices();
}


WBItemStack::WBItemStack(Wheelbarrow *_owner, Item _item, float _yaw, vec3 _position) : ItemDrop(_item, _yaw, {_position.x, _position.z}, true){
	owner = _owner;
	yaw = _yaw;
}
WBItemStack::~WBItemStack(){
	owner->StackRemove(this);
}

