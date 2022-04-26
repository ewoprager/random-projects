#include "Protocols.hpp"
#include "InstancedParent.hpp"
#include "World.hpp"

extern SmartArray<MAX_OBJECTS, Updated *> updatedArray;
extern SmartArray<MAX_OBJECTS, RenderedParent *> renderedParentArray;
extern SmartArray<MAX_OBJECTS, Rendered *> renderedArray;
extern SmartArray<MAX_OBJECTS, Collider *> colliderArray;

Updated::Updated(){
	updateId = updatedArray.Add(this);
}
Updated::~Updated(){
	updatedArray.Remove(updateId);
}

Collider::Collider(uint8_t _layers, float _radius, vec2 _position){
	layers = _layers;
	position = _position;
	SetRad(_radius);
	colliderId = colliderArray.Add(this);
}
Collider::~Collider(){
	colliderArray.Remove(colliderId);
}

RenderedParent::RenderedParent(bool _interactable){
	interactable = _interactable;
	M4x4_Identity(worldMatrix);
	parentId = renderedParentArray.Add(this);
	parentIdEncoded[0] = (GLfloat)((parentId >>  0) & 0xFF) / (GLfloat)0xFF;
	parentIdEncoded[1] = (GLfloat)((parentId >>  8) & 0xFF) / (GLfloat)0xFF;
	parentIdEncoded[2] = (GLfloat)((parentId >>  16) & 0xFF) / (GLfloat)0xFF;
	parentIdEncoded[3] = (GLfloat)((parentId >>  24) & 0xFF) / (GLfloat)0xFF;
	active = true;
}
RenderedParent::~RenderedParent(){
	renderedParentArray.Remove(parentId);
}
void RenderedParent::ResetWorldMatrix(){
	memcpy(worldMatrix, baseMatrix, 16 * sizeof(float));
}
void RenderedParent::SetBaseMatrix(const float& scale, const double& yaw){
	float m[4][4];
	M4x4_Identity(baseMatrix);
	M4x4_Scaling(scale, m);
	M4x4_PreMultiply(baseMatrix, m);
	M4x4_yRotation(-yaw, m);
	M4x4_PreMultiply(baseMatrix, m);
	ResetWorldMatrix();
}
void RenderedParent::TransformMatrixToWorldPosition(const vec2& position, const float& elevation, const bool& alignToGround){
	float m[4][4];
	
	M4x4_Translation({0.0f, elevation, 0.0f}, m);
	M4x4_PreMultiply(worldMatrix, m);
	if(alignToGround){
		World_GroundAlignmentMatrix(position, m);
		M4x4_PreMultiply(worldMatrix, m);
	}
	M4x4_Translation({position.x, World_GetGroundHeight(position), position.y}, m);
	M4x4_PreMultiply(worldMatrix, m);
}

Rendered::Rendered(const char *bin, bool interactable) : RenderedParent(interactable){
	renderId = renderedArray.Add(this);
	objectData = ReadProcessedOBJFile(bin);
}
Rendered::Rendered(ObjectData data, bool interactable) : RenderedParent(interactable){
	renderId = renderedArray.Add(this);
	objectData = data;
}
Rendered::~Rendered(){
	renderedArray.Remove(renderId);
	free(objectData.vertices);
	free(objectData.divisionData);
}

extern InstancedParent *instancedParents[];
RenderedInstanced::RenderedInstanced() : RenderedParent(false) {
	instanceId = -1;
}
RenderedInstanced::RenderedInstanced(int _instancedParentIndex, bool interactable) : RenderedParent(interactable) {
	instancedParentIndex = _instancedParentIndex;
	instanceId = instancedParents[instancedParentIndex]->Add(this);
}
RenderedInstanced::~RenderedInstanced(){
	if(instanceId != -1) instancedParents[instancedParentIndex]->Remove(instanceId);
}
void RenderedInstanced::ChangeParent(int newInstancedParentIndex){
	if(instanceId != -1) instancedParents[instancedParentIndex]->Remove(instanceId);
	instancedParentIndex = newInstancedParentIndex;
	instanceId = instancedParents[instancedParentIndex]->Add(this);
}
