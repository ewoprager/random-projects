#include "App.hpp"
#include "ObjectParent.hpp"
#include "Polygon.hpp"
#include "Circle.hpp"
#include "CollisionHandler.hpp"

App::App() {
	for(int i=0; i<MAX_OBJECTS; i++){
		update[i] = false;
		render[i] = false;
	}
}

bool App::Init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen){
	
	int flags = 0;
	if(fullscreen){
		flags = SDL_WINDOW_FULLSCREEN;
	}
	
	isRunning = true;
	
	// subsystems:
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "Subsystems failed to initialise.\n";
		isRunning = false;
		return false;
	}
	//std::cout << "Subsystems initialised.\n";
	
	// window:
	window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
	if(!window){
		std::cout << "Window creation failed.\n";
		isRunning = false;
		return false;
	}
	//std::cout << "Window created.\n";
	
	// renderer:
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer){
		std::cout << "Renderer creation failed.\n";
		isRunning = false;
		return false;
	}
	//std::cout << "Renderer created.\n";
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	// important things
	collisionHandler = new CollisionHandler(this);
	
	
	// game objects:
	// walls
	new Polygon(this, true, 10, {0, 704}, {{0, 0}, {32, -5}, {96, 0}, {96, 64}, {0, 64}}, 0, 0.5, 0.5);
	new Polygon(this, true, 10, {96, 704}, {{0, 0}, {64, -15}, {160, 0}, {160, 64}, {0, 64}}, 0, 0.5, 0.5);
	new Polygon(this, true, 10, {256, 704}, {{0, 0}, {160, -10}, {256, 0}, {256, 64}, {0, 64}}, 0, 0.5, 0.5);
	new Polygon(this, true, 10, {512, 704}, {{0, 0}, {256, -30}, {512, 0}, {512, 64}, {0, 64}}, 0, 0.5, 0.5);
	
	new Polygon(this, true, 10, {0, 0}, {{0, 0}, {1024, 0}, {1024, 64}, {0, 64}}, 0,		0.5, 0.5);
	new Polygon(this, true, 10, {0, 0}, {{0, 0}, {64, 0}, {64, 768}, {0, 768}}, 0,			0.5, 0.5);
	new Polygon(this, true, 10, {1024, 0}, {{0, 0}, {0, 768}, {-64, 768}, {-64, 0}}, 0,		0.5, 0.5);
	
	
	poly1 = new Polygon(this, false, 20, {512, 384}, {{70, -40}, {70, 40}, {-70, 40}, {-70, -40}}, 0, 0.5, 0.5);
	circ1 = new Circle(this, false, 5, {572, 414}, 30, 5, 0.1);
	circ2 = new Circle(this, false, 5, {452, 414}, 30, 5, 0.1);
	
	poly2 = new Polygon(this, false, 20, {650, 500}, {{0, -40}, {0, 40}, {-70, 40}}, 0, 0.5, 0.5);
	
	collisionHandler->AddPivot(poly1->GetHandleId(), circ1->GetHandleId(), {572, 414});
	collisionHandler->AddPivot(poly1->GetUpdateId(), circ2->GetUpdateId(), {452, 414});
	
	//new Polygon(this, false, 10, {{128, 128}, {128, 12}});
	
	return true;
}

void App::Clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void App::HandleEvents(){
	while(isRunning && SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch(event.button.button){
					case SDL_BUTTON_LEFT:
						LeftButtonDown(Vector2D(event.button.x, event.button.y));
						break;
					case SDL_BUTTON_RIGHT:
						RightButtonDown(Vector2D(event.button.x, event.button.y), event.button.timestamp);
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch(event.button.button){
					case SDL_BUTTON_LEFT:
						LeftButtonUp(Vector2D(event.button.x, event.button.y));
						break;
					case SDL_BUTTON_RIGHT:
						RightButtonUp(Vector2D(event.button.x, event.button.y), event.button.timestamp);
						break;
					default:
						break;
				}
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_LEFT:
						leftDown = true;
						break;
					case SDLK_RIGHT:
						rightDown = true;
						break;
					case SDLK_UP:
						//
						break;
					case SDLK_DOWN:
						//
						break;
					default:
						break;
				}
				break;
			case SDL_KEYUP:
				switch(event.key.keysym.sym){
					case SDLK_LEFT:
						leftDown = false;
						break;
					case SDLK_RIGHT:
						rightDown = false;
						break;
					case SDLK_UP:
						//
						break;
					case SDLK_DOWN:
						//
						break;
					default:
						break;
				}
				break;
			default:
				break;
        }
    }
}

void App::LoopStart(int deltaT){ // this is in an integer number of milliseconds
	latestDeltaT = gameSpeed*((float)deltaT)/1000.0f;
	
	if(dragging && leftButtonDown){
		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
		Vector2D mousePos = Vector2D(mouse_x, mouse_y);
		Vector2D startPos = bodyDragging->GetPosition() + RadAngle(dragStartRad, bodyDragging->GetAngle(), dragStartAngle);
		bodyDragging->ApplyForce(startPos, mousePos - startPos);
		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		DrawArrow(renderer, startPos.x, startPos.y, mouse_x, mouse_y);
	}
	
	if(firing && rightButtonDown){
		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
		Vector2D startPos = bodyFiring->GetPosition() + RadAngle(fireStartRad, bodyFiring->GetAngle(), fireStartAngle);
		
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		DrawArrow(renderer, startPos.x, startPos.y, mouse_x, mouse_y);
	}
	
	if(rightDown){
		circ1->ApplyMoment(-15);
		circ2->ApplyMoment(-15);
	}
	if(leftDown){
		circ1->ApplyMoment(15);
		circ2->ApplyMoment(15);
	}
	
	// updating objects
	for(int i=0; i<MAX_OBJECTS; i++){
		if(!update[i]){ continue; }
		if(toUpdate[i]->GetFixed()){ continue; }
		toUpdate[i]->Update(latestDeltaT); // this is float number of seconds
	}
	
	collisionHandler->DetectCollisions();
}

void App::LoopEnd(int deltaT){
	latestDeltaT = gameSpeed*((float)deltaT)/1000.0f;
	
	collisionHandler->CorrectCollisions(latestDeltaT);
}

void App::Render(){
	// put at beggining of main loop instead for debugging
	//SDL_RenderClear(renderer);
	
	for(int i=0; i<MAX_OBJECTS; i++){
		if(render[i]){
			toRender[i]->Render(renderer);
		}
	}
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderPresent(renderer);
}

int App::AddToRender(ObjectParent *object){
	int newId = GetNewId(MAX_OBJECTS, render);
	if(newId == -1){ std::cout << "Max render object count reached.\n"; return -1; }
	toRender[newId] = object;
	render[newId] = true;
	return newId;
}
int App::AddToUpdate(RigidbodyParent *object){
	int newId = GetNewId(MAX_OBJECTS, update);
	if(newId == -1){ std::cout << "Max update object count reached.\n"; return -1; }
	toUpdate[newId] = object;
	update[newId] = true;
	return newId;
}


void App::LeftButtonDown(Vector2D pos){
	leftButtonDown = true;
	bodyDragging = collisionHandler->CollisionPoint(pos);
	if(bodyDragging == nullptr){ dragging = false; return; }
	dragging = true;
	Vector2D off = pos - bodyDragging->GetPosition();
	dragStartRad = sqrt(off.SqMag());
	dragStartAngle = atan2(-off.y, off.x) - bodyDragging->GetAngle();
}
void App::LeftButtonUp(Vector2D pos){
	leftButtonDown = false;
	dragging = false;
}
void App::RightButtonDown(Vector2D pos, int time){
	rightButtonDown = true;
	bodyFiring = collisionHandler->CollisionPoint(pos);
	if(bodyFiring == nullptr){ firing = false; return; }
	firing = true;
	Vector2D off = pos - bodyFiring->GetPosition();
	fireStartRad = sqrt(off.SqMag());
	fireStartAngle = atan2(-off.y, off.x) - bodyFiring->GetAngle();
	fireStartTime = time;
}
void App::RightButtonUp(Vector2D pos, int time){
	if(rightButtonDown && firing){
		Vector2D startPos = bodyFiring->GetPosition() + RadAngle(fireStartRad, bodyFiring->GetAngle(), fireStartAngle);
		bodyFiring->ApplyImpulse(startPos, 0.1 * (pos - startPos));
	}
	rightButtonDown = false;
	firing = false;
}







int GetNewId(int size, bool bools[]){
	for(int i=0; i<size; i++){
		if(!bools[i]){
			return i;
		}
	}
	return -1;
}
void DrawArrow(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float headRatio){
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	float len = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	double angle;
	angle = atan2(y1 - y2, x2 - x1);
	float a = len * headRatio * 0.5f;
	float ax = x1 + len * (1 - headRatio) * cos(angle);
	float ay = y1 - len * (1 - headRatio) * sin(angle);
	float dx = a * sin(angle);
	float dy = a * cos(angle);
	SDL_RenderDrawLine(renderer, x2, y2, ax + dx, ay + dy);
	SDL_RenderDrawLine(renderer, x2, y2, ax - dx, ay - dy);
}
void DrawCircle(SDL_Renderer *renderer, Vector2D pos, int32_t radius){
	const int32_t diameter = (radius * 2);
	int32_t centreX = pos.x;
	int32_t centreY = pos.y;
	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y){
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0){
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0){
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
