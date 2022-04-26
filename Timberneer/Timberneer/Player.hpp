#ifndef Player_hpp
#define Player_hpp

#include "App.hpp"
#include "Protocols.hpp"
#include "Item.hpp"
#include "ItemDrop.hpp"
#include "Hud.hpp"
#include "Building.hpp"
#include "EventHandler.hpp"

class PlacingRendered;

class Player : public Updated, public Collider {
public:
	Player(vec2 _position);
	~Player();
	
	void Update(float deltaTime) override;
	
	void CameraTransform(float out[4][4]);
	
	void InputCraft(SDL_Event event);
	void InputBuild(SDL_Event event);
	void Input1(SDL_Event event);
	void Input2(SDL_Event event);
	void Input3(SDL_Event event);
	void Input4(SDL_Event event);
	void Input5(SDL_Event event);
	void InputMouseMoved(SDL_Event event);
	void InputPrimary(SDL_Event event);
	void InputSecondary(SDL_Event event);
	
	void PickUpFrom(ItemDrop *drop);
	void DropInto(ItemDrop *drop);
	int Give(Item item);
	bool Take(Item item);
	
	Item GetHolding(){ return holding; }
	
	bool CanBuild(BuildingEnum buildingId);
	void AttemptBuild();
	
	bool TryWheelbarrow(Wheelbarrow *wb);
	Wheelbarrow *GetWBPushing() const { return wbPushing; }
	
	static const int beltSlotsN = 4;
	
private:
	float transformMatrix[4][4];
	
	// holding
	const float dropDistance = 1.0f;
	Item holding = {it_stick, 0};
	RenderedInstanced *holdingRendered;
	// holding hud
	void UpdateHand();
	int handNumberHDnumber;
	int handNumberHD;
	float handTransform[4][4];
	
	// crafting
	SwitchedBool<Player> craftMenuOpen = SwitchedBool<Player>(this, &Player::UpdateCraftMenu, false);
	void UpdateCraftMenu(bool newVal);
	void AttemptCraft(int recipeId);
	const float itemGrabDistance = 7.0f;
	
	// building
	SwitchedBool<Player> buildMenuOpen = SwitchedBool<Player>(this, &Player::UpdateBuildMenu, false);
	void UpdateBuildMenu(bool newVal);
	SwitchedBool<Player> placing = SwitchedBool<Player>(this, &Player::UpdatePlacing, false);
	void UpdatePlacing(bool newVal);
	void BeginBuild(BuildingEnum buildingId);
	PlacingRendered *placingRendered;
	BuildingEnum placingBuilding;
	vec2 placingPos;
	
	// belt
	void ToggleBeltSlot(int slotNumber);
	Item beltItems[beltSlotsN] = {{it_axe, 0}, {it_axe, 0}, {it_axe, 0}, {it_axe, 0}};
	// belt hud
	void UpdateBelt();
	int beltHD[beltSlotsN];
	ItemEnum beltHDtype[beltSlotsN];
	int beltNumberHD[beltSlotsN];
	int beltNumberHDnumber[beltSlotsN];
	
	// wheel barrow
	Wheelbarrow *wbPushing = nullptr;
	
	double yaw, pitch;
	float cy, sy;
	float y;
	
	const float speed = 5.0f;
	
	// non-customisable input event listener ids
	int _1PressKeyId, _2PressKeyId, _3PressKeyId, _4PressKeyId, _5PressKeyId, mouseMoveId;
	// customisable input event listener ids
	int inputIds[INPUTS_N];
	
	int cursorX, cursorY;
};

class PlacingRendered : public Rendered {
public:
	PlacingRendered();
	~PlacingRendered();
	
	void StartHover() override;
	void StopHover() override;
	void LeftClick() override;
	
	SwitchedBool<PlacingRendered> possible = SwitchedBool<PlacingRendered>(this, &PlacingRendered::UpdatePossible, true);
	
private:
	void UpdatePossible(bool newVal);
	ObjectData possibleData = ReadProcessedOBJFile("./usr/share/Timberneer/assets/Data/placingBuilding.bin");
	ObjectData impossibleData = ReadProcessedOBJFile("./usr/share/Timberneer/assets/Data/placingImpossible.bin");
	
	int hudData;
	//vec3 hudPos = {0.0f, 0.0f, HUD_TEXT_DEPTH};
};

#endif /* Player_hpp */
