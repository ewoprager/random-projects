#ifndef Network_h
#define Network_h

#include "header.hpp"

class Network {
public:
	Network() {}
	
	virtual void Send() {}
	
	virtual void Recv() {}
	
	OtherClient clients[MAX_SOCKETS];
	
	bool GetRunning(){
		return running;
	}
	
	virtual int GetExternalData(short int ids[], float xs[], float ys[]) { return 0; }
	
	virtual short int GetId(){ return 0; }
	
	float x;
	float y;
	short int id;
	
protected:
	bool running;
};


#endif /* Network_h */
