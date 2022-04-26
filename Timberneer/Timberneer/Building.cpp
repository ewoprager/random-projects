#include "Building.hpp"
#include "World.hpp"
#include "ItemDrop.hpp"
#include "Player.hpp"
#include "Graveyard.hpp"

extern Player *player;

extern int hoveringId;

extern int graveyardsN;
extern Graveyard *graveyardArray[];

static const char *buildingHoverText[BUILDING_HOVER_TEXT_N] = {
	"Insert unpurified water",
	"Water purifier: purifying",
	"Take purified water",
	"Wheelbarrow"
};
int buildingHoverTextHDs[BUILDING_HOVER_TEXT_N];

extern const float layers_buildingIn;

const char *buildingSrcs[BUILDINGS_N] = {
	"./usr/share/Timberneer/assets/Data/wheelbarrow.bin",
	"./usr/share/Timberneer/assets/Data/waterPurifier.bin"
};
extern const BuildingData buildingData[BUILDINGS_N] = {
	{{2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0}, 0.3f, 0x1F, 0x01},	// wheelbarrow
	{{4, 4, 0, 0, 0, 1, 0, 0, 1, 0, 0}, 0.3f, 0x1F, 0x08}	// water purifier
};

int staticBuildingsN = 0;
StaticBuilding *staticBuildingArray[MAX_BUILDINGS];

void Building_Construct(){
	for(int i=0; i<BUILDING_HOVER_TEXT_N; i++) buildingHoverTextHDs[i] = Hud_AddText(buildingHoverText[i], hp_info);
}

bool Building_InputSatisfies(BuildingEnum building, ItemRecipe testInput){
	ItemRecipe input = buildingData[building].buildRecipe;
	for(int i=0; i<ITEMS_N; i++) if(testInput.stacks[i] < input.stacks[i]) return false;
	return true;
}
void Building_Execute(BuildingEnum building, ItemCollection collection, Item& holding, Item *beltItems){
	ItemRecipe remainingInput = buildingData[building].buildRecipe;
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
}

Building::Building(BuildingEnum type, bool interactable, vec2 _position) : RenderedInstanced(MAIN_IP_N + ITEMS_N + type, interactable), Collider(buildingData[type].layersIn, buildingData[type].radius, _position) {
	
}
StaticBuilding::StaticBuilding(BuildingEnum type, bool interactable, float yaw, vec2 _position) : Building(type, interactable, _position){
	float m[4][4];
	M4x4_Identity(worldMatrix);
	
	M4x4_Scaling(0.5f, m);
	M4x4_PreMultiply(worldMatrix, m);
	
	M4x4_yRotation(-yaw, m);
	M4x4_PreMultiply(worldMatrix, m);
	
	M4x4_Translation({position.x, World_GetGroundHeight(position), position.y}, m);
	M4x4_PreMultiply(worldMatrix, m);
	
	staticBuildingArray[staticBuildingsN++] = this;
	for(int i=0; i<graveyardsN; i++) graveyardArray[i]->UpdateAnger();
}
StaticBuilding::~StaticBuilding(){
	int i;
	for(i=0; i<staticBuildingsN; i++){
		if(staticBuildingArray[i] == this){
			for(int j=i; j<staticBuildingsN-1; j++){
				staticBuildingArray[j] = staticBuildingArray[j+1];
			}
			break;
		}
	}
	if(i < staticBuildingsN){
		staticBuildingsN--;
	} else {
		std::cout << "ERROR: Unknown static building deleted." << std::endl;
	}
	for(int i=0; i<graveyardsN; i++) graveyardArray[i]->UpdateAnger();
}

WaterPurifier::WaterPurifier(float yaw, vec2 _position) : StaticBuilding(bu_waterPurifer, true, yaw, _position) {
	
}

int WaterPurifier::HDs[3];
void WaterPurifier::Construct(){
	HDs[s_empty] = buildingHoverTextHDs[bht_wp_insertUnpurifiedWater];
	HDs[s_purifying] = buildingHoverTextHDs[bht_wp_purifying];
	HDs[s_done] = buildingHoverTextHDs[bht_wp_takePurifiedWater];
}

void WaterPurifier::Update(float deltaTime){
	if(state != s_purifying) return;
	purifyingTimer -= deltaTime;
	if(purifyingTimer <= 0.0f){
		state = s_done;
		if(hoveringId == GetParentId()) StartHover();
	}
}
void WaterPurifier::LeftClick(){
	RightClick();
}
void WaterPurifier::RightClick(){
	if(state == s_purifying) return;
	if(state == s_empty){
		if(player->Take({it_waterBucket, 1})){
			purifyingTimer = purifyingTime;
			state = s_purifying;
			StartHover();
		} else {
			App_DisplayWarning(w_insufficient_items);
		}
	} else { // state is s_done
		if(!player->Give({it_pureWaterBucket, 1})) DropItemWithoutIntersect({it_pureWaterBucket, 1}, position);
		state = s_empty;
		StartHover();
	}
}
void WaterPurifier::StartHover(){
	Hud_Set(hp_primary, HDs[state]);
}
void WaterPurifier::StopHover(){
	for(int i=0; i<3; i++) Hud_Reset(hp_primary, HDs[i]);
}
