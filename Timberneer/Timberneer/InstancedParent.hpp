#ifndef InstancedParent_hpp
#define InstancedParent_hpp

#include "Header.hpp"

#define MAX_INSTANCES 256

class InstancedParent : public SmartArray<MAX_INSTANCES, RenderedInstanced *> {
public:
	InstancedParent(const char *bin);
	InstancedParent(ObjectData data);
	~InstancedParent();
	
	ObjectData GetData(){ return objectData; }
	
private:
	ObjectData objectData;
};

#endif /* InstancedParent_hpp */
