#ifndef Bush_hpp
#define Bush_hpp

#include "Header.hpp"
#include "Protocols.hpp"

class Bush : public RenderedInstanced, public Collider, public Updated {
public:
	Bush(float yaw, vec2 _position);
	
	static void Construct();
	
	void Update(float deltaTime) override;
	
	void StartHover() override;
	void StopHover() override;
	void LeftClick() override;
	
private:
	float growTimer;
	bool grown = false;
	void SetGrowTimer(){
		growTimer = 60.0f + (float)(rand() % 120);
	}
	
	static const char constexpr *infoText = "Bush";
	static const char constexpr *pickText = "Pick";
	static int infoHD, pickHD;
};

#endif /* Bush_hpp */
