#include "Polygon.hpp"

Polygon::Polygon(App* app, bool _fixed, float density, Vector2D initialPos, std::initializer_list<Vector2D> list, double initialAngle, float _mu, float _cor) : RigidbodyParent(app, _fixed, 1, initialPos, initialAngle, _mu, _cor) {
	
	colour = {150, static_cast<unsigned short>(255*mu), static_cast<unsigned short>(255*cor)};
	
	shape = polygon;
	
	// temporarily storing vertex info given
	Vector2D vertices[MAX_VERTICES];
	int v = 0;
	for(Vector2D vertex : list){
		vertices[v] = vertex;
		v++;
	}
	N = v;
	
	// calculating CoM offset (also calculating the area)
	float area = 0;
	Vector2D CoM;
	for(int i=0; i<N; i++){
		float ai = vertices[i].x * vertices[(i+1) % N].y - vertices[(i+1) % N].x * vertices[i].y;
		area += ai;
		CoM.x += (vertices[i].x + vertices[(i+1) % N].x) * ai;
		CoM.y += (vertices[i].y + vertices[(i+1) % N].y) * ai;
	}
	area /= 2;
	CoM /= 6 * area;
	
	// shifting so origin is at CoM
	position += CoM;
	for(int v=0; v<N; v++){
		vertices[v] -= CoM;
	}
	
	// storing vertex positions as radii and angles
	maxRadius = 0;
	for(int v=0; v<N; v++){
		rads[v] = sqrt(vertices[v].SqMag());
		if(rads[v] > maxRadius || v == 0){ maxRadius = rads[v]; }
		angles[v] = atan2(-vertices[v].y, vertices[v].x);
	}
	
	mass = METRES_PER_PIXEL*METRES_PER_PIXEL* density * area;
	
	double Ixx = 0;
	double Iyy = 0;
	for(int i=0; i<N; i++){
		float ai = vertices[i].x * vertices[(i+1) % N].y - vertices[(i+1) % N].x * vertices[i].y;
		Ixx += (vertices[i].y*vertices[i].y + vertices[i].y*vertices[(i+1) % N].y + vertices[(i+1) % N].y*vertices[(i+1) % N].y) * ai;
		Iyy += (vertices[i].x*vertices[i].x + vertices[i].x*vertices[(i+1) % N].x + vertices[(i+1) % N].x*vertices[(i+1) % N].x) * ai;
	}
	smoa = METRES_PER_PIXEL*METRES_PER_PIXEL*METRES_PER_PIXEL*METRES_PER_PIXEL* (Ixx + Iyy) / 12; // smoa = Izz = Ixx + Iyy (perpendicular axis theorum)
	
	theApp->AddToRender(this);
	updateId = theApp->AddToUpdate(this);
	handleId = theApp->collisionHandler->AddToHandle(this);
}

void Polygon::Render(SDL_Renderer *renderer){
	CalculateGeometry();
	
	SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 255);
	
	for(int v=0; v<N; v++){
		SDL_RenderDrawLine(renderer, position.x + vertexPoss[v].x, position.y + vertexPoss[v].y, position.x + vertexPoss[(v+1) % N].x, position.y + vertexPoss[(v+1) % N].y);
	}
}

void Polygon::CalculateGeometry(){
	if(geometryCalculated){ return; }
	for(int v=0; v<N; v++){
		vertexPoss[v] = RadAngle(rads[v], angle, angles[v]);
	}
	for(int v=0; v<N; v++){
		sideDirs[v] = (vertexPoss[(v+1) % N] - vertexPoss[v]).Crossed();
	}
	geometryCalculated = true;
	dirsNormalised = false;
}
void Polygon::NormaliseDirs(){
	if(dirsNormalised){ return; }
	for(int s=0; s<N; s++){
		float mag = sqrt(sideDirs[s].SqMag());
		if(mag == 0){
			std::cout << s << ": mag = " << mag << "\n";
			return;
		}
		sideDirs[s] /= mag;
	}
	dirsNormalised = true;
}

bool Polygon::PointInside(Vector2D point, float radius){
	CalculateGeometry();
	
	if(radius != 0){ NormaliseDirs(); }
	
	for(int vs=0; vs<N; vs++){
		if((Dot(point, sideDirs[vs].Crossed()) < Dot(position + vertexPoss[(vs+1) % N], sideDirs[vs].Crossed())) &&
		   (Dot(point, sideDirs[(vs+1) % N].Crossed()) > Dot(position + vertexPoss[(vs+1) % N], sideDirs[(vs+1) % N].Crossed()))){ // point is after the side
			return false;
		}
		if(Dot(point, sideDirs[vs]) - radius > Dot(position + vertexPoss[vs], sideDirs[vs])){ // point is in front of this side
			return false;
		}
	}
	//std::cout << "Point inside.\n";
	return true;
}
float Polygon::PointInsideDistance(Vector2D point, Vector2D *dirDest, float radius){
	CalculateGeometry();
	NormaliseDirs();
	
	float minIn = 0;
	int theVS = 0;
	for(int vs=0; vs<N; vs++){
		float in = Dot(position + vertexPoss[vs], sideDirs[vs]) - (Dot(point, sideDirs[vs]) - radius);
		if(in < minIn || vs == 0){
			minIn = in;
			theVS = vs;
		}
	}
	*dirDest = sideDirs[theVS];
	//std::cout << *dirDest << "\n";
	return minIn;
}

Vector2D Polygon::GetVertex(int which){
	if(which < 0 || which >= N){ std::cout << "Polygon asked for invalid vertex.\n"; return Vector2D(); }
	CalculateGeometry();
	return position + vertexPoss[which];
}
