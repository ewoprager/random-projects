#ifndef Cube_hpp
#define Cube_hpp

#include "App.hpp"
#include "Protocols.hpp"
#include "InstancedParent.hpp"

class Cube : public RenderedInstanced {
public:
	Cube(float yaw, vec2 _position) : RenderedInstanced(ip_cube, true) {
		SetBaseMatrix(0.5f, yaw);
		TransformMatrixToWorldPosition(_position, 0.0f);
	}
	
	void StartHover() override {
		//std::cout << "Hovering over a cube." << std::endl;
	}
};

#endif /* Cube_hpp */
