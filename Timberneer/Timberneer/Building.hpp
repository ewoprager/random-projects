#ifndef Building_hpp
#define Building_hpp

#include "Header.hpp"
#include "Item.hpp"
#include "Protocols.hpp"

#define MAX_BUILDINGS 128

#define BUILDINGS_N 2
enum BuildingEnum {bu_wheelbarrow, bu_waterPurifer};

struct BuildingData {
	ItemRecipe buildRecipe;
	float radius;
	uint8_t layersIn; // layers this infringes upon
	uint8_t layersUse; // layers this can't be placed on
	//bool operated;
	//bool powered;
};

#define BUILDING_HOVER_TEXT_N 4
enum BHTEnum {bht_wp_insertUnpurifiedWater, bht_wp_purifying, bht_wp_takePurifiedWater, bht_wb_placeHolder};

void Building_Construct();

bool Building_InputSatisfies(BuildingEnum building, ItemRecipe testInput);
void Building_Execute(BuildingEnum building, ItemCollection collection, Item& holding, Item *beltItems);

class Building : public RenderedInstanced, public Collider {
public:
	Building(BuildingEnum type, bool interactable, vec2 _position);
};
class StaticBuilding : public Building {
public:
	StaticBuilding(BuildingEnum type, bool interactable, float yaw, vec2 _position);
	virtual ~StaticBuilding();
};

class WaterPurifier : public StaticBuilding, public Updated {
public:
	WaterPurifier(float yaw, vec2 _position);
	
	static void Construct();
	
	void LeftClick() override;
	void RightClick() override;
	void StartHover() override;
	void StopHover() override;
	
	void Update(float deltaTime) override;
	
private:
	static constexpr float purifyingTime = 10.0f;
	enum State {
		s_empty,
		s_purifying,
		s_done
	};
	State state = s_empty;
	static int HDs[3];
	float purifyingTimer = 1.0f;
};

#endif /* Building_hpp */
