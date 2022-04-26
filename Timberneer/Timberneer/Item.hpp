#ifndef Item_hpp
#define Item_hpp

#include "Header.hpp"
#include "Protocols.hpp"

#define ITEMS_N 11
enum ItemEnum {it_stick, it_leaves_and_fibres, it_spear, it_log, it_axe, it_bucket, it_waterBucket, it_pureWaterBucket, it_rock, it_plank, it_heart};

#define CRAFT_RECIPES_N 4
#define MAX_CRAFT_HUD_DATAS 64

struct ItemData {
	int maxStack;
	int maxToolStack;
};

struct Item {
	ItemEnum type;
	int stackSize;
};

struct ItemRecipe {
	int stacks[ITEMS_N];
};

class ItemDrop;
struct ItemCollection {
	ItemRecipe recipe;
	ItemDrop *drops[MAX_OBJECTS];
	int dropsN;
	void Add(Item item){
		recipe.stacks[item.type] += item.stackSize;
	}
};

struct CraftRecipe {
	CraftRecipe(ItemRecipe _input, Item _output){
		input = _input;
		output = _output;
	}
	bool InputSatisfies(ItemRecipe testInput) const {
		for(int i=0; i<ITEMS_N; i++) if(testInput.stacks[i] < input.stacks[i]) return false;
		return true;
	}
	void Execute(ItemCollection collection, Item& holding, Item *beltItems) const;
	
	bool NeedOrMake(ItemEnum type) const {
		if(input.stacks[(int)type]) return true;
		if(type == output.type) return true;
		return false;
	}
	
	ItemRecipe input;
	Item output;
};

void Items_Construct();

ItemCollection FindNearbyItems(const vec2& pos, const float& sqDist);
void DropItemWithoutIntersect(Item item, const vec2& pos);

#endif /* Item_hpp */
