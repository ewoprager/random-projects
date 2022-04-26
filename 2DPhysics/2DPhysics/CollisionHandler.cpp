#include "CollisionHandler.hpp"
#include "RigidbodyParent.hpp"
#include "Polygon.hpp"
#include "Circle.hpp"

void CollisionHandler::DetectCollisions(){
	colsN = 0;
	for(int i=0; i<MAX_OBJECTS; i++){ // for every object, i
	if(!handle[i]){ continue; }
	for(int j=i+1; j<MAX_OBJECTS; j++){ // for every other object after i in the array, j
	if(!handle[j]){ continue; }
		// don't bother if they are both fixed
		if(toHandle[i].rbp->GetFixed() && toHandle[j].rbp->GetFixed()){ continue; }
		
		// seeing if we have been told they dont collide
		bool found = false;
		for(int cc=0; cc<cantCollideN[i]; cc++){
			if(cantCollide[i][cc] == j){ found = true; break; }
		}
		if(found){ continue; }
		
		// narrow down collision possibilities
		// check if within max radii
		if((toHandle[i].rbp->GetPosition() - toHandle[j].rbp->GetPosition()).SqMag() >=
		   (toHandle[i].rbp->GetMaxRadius() + toHandle[j].rbp->GetMaxRadius())*(toHandle[i].rbp->GetMaxRadius() + toHandle[j].rbp->GetMaxRadius()))
		{ continue; } // out of range
		
		//std::cout << "Potential Collision\n";
		
		Vertex vertices[MAX_VERTICES*2];
		int vN = 0;
		
		// polygon-polygon: check both ways
		// polygon-circle: check both ways but only use one - polygon check more important
		// circle-circle: check one way

		if(toHandle[i].shape == polygon){
			if(toHandle[j].shape == polygon){
				// look for i's vertices in j
				RigidbodyParent *insideMe = toHandle[j].rbp;
				Polygon *imInside = polygons[toHandle[i].index];
				for(int v=0; v<imInside->GetN(); v++){
					if(insideMe->PointInside(imInside->GetVertex(v))){
						vertices[vN].inInotJ = false;
						vertices[vN].pos = imInside->GetVertex(v);
						Vector2D dest;
						vertices[vN].distIn = insideMe->PointInsideDistance(vertices[vN].pos, &dest);
						vertices[vN].dir = dest;
						vN++;
					}
				}
				
				// look for j's vertices in i
				insideMe = toHandle[i].rbp;
				imInside = polygons[toHandle[j].index];
				for(int v=0; v<imInside->GetN(); v++){
					if(insideMe->PointInside(imInside->GetVertex(v))){
						vertices[vN].inInotJ = true;
						vertices[vN].pos = imInside->GetVertex(v);
						Vector2D dest;
						vertices[vN].distIn = insideMe->PointInsideDistance(vertices[vN].pos, &dest);
						vertices[vN].dir = dest;
						vN++;
					}
				}
			} else { // j is a circle (and i is a polygon)
				// look for j's point radius in i
				RigidbodyParent *insideMe = toHandle[i].rbp;
				Circle *imInside = circles[toHandle[j].index];
				if(insideMe->PointInside(imInside->GetPosition(), imInside->GetRadius())){
					vertices[vN].inInotJ = true;
					Vector2D dest;
					vertices[vN].distIn = insideMe->PointInsideDistance(imInside->GetPosition(), &dest, imInside->GetRadius());
					vertices[vN].dir = dest;
					vertices[vN].pos = imInside->GetPosition() - imInside->GetRadius() * dest;
					vN++;
				} else {
					// look for i's vertices in j
					RigidbodyParent *insideMe = toHandle[j].rbp;
					Polygon *imInside = polygons[toHandle[i].index];
					for(int v=0; v<imInside->GetN(); v++){
						if(insideMe->PointInside(imInside->GetVertex(v))){
							vertices[vN].inInotJ = false;
							vertices[vN].pos = imInside->GetVertex(v);
							Vector2D dest;
							vertices[vN].distIn = insideMe->PointInsideDistance(vertices[vN].pos, &dest);
							vertices[vN].dir = dest;
							vN++;
						}
					}
				}
			}
		} else { // i is a circle
			if(toHandle[j].shape == polygon){
				// look for i's point radius in j
				RigidbodyParent *insideMe = toHandle[j].rbp;
				Circle *imInside = circles[toHandle[i].index];
				if(insideMe->PointInside(imInside->GetPosition(), imInside->GetRadius())){
					vertices[vN].inInotJ = false;
					Vector2D dest;
					vertices[vN].distIn = insideMe->PointInsideDistance(imInside->GetPosition(), &dest, imInside->GetRadius());
					vertices[vN].dir = dest;
					vertices[vN].pos = imInside->GetPosition() - imInside->GetRadius() * dest;
					vN++;
				} else {
					// look for j's vertices in i
					RigidbodyParent *insideMe = toHandle[i].rbp;
					Polygon *imInside = polygons[toHandle[j].index];
					for(int v=0; v<imInside->GetN(); v++){
						if(insideMe->PointInside(imInside->GetVertex(v))){
							vertices[vN].inInotJ = true;
							vertices[vN].pos = imInside->GetVertex(v);
							Vector2D dest;
							vertices[vN].distIn = insideMe->PointInsideDistance(vertices[vN].pos, &dest);
							vertices[vN].dir = dest;
							vN++;
						}
					}
				}
			} else { // j is a circle (and i is a circle)
				// look for i's point-radius in j
				RigidbodyParent *insideMe = toHandle[j].rbp;
				Circle *imInside = circles[toHandle[i].index];
				if(insideMe->PointInside(imInside->GetPosition(), imInside->GetRadius())){
					vertices[vN].inInotJ = false;
					Vector2D dest;
					vertices[vN].distIn = insideMe->PointInsideDistance(imInside->GetPosition(), &dest, imInside->GetRadius());
					vertices[vN].dir = dest;
					vertices[vN].pos = imInside->GetPosition() - imInside->GetRadius() * dest;
					vN++;
				}
			}
		}
		
		for(int v=0; v<vN; v++){ // looping through all the vertices that have collided
			cols[colsN].whosIn = ( vertices[v].inInotJ ? j : i );
			cols[colsN].inWho = ( vertices[v].inInotJ ? i : j );
			cols[colsN].position = vertices[v].pos;
			
			RigidbodyParent *rb_WhosIn = toHandle[cols[colsN].whosIn].rbp;
			RigidbodyParent *rb_inWho = toHandle[cols[colsN].inWho].rbp;
			
			// debugging
			//SDL_SetRenderDrawColor(theApp->renderer, 255, 0, 0, 255); // red
			
			Vector2D colDir = vertices[v].dir;
			
			// normal reaction
			float corAvg = (rb_WhosIn->GetCor() + rb_inWho->GetCor())/2;
			// (reciprocals of) effective masses normally
			float repn_whosIn = rb_WhosIn->GetRecipEffMass(cols[colsN].position, colDir);
			float repn_inWho = rb_inWho->GetRecipEffMass(cols[colsN].position, colDir);
			// jump
			Vector2D jumpN = colDir * vertices[v].distIn * (1 + corAvg); // [pixels/dt]
			cols[colsN].jumpN_whosIn = jumpN / (1 + repn_inWho/repn_whosIn);
			cols[colsN].jumpN_inWho = jumpN / (1 + repn_whosIn/repn_inWho);
			// impulse
			float relVelN = ( (rb_inWho->GetFixed()) ? 0 : Dot(colDir, rb_inWho->GetPointVel(cols[colsN].position)) ) - ( (rb_WhosIn->GetFixed()) ? 0 : Dot(colDir, rb_WhosIn->GetPointVel(cols[colsN].position)) ); // [pixels/s]
			Vector2D impN = colDir * relVelN * (1 + corAvg) * METRES_PER_PIXEL; // [metres/s]
			cols[colsN].impN_whosIn = impN / (1 + repn_inWho/repn_whosIn);
			cols[colsN].impN_inWho = impN / (1 + repn_whosIn/repn_inWho);
			// for calculating max friction
			float reactionForce = relVelN * (1 + corAvg) * METRES_PER_PIXEL / (repn_whosIn + repn_inWho); // [metres/s]
			
			// tangential reaction
			Vector2D frictionDir = colDir.Crossed();
			float muAvg = (rb_WhosIn->GetMu() + rb_inWho->GetMu())/2;
			float frictionMaxMag = muAvg * reactionForce; // [metres/s]
			float relVelT = 0.5 * (( (rb_inWho->GetFixed()) ? 0 : Dot(frictionDir, rb_inWho->GetPointVel(cols[colsN].position)) ) -
									  ( (rb_WhosIn->GetFixed()) ? 0 : Dot(frictionDir, rb_WhosIn->GetPointVel(cols[colsN].position)) )); // [pixels/s]
			float abs_relVelT_convUnits = abs(relVelT * METRES_PER_PIXEL); // [metres/s]

			Vector2D jumpT; // [pixels/dt]
			Vector2D impT; // [metres/s]
			
			// debugging
			//SDL_SetRenderDrawColor(theApp->renderer, 0, 0, 255, 255); // blue
			
			if(frictionMaxMag >= abs_relVelT_convUnits){ // [metres/s]
				// static friction in action providing a reaction also
				cols[colsN].staticFric = true;
				// (reciprocals of) effective masses tangentially
				float rept_whosIn = rb_WhosIn->GetRecipEffMass(cols[colsN].position, frictionDir);
				float rept_inWho = rb_inWho->GetRecipEffMass(cols[colsN].position, frictionDir);
				// jump
				jumpT = frictionDir * relVelT * vertices[v].distIn / relVelN; // [pixels/dt]
				cols[colsN].jumpT_whosIn = jumpT / (1 + rept_inWho/rept_whosIn);
				cols[colsN].jumpT_inWho = jumpT / (1 + rept_whosIn/rept_inWho);
				
				// impulse
				impT = frictionDir * relVelT * METRES_PER_PIXEL; // [metres/s]
				cols[colsN].impT_whosIn = impT / (1 + rept_inWho/rept_whosIn);
				cols[colsN].impT_inWho = impT / (1 + rept_whosIn/rept_inWho);
				
				// debugging
				//std::cout << "Static,  V  limited - max: " << frictionMaxMag << " >= relVelT: " << abs_relVelT_convUnits << "\n";
				//SDL_SetRenderDrawColor(theApp->renderer, 0, 255, 0, 255); // green
				//DrawArrow(theApp->renderer, cols[colsN].position.x, cols[colsN].position.y, cols[colsN].position.x + cols[colsN].fricVec_whosIn.x, cols[colsN].position.y + cols[colsN].fricVec_whosIn.y);
				
			} else {
				// dynamic friction in action
				cols[colsN].staticFric = false;
				Vector2D friction =	frictionDir * frictionMaxMag * ( (relVelT >= 0) ? 1 : -1 ); // [metres/s]
				cols[colsN].fricForce_whosIn = friction;
				cols[colsN].fricForce_inWho = friction;
				
				// debugging
				//std::cout << "Dynamic, Mu limited - max: " << frictionMaxMag << " <  relVelT: " << abs_relVelT_convUnits << "\n";
				//SDL_SetRenderDrawColor(theApp->renderer, 255, 0, 0, 255); // red
				//DrawArrow(theApp->renderer, cols[colsN].position.x, cols[colsN].position.y, cols[colsN].position.x + friction.x * deltaT, cols[colsN].position.y + friction.y * deltaT);
			}
			
			colsN++;
			
			// drawing for debugging:
			
			// vertex:
			//SDL_SetRenderDrawColor(theApp->renderer, 0, 0, 255, 255); // blue
			//SDL_RenderDrawLine(theApp->renderer, cols[colsN].position.x - 5, cols[colsN].position.y, cols[colsN].position.x, cols[colsN].position.y - 5);
			//SDL_RenderDrawLine(theApp->renderer, cols[colsN].position.x, cols[colsN].position.y - 5, cols[colsN].position.x + 5, cols[colsN].position.y);
			//SDL_RenderDrawLine(theApp->renderer, cols[colsN].position.x + 5, cols[colsN].position.y, cols[colsN].position.x, cols[colsN].position.y + 5);
			//SDL_RenderDrawLine(theApp->renderer, cols[colsN].position.x, cols[colsN].position.y + 5, cols[colsN].position.x - 5, cols[colsN].position.y);
			
			/*
			// normal reaction
			SDL_SetRenderDrawColor(theApp->renderer, 255, 0, 0, 255); // red
			SDL_RenderDrawLine(theApp->renderer, vertexIn.x, vertexIn.y, vertexIn.x + 500* jump_whosIn.x * METRES_PER_PIXEL, vertexIn.y + 500* jump_whosIn.y * METRES_PER_PIXEL);
			
			// friction
			SDL_SetRenderDrawColor(theApp->renderer, 0, 255, 0, 255); // green
			SDL_RenderDrawLine(theApp->renderer, vertexIn.x, vertexIn.y, vertexIn.x + 500* friction.x, vertexIn.y + 500* friction.y);
			 */
		}
	}
	}
	
}

void CollisionHandler::CorrectCollisions(float deltaT){
	// applying normal and friction jumps & impulses / forces for each collision
	for(int c=0; c<colsN; c++){
		RigidbodyParent *rb_WhosIn = toHandle[cols[c].whosIn].rbp;
		RigidbodyParent *rb_inWho = toHandle[cols[c].inWho].rbp;
		
		if(!rb_WhosIn->GetFixed()){
			// normal
			rb_WhosIn->ApplyJump(cols[c].position, cols[c].jumpN_whosIn); // move out of contact
			rb_WhosIn->ApplyImpulse(cols[c].position, cols[c].impN_whosIn); // restitution
			
			// tangential
			if(cols[c].staticFric){
				rb_WhosIn->ApplyJump(cols[colsN].position, cols[c].jumpT_whosIn); // undo slide (equivilant to move out of contact)
				rb_WhosIn->ApplyImpulse(cols[c].position, cols[c].impT_whosIn); // 'stopping'
			} else {
				rb_WhosIn->ApplyForce(cols[c].position, cols[c].fricForce_whosIn); // just a frictional force
			}
		}
		if(!rb_inWho->GetFixed()){
			// normal
			rb_inWho->ApplyJump(cols[c].position, -1 * cols[c].jumpN_inWho); // move out of contact
			rb_inWho->ApplyImpulse(cols[c].position, -1 * cols[c].impN_inWho); // restitution
			
			// tangential
			if(cols[c].staticFric){
				rb_inWho->ApplyJump(cols[c].position, -1 * cols[c].jumpT_inWho); // undo slide (equivilant to move out of contact)
				rb_inWho->ApplyImpulse(cols[c].position, -1 * cols[c].impT_inWho); // 'stopping'
			} else {
				rb_inWho->ApplyForce(cols[c].position, -1 * cols[c].fricForce_inWho); // just a frictional force
			}
		}
	}
	
	// handling pivots
	for(int p=0; p<pivotsN; p++){
		RigidbodyParent *rb1 = toHandle[pivots[p].object1].rbp;
		RigidbodyParent *rb2 = toHandle[pivots[p].object2].rbp;
		
		Vector2D point1 = rb1->GetPointPos(pivots[p].rad1, pivots[p].angle1);
		Vector2D point2 = rb2->GetPointPos(pivots[p].rad2, pivots[p].angle2);
		// jump
		Vector2D offset = point2 - point1; // [pixels/dt]
		if(offset.SqMag() > 0){
			Vector2D offsetDir = offset.Normalised();
			float rep1 = rb1->GetRecipEffMass(point1, offsetDir);
			float rep2 = rb2->GetRecipEffMass(point2, offsetDir);
			Vector2D jump1 = offset / (1 + rep2/rep1);
			Vector2D jump2 = offset / (1 + rep1/rep2);
			// impulse
			float relVel = ( (rb2->GetFixed()) ? 0 : Dot(offsetDir, rb2->GetPointVel(point2)) ) - ( (rb1->GetFixed()) ? 0 : Dot(offsetDir, rb1->GetPointVel(point1)) ); // [pixels/s]
			Vector2D impulse = offsetDir * relVel * METRES_PER_PIXEL; // [metres/s]
			Vector2D imp1 = impulse / (1 + rep2/rep1);
			Vector2D imp2 = impulse / (1 + rep1/rep2);
			
			if(!rb1->GetFixed()){
				rb1->ApplyJump(point1, jump1); // position correction
				rb1->ApplyImpulse(point1, imp1); // velocity correction
			}
			
			if(!rb2->GetFixed()){
				rb2->ApplyJump(point2, -1 * jump2); // position correction
				rb2->ApplyImpulse(point2, -1 * imp2); // velocity correction
			}
			
			// for debugging
			SDL_SetRenderDrawColor(theApp->renderer, 255, 0, 0, 255);
			SDL_RenderDrawLine(theApp->renderer, rb1->GetPosition().x, rb1->GetPosition().y, point1.x, point1.y);
			SDL_RenderDrawLine(theApp->renderer, rb2->GetPosition().x, rb2->GetPosition().y, point2.x, point2.y);
		}
	}
}

void CollisionHandler::AddCantCollide(int obj1, int obj2){
	cantCollide[obj1][cantCollideN[obj1]] = obj2;
	cantCollideN[obj1]++;
}

void CollisionHandler::AddPivot(int obj1, int obj2, Vector2D position, bool cantCollide){
	pivots[pivotsN].object1 = obj1;
	pivots[pivotsN].object2 = obj2;
	Vector2D off1 = position - toHandle[obj1].rbp->GetPosition();
	float bla = sqrt(off1.SqMag());
	pivots[pivotsN].rad1 = bla;
	pivots[pivotsN].angle1 = atan2(-off1.y, off1.x) + toHandle[obj1].rbp->GetAngle();
	Vector2D off2 = position - toHandle[obj2].rbp->GetPosition();
	pivots[pivotsN].rad2 = sqrt(off2.SqMag());
	pivots[pivotsN].angle2 = atan2(-off2.y, off2.x) + toHandle[obj2].rbp->GetAngle();
	
	// not in use
	/*
	// telling the objects
	toUpdate[obj1]->SetPivot(toUpdate[obj2], pivots[pivotsN].rad1, pivots[pivotsN].angle1);
	toUpdate[obj2]->SetPivot(toUpdate[obj1], pivots[pivotsN].rad2, pivots[pivotsN].angle2);
	*/
	
	if(cantCollide){
		AddCantCollide(obj1, obj2);
		AddCantCollide(obj2, obj1);
	}
	
	pivotsN++;
}

RigidbodyParent *CollisionHandler::CollisionPoint(Vector2D pos){
	for(int i=0; i<MAX_OBJECTS; i++){ // for every object, i
		if(!handle[i]){ continue; }
		// narrow down collision possibilities
		if((toHandle[i].rbp->GetPosition() - pos).SqMag() >= ( toHandle[i].rbp->GetMaxRadius() * toHandle[i].rbp->GetMaxRadius() )){ continue; } // out of range

		// detect exact collision with i
		if(toHandle[i].rbp->PointInside(pos)){
			return toHandle[i].rbp;
		}
	}
	return nullptr;
}

int CollisionHandler::AddToHandle(RigidbodyParent *object){
	int newId = GetNewId(MAX_OBJECTS, handle);
	if(newId == -1){ std::cout << "Max handle object count reached.\n"; return -1; }
	switch(object->GetShape()){
		case polygon: {
			int newPolygonId = GetNewId(MAX_OBJECTS, _polygon);
			if(newPolygonId == -1){ std::cout << "Max polygon count reached.\n"; return -1; }
			polygons[newPolygonId] = (Polygon *)object;
			_polygon[newPolygonId] = true;
			toHandle[newId] = {object, polygon, newPolygonId};
		} break;
		case circle: {
			int newCircleId = GetNewId(MAX_OBJECTS, _circle);
			if(newCircleId == -1){ std::cout << "Max circle count reached.\n"; return -1; }
			circles[newCircleId] = (Circle *)object;
			_circle[newCircleId] = true;
			toHandle[newId] = {object, circle, newCircleId};
		} break;
	}
	handle[newId] = true;
	
	return newId;
}
