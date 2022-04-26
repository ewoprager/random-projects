#include "ThrownSpear.hpp"
#include "ItemDrop.hpp"
#include "World.hpp"

float Private_Motion(float y0, float grad, float grav, float time){
	time *= 4.0f;
	return y0 + grad*time - grav*time*time;
}

ThrownSpear::ThrownSpear(vec3 _position, double _yaw, double pitch) : RenderedInstanced(MAIN_IP_N + it_spear, false){
	position = {_position.x, _position.z};
	yaw = _yaw;
	
	SetBaseMatrix(0.25f, yaw);
	
	float horizontalSpeed = speed * cos(pitch);
	
	y0 = _position.y;
	grad = tan(pitch);
	grav = gravity / (2.0f * horizontalSpeed*horizontalSpeed);
	
	velocity = horizontalSpeed * (vec2){(float)sin(yaw), -(float)cos(yaw)};
	
	timer = 0.0f;
}
void ThrownSpear::Update(float deltaTime){
	timer += deltaTime;
	position += deltaTime * velocity;
	y = Private_Motion(y0, grad, grav, timer);
	
	float m[4][4];
	ResetWorldMatrix();
	M4x4_Translation({position.x, y, position.y}, m);
	M4x4_PreMultiply(worldMatrix, m);
	
	if(y <= World_GetGroundHeight(position)){
		new ItemDrop({it_spear, 1}, yaw, position, false);
		delete this;
	}
}
