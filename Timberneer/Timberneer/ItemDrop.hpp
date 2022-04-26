#ifndef ItemDrop_hpp
#define ItemDrop_hpp

#include "Header.hpp"
#include "Protocols.hpp"
#include "Item.hpp"

class ItemDrop : public RenderedInstanced, public Collider {
public:
	ItemDrop(Item _item, float yaw, vec2 pos, bool _maxStackLimited=false);
	virtual ~ItemDrop();
	
	static void Construct();
	
	void StartHover() override;
	void StopHover() override;
	void LeftClick() override;
	void RightClick() override;
	
	Item GetItem(){ return item; }
	int TryRemove(int num);
	int TryAdd(int num);
	void Change(Item to, bool deleteIfEmpty=true);
	
	int TrySize(const int& stackSize);
	void UpdateSize();
	
protected:
	bool maxStackLimited;
	
	Item item;
	
	bool hovering = false;
	static int numberHD, numberHDnumber, interactPrimaryHD, interactSecondaryHD;
	static const char constexpr *interactPrimaryText = "Pick up";
	static const char constexpr *interactSecondaryText = "Drop into";
	void UpdateHoverHud();
};

#endif /* ItemDrop_hpp */
