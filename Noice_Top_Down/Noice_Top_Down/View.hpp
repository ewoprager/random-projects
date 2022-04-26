#ifndef View_hpp
#define View_hpp

#include "Game.hpp"
#include "Player.hpp"
#include "UpdateRenderParent.hpp"

class View : public UpdateRenderParent {
public:
	View(Game *game);
	~View();
	
	void Update() override;

	void Shake(float amount){
		shake += amount;
	}
	
	Vector2D GetIntendedPosition(){
		return intendedPosition;
	}
	
private:
	float followSpd = 0.4;
	float shake = 0;
	Vector2D intendedPosition = Vector2D();
	
	Vector2D CalcShake();
	float shakeMin = 1;
	float shakeMax = 60;
	float shakeDamp = 1.3;
};

#endif /* View_hpp */
