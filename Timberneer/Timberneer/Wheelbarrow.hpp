#ifndef Wheelbarrow_hpp
#define Wheelbarrow_hpp

#include "Header.hpp"
#include "Building.hpp"
#include "ItemDrop.hpp"
#include "Item.hpp"

#define WB_MAX_STACKS 10

class WBItemStack : public ItemDrop {
public:
	WBItemStack(Wheelbarrow *_owner, Item _item, float _yaw, vec3 _position);
	~WBItemStack();
	
	float yaw;
	
private:
	Wheelbarrow *owner;
};

class Wheelbarrow : public Building {
public:
	Wheelbarrow(float _yaw, vec2 _position);
	
	static void Construct();
	
	void UpdateMatrices();
	
	void StartHover() override;
	void StopHover() override;
	void LeftClick() override;
	void RightClick() override;
	
	void SetYaw(const float& _yaw);
	void MovePos(const vec2& displacement);
	
	void StackRemove(WBItemStack *stack);
	
	float yaw;
	
private:
	int stacksN = 0;
	WBItemStack *stacks[WB_MAX_STACKS];
	
	void Empty();
	
	static const char constexpr *useText = "Use";
	static const char constexpr *stopText = "Put down; modifier: empty";
	static const char constexpr *insertText = "Insert item";
	static int useHD, stopHD, insertHD;
};

#endif /* Wheelbarrow_hpp */
