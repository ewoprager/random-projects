#include "ItemDrop.hpp"
#include "App.hpp"
#include "InstancedParent.hpp"
#include "Player.hpp"
#include "Hud.hpp"
#include "CollisionHandler.hpp"
#include "World.hpp"

extern Player *player;

extern const ItemData itemData[];

//extern const Align hudFixedHAlign[HUD_FIXED_POSITIONS];
//extern const Align hudFixedVAlign[HUD_FIXED_POSITIONS];
extern int itemTextHDs[];

const char *itemDropSrcs[ITEMS_N] = {
	"./usr/share/Timberneer/assets/Data/stick.bin",
	"./usr/share/Timberneer/assets/Data/green_cube.bin",
	"./usr/share/Timberneer/assets/Data/brown_cube.bin",
	"./usr/share/Timberneer/assets/Data/log.bin",
	"./usr/share/Timberneer/assets/Data/axe.bin",
	"./usr/share/Timberneer/assets/Data/bucket.bin",
	"./usr/share/Timberneer/assets/Data/waterBucket.bin", // normal water bucket
	"./usr/share/Timberneer/assets/Data/waterBucket.bin", // purified water bucket
	"./usr/share/Timberneer/assets/Data/rock.bin",
	"./usr/share/Timberneer/assets/Data/plank.bin",
	"./usr/share/Timberneer/assets/Data/heart.bin"
};

extern const uint8_t layers_itemDrop_in;
extern const uint8_t layers_usedBy_itemDrop;
extern const float radius_itemDrop;

ItemDrop::ItemDrop(Item _item, float yaw, vec2 _position, bool _maxStackLimited) : RenderedInstanced(MAIN_IP_N + _item.type, true), Collider(layers_itemDrop_in, radius_itemDrop, _position) {
	item = _item;
	
	SetBaseMatrix(0.5f, yaw);
	TransformMatrixToWorldPosition(_position, 0.0f, true);
	
	maxStackLimited = _maxStackLimited;
}
ItemDrop::~ItemDrop(){
	if(hovering) StopHover();
}

int ItemDrop::numberHD, ItemDrop::numberHDnumber, ItemDrop::interactPrimaryHD, ItemDrop::interactSecondaryHD;
void ItemDrop::Construct(){
	numberHDnumber = 1;
	numberHD = Hud_AddText("1", hp_infoNo);
	interactPrimaryHD = Hud_AddText(interactPrimaryText, hp_primary);
	interactSecondaryHD = Hud_AddText(interactSecondaryText, hp_secondary);
}

void ItemDrop::StartHover(){
	if(hovering) return;
	hovering = true;
	UpdateHoverHud();
}
void ItemDrop::StopHover(){
	if(!hovering) return;
	hovering = false;
	UpdateHoverHud();
}
void ItemDrop::LeftClick(){
	player->PickUpFrom(this);
}
void ItemDrop::RightClick(){
	player->DropInto(this);
}

int ItemDrop::TryRemove(int num){
	if(item.stackSize > num){
		item.stackSize -= num;
		UpdateSize();
		UpdateHoverHud();
		return num;
	}
	int ret = item.stackSize;
	delete this;
	return ret;
}
int ItemDrop::TryAdd(int num){
	if(maxStackLimited){
		int ms = itemData[item.type].maxStack;
		if(item.stackSize + num <= ms){
			item.stackSize += num;
			UpdateHoverHud();
			return num;
		}
		int added = ms - item.stackSize;
		item.stackSize = ms;
		UpdateHoverHud();
		return added;
	} else {
		int oldSize = item.stackSize;
		item.stackSize = TrySize(oldSize + num);
		UpdateSize();
		UpdateHoverHud();
		return item.stackSize - oldSize;
	}
}
void ItemDrop::Change(Item to, bool deleteIfEmpty){
	ItemEnum from = item.type;
	item = to;
	bool changeType = (item.type != from);
	if(item.stackSize || !deleteIfEmpty){
		if(changeType){
			ChangeParent(MAIN_IP_N + item.type);
		}
		UpdateSize();
		UpdateHoverHud();
	} else if(deleteIfEmpty){
		delete this;
	}
}

void ItemDrop::UpdateHoverHud(){
	bool show = (hovering && item.stackSize);
	if(show){
		Hud_Set(hp_info, itemTextHDs[item.type]);
		Hud_Set(hp_primary, interactPrimaryHD);
		Hud_Set(hp_secondary, interactSecondaryHD);
		if(item.stackSize == numberHDnumber){
			Hud_Set(hp_infoNo, numberHD);
			return;
		}
		numberHDnumber = item.stackSize;
		char str[10];
		sprintf(str, "%d", numberHDnumber);
		Hud_ReplaceText(numberHD, str, hp_infoNo, true);
		return;
	}
	Hud_Reset(hp_info, itemTextHDs[item.type]);
	Hud_Reset(hp_primary, interactPrimaryHD);
	Hud_Reset(hp_secondary, interactSecondaryHD);
	Hud_Reset(hp_infoNo, numberHD);
	return;
}
int ItemDrop::TrySize(const int& stackSize){
	if(!stackSize) return stackSize;
	int currentLevel = 1 + ((item.stackSize - 1) / itemData[item.type].maxStack);
	int level = 1 + ((stackSize - 1) / itemData[item.type].maxStack);
	while(CollisionHandler_CheckPos({position, sqrt(GetRad()*GetRad() * (float)level), layers_usedBy_itemDrop, GetColId()})){
		level--;
		if(level < currentLevel){
			App_DisplayWarning(w_no_pile_room);
			return item.stackSize;
		}
	}
	int ret = level*itemData[item.type].maxStack;
	if(ret < stackSize) App_DisplayWarning(w_no_pile_room);
	else if(ret > stackSize) ret = stackSize;
	return ret;
}
void ItemDrop::UpdateSize(){
	float oldRadius = GetRad();
	if(item.stackSize < itemData[item.type].maxStack){
		SetRad(radius_itemDrop);
	} else {
		SetSqRad(radius_itemDrop*radius_itemDrop * (float)(1 + ((item.stackSize - 1) / itemData[item.type].maxStack)));
	}
	M4x4_ScaleExcludingTranslation(worldMatrix, GetRad() / oldRadius);
}
