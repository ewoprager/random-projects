#include "InstancedParent.hpp"
#include "Protocols.hpp"

InstancedParent::InstancedParent(const char *bin) : SmartArray<MAX_INSTANCES, RenderedInstanced *>() {
	objectData = ReadProcessedOBJFile(bin);
}
InstancedParent::InstancedParent(ObjectData data){
	objectData = data;
}
InstancedParent::~InstancedParent(){
	free(objectData.vertices);
	free(objectData.divisionData);
}
