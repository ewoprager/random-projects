#include "Circle.hpp"

Circle::Circle(App* app, bool _fixed, float density, Vector2D initialPos, float _radius, float _mu, float _cor) : RigidbodyParent(app, _fixed, 1, initialPos, 0, _mu, _cor) {
	
	colour = {150, static_cast<unsigned short>(255*mu), static_cast<unsigned short>(255*cor)};
	
	shape = circle;
	
	radius = _radius;
	maxRadius = radius;
	
	mass = METRES_PER_PIXEL*METRES_PER_PIXEL* density * PI * radius * radius;
	
	smoa = METRES_PER_PIXEL*METRES_PER_PIXEL*METRES_PER_PIXEL*METRES_PER_PIXEL* PI * radius*radius*radius*radius / 2;
	
	theApp->AddToRender(this);
	updateId = theApp->AddToUpdate(this);
	handleId = theApp->collisionHandler->AddToHandle(this);
}

void Circle::Render(SDL_Renderer *renderer){
	SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 255);

	DrawCircle(renderer, position, radius);
	SDL_RenderDrawLine(renderer, position.x, position.y, position.x + radius * cos(angle), position.y - radius * sin(angle));
}

bool Circle::PointInside(Vector2D point, float _radius){
	if((point - position).SqMag() < (radius+_radius)*(radius+_radius)){
		return true;
	}
	return false;
}
float Circle::PointInsideDistance(Vector2D point, Vector2D *dirDest, float _radius){
	float dist = sqrt((point - position).SqMag());
	*dirDest = (point - position) / dist;
	return radius + _radius - dist;
}
