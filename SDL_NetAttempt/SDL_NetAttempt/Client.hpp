#ifndef Client_hpp
#define Client_hpp

#include "Network.hpp"

class Client : public Network {
public:
	Client(const char *ip);
	~Client();
	
	void Send() override;
	void Recv() override;
	
	int GetExternalData(short int ids[], float xs[], float ys[]) override;
	
private:
	void CloseSocket(void);
	void SendData(uint8_t* data, uint16_t length, uint16_t flag);
	uint8_t* RecvData(uint16_t* length);
	void ProcessData(uint8_t* data, uint16_t* offset);
	void InitNetwork(const char* pIP, int iPort);
	bool CheckSocket(void);
	
	TCPsocket socket;
	SDLNet_SocketSet socket_set;
	
	OtherClient serverAsClient;
};

#endif /* Client_hpp */
