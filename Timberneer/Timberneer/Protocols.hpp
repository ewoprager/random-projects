#ifndef Protocols_hpp
#define Protocols_hpp

#include "App.hpp"

class Updated {
public:
	Updated();
	virtual ~Updated();
	
	virtual void Update(float deltaTime) {}
	int GetUpdateId(){ return updateId; }
	
private:
	int updateId;
};

class Collider {
public:
	Collider(uint8_t _layers, float _radius, vec2 _position);
	virtual ~Collider();
	
	int GetColId(){ return colliderId; }
	uint8_t GetLayers(){ return layers; }
	vec2 GetPos(){ return position; }
	void SetPos(vec2 pos){ position = pos; }
	float GetSqRad(){ return squareRadius; }
	float GetRad(){ return radius; }
	void SetRad(const float& val){
		radius = val;
		squareRadius = val*val;
	}
	void SetSqRad(const float& val){
		squareRadius = val;
		radius = sqrt(val);
	}
	
protected:
	vec2 position;
	uint8_t layers; // layers this infringes upon (different to layers a child object might not be able to be placed on)

private:
	int colliderId;
	float squareRadius, radius;
};

class RenderedParent {
public:
	RenderedParent(bool _interactable);
	virtual ~RenderedParent();
	
	virtual void StartHover() {}
	virtual void StopHover() {}
	virtual void LeftClick() {}
	virtual void RightClick() {}
	
	int GetParentId(){ return parentId; }
	GLfloat *GetParentIdEncoded(){ return parentIdEncoded; }
	
	bool active;
	float baseMatrix[4][4];
	float worldMatrix[4][4];
	bool interactable;
	
protected:
	void SetBaseMatrix(const float& scale, const double& yaw);
	void ResetWorldMatrix();
	void TransformMatrixToWorldPosition(const vec2& position, const float& elevation, const bool& alignToGround=false);
	
private:
	int parentId;
	GLfloat parentIdEncoded[4];
};

class Rendered : public RenderedParent {
public:
	Rendered(const char *bin, bool interactable);
	Rendered(ObjectData data, bool interactable);
	virtual ~Rendered();
	
	int GetRenderId(){ return renderId; }
	void SetData(const ObjectData& data){ objectData = data; }
	ObjectData GetData(){ return objectData; }
	
protected:
	ObjectData objectData;
	
private:
	int renderId;
};

class RenderedInstanced : public RenderedParent {
public:
	RenderedInstanced();
	RenderedInstanced(int _instancedParentIndex, bool interactable);
	virtual ~RenderedInstanced();
	
	int GetInstanceId(){ return instanceId; }
	void ChangeParent(int newInstancedParentIndex);
	
protected:
	unsigned char instancedParentIndex;
	
private:
	int instanceId;
};


#endif /* Protocols_hpp */
