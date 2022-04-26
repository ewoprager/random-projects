#ifndef Server_hpp
#define Server_hpp

#include "Network.hpp"

class Server : public Network {
public:
	Server();
	~Server();
	
	void Send() override;
	void Recv() override;
	
	int GetExternalData(short int ids[], float xs[], float ys[]) override;
	
private:
	void CloseSocket(int index);
	int AcceptSocket(int index);
	uint8_t* RecvData(int index, uint16_t* length);
	void SendData(int index, uint8_t* data, uint16_t length, uint16_t flag);
	void ProcessData(int index, uint8_t* data, uint16_t* offset);
	
	void NewPlayer(short int ind);
	void PlayerQuit(short int ind);
	
	int next_ind = 1;
	TCPsocket server_socket;
	SDLNet_SocketSet socket_set;
	TCPsocket sockets[MAX_SOCKETS];
	IPaddress ip;
};

#endif /* Server_hpp */
