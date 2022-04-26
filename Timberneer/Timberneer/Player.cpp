#include "Player.hpp"
#include "World.hpp"
#include "CollisionHandler.hpp"
#include "Wheelbarrow.hpp"
#include "ThrownSpear.hpp"

extern int hoveringId;

extern SmartArray<MAX_OBJECTS, RenderedParent *> renderedParentArray;
extern const Align hudFixedHAlign[HUD_FIXED_POSITIONS];
extern const Align hudFixedVAlign[HUD_FIXED_POSITIONS];
extern const char *itemTextText[ITEMS_N];
static int craftMenuHD;
extern const CraftRecipe craftRecipes[CRAFT_RECIPES_N];
extern int craftMenuRecipeHDs[MAX_CRAFT_HUD_DATAS];
extern int cmhdN;
static int buildMenuHD;
extern const BuildingData buildingData[BUILDINGS_N];
extern int buildMenuRecipeHDs[MAX_CRAFT_HUD_DATAS];
extern int bmhdN;

extern const ItemData itemData[];
extern const uint8_t layers_player_in;
extern const uint8_t layers_usedBy_player;
extern const float radius_player;

extern InputBindingStruct inputBindings[INPUTS_N];
extern SDL_KeyCode keyDownInputBindings[KEYDOWNINPUTS_N];

template int EventHandler_Add<Player>(MemberFunction<Player, void, SDL_Event>, SDL_Event);

Player::Player(vec2 _position) : Updated(), Collider(layers_player_in, radius_player, _position) {
	yaw = pitch = 0.0f;
	cursorX = cursorY = 0;
	
	// hud
	// hand
	handNumberHDnumber = 1;
	handNumberHD = Hud_AddText("1", hp_holdingNo);
	// belt
	for(int i=0; i<beltSlotsN; i++){
		beltHDtype[i] = beltItems[i].type;
		beltHD[i] = Hud_AddText(itemTextText[beltHDtype[i]], hp_belt1 + 3*i);
		beltNumberHDnumber[i] = 1;
		beltNumberHD[i] = Hud_AddText("1", hp_beltNo1 + 3*i);
	}
	// craft menu
	craftMenuHD = Hud_Add(MakeHudDataFromTexture(LoadTexture("./usr/share/Timberneer/assets/Data/craftMenu.png"), hudFixedHAlign[hp_menu], hudFixedVAlign[hp_menu], 1.0f, 1.0f));
	/*for(int i=0; i<CRAFT_RECIPES_N; i++){
		craftMenuRecipeHDs[cmhdN++] = Hud_Add(MakeHudDataFromTexture(instancedParents[MAIN_IP_N + craftRecipes[i].output.type]->GetData().divisionData[0].texture, 0.1f, 0.1f));
		int n = 0;
		for(int j=0; j<ITEMS_N; j++){
			if(craftRecipes[i].input.stacks[j]){
				craftMenuRecipeHDs[cmhdN++] = Hud_Add(MakeHudDataFromTexture(instancedParents[MAIN_IP_N + j]->GetData().divisionData[0].texture, 0.1f, 0.1f));
				n++;
			}
		}
	}*/
	// buildmenu
	buildMenuHD = Hud_Add(MakeHudDataFromTexture(LoadTexture("./usr/share/Timberneer/assets/Data/buildMenu.png"), hudFixedHAlign[hp_menu], hudFixedVAlign[hp_menu], 1.0f, 1.0f));
	/*for(int i=0; i<BUILDINGS_N; i++){
		buildMenuRecipeHDs[bmhdN++] = Hud_Add({MakeHudDataFromTexture(instancedParents[MAIN_IP_N + ITEMS_N + i]->GetData().divisionData[0].texture, 0.1f, 0.1f), {-0.45f, 0.45f - 0.1f*(float)i, HUD_MENU_TEXT_DEPTH}});
		int n = 0;
		for(int j=0; j<ITEMS_N; j++){
			if(buildingData[i].buildRecipe.stacks[j]){
				buildMenuRecipeHDs[bmhdN++] = Hud_Add({MakeHudDataFromTexture(instancedParents[MAIN_IP_N + j]->GetData().divisionData[0].texture, 0.1f, 0.1f), {-0.2f + 0.1f*(float)n, 0.45f - 0.1f*(float)i, HUD_MENU_TEXT_DEPTH}});
				n++;
			}
		}
	}*/
	
	// input
	// key down inputs
	keyDownInputBindings[ib_forward] = SDLK_w;
	keyDownInputBindings[ib_backward] = SDLK_s;
	keyDownInputBindings[ib_left] = SDLK_a;
	keyDownInputBindings[ib_right] = SDLK_d;
	keyDownInputBindings[ib_modifier] = SDLK_LSHIFT;
	// event callback inputs
	SDL_Event event;
	event.type = SDL_KEYDOWN;
	event.key.keysym.sym = SDLK_e;
	inputBindings[ib_craft] = {event, &Player::InputCraft};
	event.key.keysym.sym = SDLK_r;
	inputBindings[ib_build] = {event, &Player::InputBuild};
	event.key.keysym.sym = SDLK_1;
	_1PressKeyId = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, &Player::Input1), event);
	event.key.keysym.sym = SDLK_2;
	_2PressKeyId = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, &Player::Input2), event);
	event.key.keysym.sym = SDLK_3;
	_3PressKeyId = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, &Player::Input3), event);
	event.key.keysym.sym = SDLK_4;
	_4PressKeyId = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, &Player::Input4), event);
	event.key.keysym.sym = SDLK_5;
	_5PressKeyId = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, &Player::Input5), event);
	event.type = SDL_MOUSEMOTION;
	mouseMoveId = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, &Player::InputMouseMoved), event);
	event.type = SDL_MOUSEBUTTONDOWN;
	event.button.button = SDL_BUTTON_LEFT;
	inputBindings[ib_interact_primary] = {event, &Player::InputPrimary};
	event.button.button = SDL_BUTTON_RIGHT;
	inputBindings[ib_interact_secondary] = {event, &Player::InputSecondary};
	for(int i=0; i<INPUTS_N; i++) inputIds[i] = EventHandler_Add(MemberFunction<Player, void, SDL_Event>(this, inputBindings[i].callBack), inputBindings[i].event);
	
	holdingRendered = new RenderedInstanced(0, false);
	holdingRendered->active = false;
	float transform[4][4];
	M4x4_Identity(handTransform);
	M4x4_Scaling(0.025f, transform);
	M4x4_PreMultiply(handTransform, transform);
	M4x4_Translation({0.075f, -0.075f, -0.25f}, transform);
	M4x4_PreMultiply(handTransform, transform);
	
	placingRendered = new PlacingRendered();
}
Player::~Player(){
	EventHandler_Remove(_1PressKeyId);
	EventHandler_Remove(_2PressKeyId);
	EventHandler_Remove(_3PressKeyId);
	EventHandler_Remove(_4PressKeyId);
	EventHandler_Remove(_5PressKeyId);
	EventHandler_Remove(mouseMoveId);
	for(int i=0; i<INPUTS_N; i++) EventHandler_Remove(inputIds[i]);
	
	delete holdingRendered;
	delete placingRendered;
}


void Player::CameraTransform(float out[4][4]){
	float transform[4][4];
	
	M4x4_Identity(transformMatrix);
	M4x4_xRotation(-pitch, transform); // camera pitch rotation
	M4x4_PreMultiply(transformMatrix, transform);
	M4x4_yRotation(-yaw, transform); // camera yaw rotation
	M4x4_PreMultiply(transformMatrix, transform);
	M4x4_Translation({position.x, y + 1.0f, position.y}, transform); // camera position
	M4x4_PreMultiply(transformMatrix, transform);
	
	if(holding.stackSize){
		memcpy(holdingRendered->worldMatrix, handTransform, 16 * sizeof(float));
		M4x4_PreMultiply(holdingRendered->worldMatrix, transformMatrix);
	}
	
	memcpy(out, transformMatrix, 16 * sizeof(float));
}


void Player::Update(float deltaTime){
	yaw = (float)cursorX * MOUSE_SENSITIVITY_X;
	cy = cos(yaw);
	sy = sin(yaw);
	pitch = (float)cursorY * MOUSE_SENSITIVITY_Y;
	if(pitch < -1.57f){
		pitch = -1.57f;
		cursorY = (int)(pitch / MOUSE_SENSITIVITY_Y);
	} else if(pitch > 1.57f){
		pitch = 1.57f;
		cursorY = (int)(pitch / MOUSE_SENSITIVITY_Y);
	}
	
	int right = EventHandler_GetKeyDown(ib_right) - EventHandler_GetKeyDown(ib_left);
	int forward = EventHandler_GetKeyDown(ib_forward) - EventHandler_GetKeyDown(ib_backward);
	if(right || forward){
		float thisSpeed = speed * deltaTime;
		if(right && forward) thisSpeed *= 0.707;
		if(wbPushing){
			static const float pushDist = radius_player + buildingData[bu_wheelbarrow].radius + 1.0f;
			vec2 d = wbPushing->GetPos() - position;
			float dist = sqrt(d.SqMag());
			vec2 dHat = d * 1.0f/dist;
			if(dist != pushDist){
				vec2 tryDisplacement = position + pushDist*dHat - wbPushing->GetPos();
				wbPushing->MovePos(CollisionHandler_MoveToCol({wbPushing->GetPos(), wbPushing->GetRad(), buildingData[bu_wheelbarrow].layersUse, wbPushing->GetColId()}, tryDisplacement, 0.05f));
			}
			if(right){
				vec2 tryDisplacement = -thisSpeed * right * V2_Crossed(dHat);
				float prevAngle = -dHat.Angle();
				position += CollisionHandler_MoveToCol({position, GetRad(), layers_usedBy_player, GetColId()}, tryDisplacement, 0.05f);
				dHat = V2_Normalised(wbPushing->GetPos() - position);
				float newAngle = -dHat.Angle();
				wbPushing->SetYaw(newAngle);
				cursorX += AngleDifference(prevAngle, newAngle) / MOUSE_SENSITIVITY_X;
			}
			if(forward){
				vec2 tryDisplacement = thisSpeed * forward * dHat;
				vec2 displacement = CollisionHandler_DoubleMoveToCol({position, GetRad(), layers_usedBy_player, GetColId()}, {wbPushing->GetPos(), wbPushing->GetRad(), buildingData[bu_wheelbarrow].layersUse, wbPushing->GetColId()}, tryDisplacement, 0.05f);
				position += displacement;
				wbPushing->MovePos(displacement);
			}
		} else {
			vec2 tryDisplacement = thisSpeed * (vec2){forward*sy + right*cy, - forward*cy + right*sy};
			position += CollisionHandler_MoveToCol({position, GetRad(), layers_usedBy_player, GetColId()}, tryDisplacement, 0.05f);
		}
		y = World_GetGroundHeight(position);
	}
	
	if(placing.Get()){
		float transform[4][4];
		M4x4_Identity(placingRendered->worldMatrix);
		M4x4_Scaling(0.5f, transform);
		M4x4_PreMultiply(placingRendered->worldMatrix, transform);
		M4x4_yRotation(-yaw, transform);
		M4x4_PreMultiply(placingRendered->worldMatrix, transform);
		float placeDistance = GetRad() + buildingData[placingBuilding].radius + 1.0f;
		placingPos = position + placeDistance*(vec2){sy, -cy};
		M4x4_Translation({placingPos.x, World_GetGroundHeight(placingPos) + 0.25f, placingPos.y}, transform);
		M4x4_PreMultiply(placingRendered->worldMatrix, transform);
		
		placingRendered->possible.Set(!CollisionHandler_CheckPos({placingPos, buildingData[placingBuilding].radius, buildingData[placingBuilding].layersUse, -1}));
	}
}


void Player::PickUpFrom(ItemDrop *drop){
	Item item = drop->GetItem();
	if(!holding.stackSize || (holding.type == item.type && holding.stackSize < itemData[holding.type].maxStack)){
		holding.type = item.type;
		holding.stackSize += drop->TryRemove(EventHandler_GetKeyDown(ib_modifier) ? 1 : itemData[item.type].maxStack - holding.stackSize);
		UpdateHand();
	} else if(item.stackSize <= itemData[item.type].maxStack && holding.type != item.type){
		drop->Change(holding);
		holding = item;
		UpdateHand();
	}
}
void Player::DropInto(ItemDrop *drop){
	if(!holding.stackSize) return;
	Item item = drop->GetItem();
	if(item.stackSize){
		if(item.type != holding.type) return;
	} else {
		drop->Change({holding.type, 0}, false);
	}
	holding.stackSize -= drop->TryAdd(EventHandler_GetKeyDown(ib_modifier) ? 1 : holding.stackSize);
	UpdateHand();
}

int Player::Give(Item item){
	if(holding.stackSize && item.type != holding.type) return 0;
	holding.type = item.type;
	int ms = itemData[holding.type].maxStack;
	if(holding.stackSize >= ms) return 0;
	holding.stackSize += item.stackSize;
	int ret = item.stackSize;
	if(holding.stackSize > ms){
		ret -= holding.stackSize - ms;
		holding.stackSize = ms;
	}
	UpdateHand();
	return ret;
}
bool Player::Take(Item item){
	if(!item.stackSize) return true;
	if(!holding.stackSize) return false;
	if(holding.type != item.type) return false;
	if(holding.stackSize < item.stackSize) return false;
	holding.stackSize -= item.stackSize;
	UpdateHand();
	return true;
}

void Player::InputCraft(SDL_Event event){
	if(buildMenuOpen.Get()) return;
	craftMenuOpen.Switch();
}
void Player::InputBuild(SDL_Event event){
	if(craftMenuOpen.Get()) return;
	if(placing.Get()) placing.Switch();
	buildMenuOpen.Switch();
}


void Player::Input1(SDL_Event event){ if(craftMenuOpen.Get()) AttemptCraft(0); else if(buildMenuOpen.Get()) BeginBuild((BuildingEnum)0); else ToggleBeltSlot(0); }
void Player::Input2(SDL_Event event){ if(craftMenuOpen.Get()) AttemptCraft(1); else if(buildMenuOpen.Get()) BeginBuild((BuildingEnum)1); else ToggleBeltSlot(1); }
void Player::Input3(SDL_Event event){ if(craftMenuOpen.Get()) AttemptCraft(2); else if(buildMenuOpen.Get()) BeginBuild((BuildingEnum)2); else ToggleBeltSlot(2); }
void Player::Input4(SDL_Event event){ if(craftMenuOpen.Get()) AttemptCraft(3); else if(buildMenuOpen.Get()) BeginBuild((BuildingEnum)3); else ToggleBeltSlot(3); }
void Player::Input5(SDL_Event event){ if(craftMenuOpen.Get()) AttemptCraft(4); else if(buildMenuOpen.Get()) BeginBuild((BuildingEnum)4); else ToggleBeltSlot(4); }

extern const CraftRecipe craftRecipes[];
void Player::AttemptCraft(int recipeId){
	if(recipeId < 0) return; if(recipeId >= CRAFT_RECIPES_N) return;
	if(holding.stackSize){
		if(!craftRecipes[recipeId].NeedOrMake(holding.type)){
			App_DisplayWarning(w_holding_wrong);
			return;
		}
	}
	ItemCollection nearby = FindNearbyItems(position, itemGrabDistance);
	nearby.Add(holding);
	for(int i=0; i<beltSlotsN; i++) nearby.Add(beltItems[i]);
	if(!craftRecipes[recipeId].InputSatisfies(nearby.recipe)){
		App_DisplayWarning(w_insufficient_items);
		return;
	}
	craftRecipes[recipeId].Execute(nearby, holding, beltItems);
	UpdateHand();
	UpdateBelt();
}

void Player::BeginBuild(BuildingEnum buildingId){
	buildMenuOpen.Set(false);
	if(wbPushing) return;
	placingBuilding = (BuildingEnum)buildingId;
	placing.Set(true);
}
extern const BuildingData buildingData[BUILDINGS_N];
bool Player::CanBuild(BuildingEnum buildingId){
	if(CollisionHandler_CheckPos({placingPos, buildingData[buildingId].radius, buildingData[buildingId].layersUse, -1})) return false;
	if(holding.stackSize){
		if(!buildingData[buildingId].buildRecipe.stacks[holding.type]){
			App_DisplayWarning(w_holding_wrong);
			return false;
		}
	}
	ItemCollection nearby = FindNearbyItems(position, itemGrabDistance);
	nearby.Add(holding);
	for(int i=0; i<beltSlotsN; i++) nearby.Add(beltItems[i]);
	if(!Building_InputSatisfies(buildingId, nearby.recipe)){
		App_DisplayWarning(w_insufficient_items);
		return false;
	}
	Building_Execute(buildingId, nearby, holding, beltItems);
	UpdateHand();
	UpdateBelt();
	return true;
}
void Player::AttemptBuild(){
	if(CanBuild(placingBuilding)){
		placing.Set(false);
		switch (placingBuilding) {
			case bu_waterPurifer: {
				new WaterPurifier(yaw, placingPos);
				break;
			}
			case bu_wheelbarrow: {
				new Wheelbarrow(yaw, placingPos);
				break;
			}
			default: std::cout << "ERROR: Unknown building crafted." << std::endl;
		}
	}
}

void Player::ToggleBeltSlot(int slotNumber){
	if(slotNumber < 0) return;
	if(slotNumber >= beltSlotsN) return;
	if(holding.stackSize){
		int mts = itemData[holding.type].maxToolStack;
		if(!mts) return;
		if(beltItems[slotNumber].stackSize){
			int ms = itemData[holding.type].maxStack;
			if(holding.stackSize >= ms) return;
			if(beltItems[slotNumber].type == holding.type){
				if(holding.stackSize + beltItems[slotNumber].stackSize > ms){
					beltItems[slotNumber].stackSize -= ms - holding.stackSize;
					holding.stackSize = ms;
				} else {
					holding.stackSize += beltItems[slotNumber].stackSize;
					beltItems[slotNumber].stackSize = 0;
				}
			} else {
				if(holding.stackSize > mts) return;
				Item temp = beltItems[slotNumber];
				beltItems[slotNumber] = holding;
				holding = temp;
			}
		} else {
			if(holding.stackSize > mts){
				beltItems[slotNumber].type = holding.type;
				beltItems[slotNumber].stackSize = mts;
				holding.stackSize -= mts;
			} else {
				beltItems[slotNumber] = holding;
				holding.stackSize = 0;
			}
		}
	} else {
		if(!beltItems[slotNumber].stackSize) return;
		holding = beltItems[slotNumber];
		beltItems[slotNumber].stackSize = 0;
	}
	UpdateHand();
	UpdateBelt();
}


void Player::InputMouseMoved(SDL_Event event){
	cursorX += event.motion.xrel;
	cursorY += event.motion.yrel;
}
void Player::InputPrimary(SDL_Event event){
	if(hoveringId == 0){ // nothing
		if(holding.stackSize != 1) return;
		if(holding.type != it_spear) return;
		new ThrownSpear({position.x, World_GetGroundHeight(position) + 1.5f, position.y}, yaw, -pitch);
		holding.stackSize = 0;
		UpdateHand();
		return;
	}
	if(hoveringId == (0xFF << 24)){ // water
		if(!holding.stackSize || holding.type != it_bucket){
			App_DisplayWarning(w_insufficient_items);
			return;
		}
		holding.type = it_waterBucket;
		UpdateHand();
		return;
	}
	RenderedParent *ptr = renderedParentArray(hoveringId);
	if(ptr) ptr->LeftClick();
}
void Player::InputSecondary(SDL_Event event){	
	RenderedParent *ptr = renderedParentArray(hoveringId);
	if(!ptr || hoveringId == 0){ // nothing or water
		if(!holding.stackSize) return;
		Item toDrop = holding;
		if(EventHandler_GetKeyDown(ib_modifier)) toDrop.stackSize = 1;
		DropItemWithoutIntersect(toDrop, position + dropDistance*(vec2){sy, -cy});
		holding.stackSize -= toDrop.stackSize;
		UpdateHand();
	} else { // something
		ptr->RightClick();
	}
}

void Player::UpdateHand(){
	if(!holding.stackSize){
		holdingRendered->active = false;
		if(handNumberHD < 0) return;
		Hud_Set(hp_holdingNo, -1);
		return;
	}
	holdingRendered->active = true;
	holdingRendered->ChangeParent(MAIN_IP_N + holding.type);
	
	if(handNumberHDnumber == holding.stackSize){
		Hud_Set(hp_holdingNo, handNumberHD);
		return;
	}
	handNumberHDnumber = holding.stackSize;
	char str[10];
	sprintf(str, "%d", handNumberHDnumber);
	Hud_ReplaceText(handNumberHD, str, hp_holdingNo, true);
}

void Player::UpdateBelt(){
	for(int i=0; i<beltSlotsN; i++){
		if(!beltItems[i].stackSize){
			Hud_Set(hp_beltNo1 + 3*i, -1);
			Hud_Set(hp_belt1 + 3*i, -1);
			continue;
		}
		
		if(beltHDtype[i] == beltItems[i].type){
			Hud_Set(hp_belt1 + 3*i, beltHD[i]);
		} else {
			beltHDtype[i] = beltItems[i].type;
			Hud_ReplaceText(beltHD[i], itemTextText[beltHDtype[i]], hp_belt1 + 3*i, true);
		}
		
		if(beltNumberHDnumber[i] == beltItems[i].stackSize){
			Hud_Set(hp_beltNo1 + 3*i, beltNumberHD[i]);
		} else {
			beltNumberHDnumber[i] = beltItems[i].stackSize;
			char str[10];
			sprintf(str, "%d", beltNumberHDnumber[i]);
			Hud_ReplaceText(beltNumberHD[i], str, hp_beltNo1 + 3*i, true);
		}
	}
}

void Player::UpdateCraftMenu(bool visible){
	if(visible){
		Hud_Set(hp_menu, craftMenuHD);
	} else {
		Hud_Reset(hp_menu, craftMenuHD);
	}
	//for(int i=0; i<cmhdN; i++) hudDataActive[craftMenuRecipeHDs[i]] = newVal;
}
void Player::UpdateBuildMenu(bool visible){
	if(visible){
		Hud_Set(hp_menu, buildMenuHD);
	} else {
		Hud_Reset(hp_menu, buildMenuHD);
	}
	//for(int i=0; i<bmhdN; i++) hudDataActive[buildMenuRecipeHDs[i]] = newVal;
}
void Player::UpdatePlacing(bool newVal){
	placingRendered->active = newVal;
}
bool Player::TryWheelbarrow(Wheelbarrow *wb){
	if(placing.Get()) return false;
	if(wbPushing){
		if(wbPushing != wb) return false;
		wbPushing = nullptr;
		return true;
	} else {
		wbPushing = wb;
		wbPushing->SetYaw(-(wbPushing->GetPos() - position).Angle());
		return true;
	}
}

extern Player *player;
PlacingRendered::PlacingRendered() : Rendered("./usr/share/Timberneer/assets/Data/placingBuilding.bin", true) {
	active = false;
	
	hudData = Hud_Add(MakeHudDataFromTexture(MakeTextTexture("Build here")));
}
PlacingRendered::~PlacingRendered(){}
void PlacingRendered::StartHover(){
	Hud_Set(hp_primary, hudData);
}
void PlacingRendered::StopHover(){
	Hud_Reset(hp_primary, hudData);
}
void PlacingRendered::LeftClick(){
	player->AttemptBuild();
}
void PlacingRendered::UpdatePossible(bool newVal){
	SetData(newVal ? possibleData : impossibleData);
}
