#ifndef ThrownSpear_hpp
#define ThrownSpear_hpp

#include "Header.hpp"
#include "Protocols.hpp"

class ThrownSpear : public RenderedInstanced, public Updated {
public:
	ThrownSpear(vec3 _position, double yaw, double pitch);
	
	void Update(float deltaTime) override;
	
private:
	static constexpr float gravity = 300.0f;
	static constexpr float speed = 25.0f;
	vec2 position, velocity;
	float y;
	float timer;
	double yaw;
	float y0, grad, grav;
};

#endif /* ThrownSpear_hpp */
