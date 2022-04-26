#include "Item.hpp"
#include "App.hpp"
#include "InstancedParent.hpp"
#include "Player.hpp"
#include "CollisionHandler.hpp"
#include "ItemDrop.hpp"
#include "Hud.hpp"

extern Player *player;
extern InstancedParent *instancedParents[];

extern const float radius_itemDrop;
extern const uint8_t layers_usedBy_itemDrop;

//extern const Align hudFixedHAlign[HUD_FIXED_POSITIONS];
//extern const Align hudFixedVAlign[HUD_FIXED_POSITIONS];
const char *itemTextText[ITEMS_N] = {"Stick", "Leaves and fibres", "Spear", "Log", "Axe", "Bucket", "Bucket of water", "Bucket of pure water", "Rock", "Wooden plank", "Glowing heart"};
int itemTextHDs[ITEMS_N];
extern const ItemData itemData[ITEMS_N] = {
	{3, 0},	// stick
	{8, 0},	// leaves & fibres
	{3, 1},	// spear
	{1, 0},	// log
	{3, 1},	// axe
	{1, 1},	// bucket
	{1, 1},	// water bucket
	{1, 1},	// pure water bucket
	{3, 0},	// rock
	{4, 0},	// plank
	{1, 0}	// heart
};
// recipes cannot make more than a max-stack-size of the output item
extern const CraftRecipe craftRecipes[CRAFT_RECIPES_N] = {
	CraftRecipe({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {it_spear, 1}),
	CraftRecipe({1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, {it_axe, 1}),
	CraftRecipe({0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, {it_plank, 2}),
	CraftRecipe({0, 2, 0, 0, 0, 0, 0, 0, 0, 4, 0}, {it_bucket, 1})
};

void Items_Construct(){
	for(int i=0; i<ITEMS_N; i++) itemTextHDs[i] = Hud_AddText(itemTextText[i], hp_info);
}

ItemCollection FindNearbyItems(const vec2& pos, const float& sqDist){
	ItemCollection ret;
	for(int i=0; i<ITEMS_N; i++) ret.recipe.stacks[i] = 0;
	ret.dropsN = 0;
	for(int p=0; p<ITEMS_N; p++){
		InstancedParent *itemParent = instancedParents[MAIN_IP_N + p];
		for(int i=0; i<itemParent->GetN(); i++){
			if(!(*itemParent)[i]->active) continue;
			ItemDrop *itemDrop = dynamic_cast<ItemDrop *>((*itemParent)[i]);
			if(!itemDrop) continue;
			if((pos - itemDrop->GetPos()).SqMag() > sqDist) continue;
			ret.recipe.stacks[itemDrop->GetItem().type] += itemDrop->GetItem().stackSize;
			ret.drops[ret.dropsN++] = itemDrop;
		}
	}
	return ret;
}

void DropItemWithoutIntersect(Item item, const vec2& pos){
	float r = 0.0f;
	const float deltaR = 0.125f;
	float theta = 0.0f;
	const float deltaTheta = 3.883222f;
	vec2 _pos;
	while(true){
		_pos = pos + r * (vec2){cos(theta), sin(theta)};
		if(!CollisionHandler_CheckPos({_pos, radius_itemDrop, layers_usedBy_itemDrop, -1})){
			new ItemDrop(item, 0.0f, _pos);
			return;
		}
		r += deltaR;
		theta += deltaTheta;
	}
}

void CraftRecipe::Execute(ItemCollection collection, Item& holding, Item *beltItems) const {
	ItemRecipe remainingInput = input;
	// taking from hand for input first
	if(holding.stackSize){
		if(holding.stackSize > remainingInput.stacks[holding.type]){
			holding.stackSize -= remainingInput.stacks[holding.type];
			remainingInput.stacks[holding.type] = 0;
		} else {
			remainingInput.stacks[holding.type] -= holding.stackSize;
			holding.stackSize = 0;
		}
	}
	// taking from ground for input
	for(int o=0; o<MAX_OBJECTS; o++){
		for(int i=0; i<ITEMS_N; i++){
			if(!remainingInput.stacks[i]) continue;
			if(!collection.drops[o]) continue;
			if(!collection.drops[o]->GetItem().stackSize) continue;
			if(collection.drops[o]->GetItem().type != i) continue;
			int gain = collection.drops[o]->TryRemove(remainingInput.stacks[i]);
			remainingInput.stacks[i] -= gain;
		}
	}
	// taking from belt for input
	for(int s=0; s<Player::beltSlotsN; s++){
		if(!beltItems[s].stackSize) continue;
		if(beltItems[s].stackSize > remainingInput.stacks[beltItems[s].type]){
			beltItems[s].stackSize -= remainingInput.stacks[beltItems[s].type];
			remainingInput.stacks[beltItems[s].type] = 0;
		} else {
			remainingInput.stacks[beltItems[s].type] -= beltItems[s].stackSize;
			beltItems[s].stackSize = 0;
		}
	}
	
	// could check just in case that remainingInput is now empty
	// outputting
	if(!holding.stackSize){
		holding = output;
	} else {
		int ms = itemData[output.type].maxStack;
		int remainingOutput = output.stackSize;
		
		if(holding.type == output.type){
			if(ms - holding.stackSize >= remainingOutput){
				holding.stackSize += remainingOutput;
				remainingOutput = 0;
			} else {
				remainingOutput -= ms - holding.stackSize;
				holding.stackSize = ms;
			}
		}
		if(remainingOutput) DropItemWithoutIntersect({output.type, remainingOutput}, player->GetPos());
	}
}
