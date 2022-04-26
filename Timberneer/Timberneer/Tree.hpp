#ifndef Tree_hpp
#define Tree_hpp

#include "Header.hpp"
#include "Protocols.hpp"

class Tree : public RenderedInstanced, public Collider, public Updated {
public:
	Tree(float yaw, vec2 _position);
	
	static void Construct();
	
	void Update(float deltaTime) override;
	
	void StartHover() override;
	void StopHover() override;
	void LeftClick() override;
	
private:
	float growTimer = 0.0f;
	bool grown = true;
	void SetGrowTimer(){
		growTimer = 180.0f + (float)(rand() % 120);
	}
	
	float stickTimer;
	void SetStickTimer(){
		stickTimer = 20.0f + (float)(rand() % 180);
	}
	
	static const char constexpr *infoText = "Tree";
	static const char constexpr *chopText = "Chop down";
	static int infoHD, chopHD;
};


#endif /* Tree_hpp */
