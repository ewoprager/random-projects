#include "Client.hpp"

Client::Client(const char *ip) : Network() {
	InitNetwork(ip, 3211); // arg #2 is the port
	
	serverAsClient.in_use = 1;
	serverAsClient.x = 0;
	serverAsClient.y = 0;
}

void Client::Recv(){

	if(CheckSocket()) {
		uint16_t length;
		uint8_t *data = RecvData(&length);
		if(data != NULL){
			uint16_t offset = 0;
			while(offset < length) {
				ProcessData(data, &offset);
			}
			free(data);
		} else {
			std::cout << "NULL data received.\n";
		}
	}
}

void Client::Send(){
	int offset = 0;
	uint8_t *array = (uint8_t*) malloc(2 * sizeof(float));
	memcpy(array + offset, reinterpret_cast<uint8_t*>(&x), sizeof(float));
	offset += sizeof(float);
	memcpy(array + offset, reinterpret_cast<uint8_t*>(&y), sizeof(float));
	offset += sizeof(float);
	SendData(array, offset, FLAG_NUMBER);
}

Client::~Client(){
	SendData(NULL, 0, FLAG_QUIT);
	CloseSocket();
	running = false;
	SDLNet_Quit();
}





void Client::CloseSocket(void) {
	if(SDLNet_TCP_DelSocket(socket_set, socket) == -1) {
		fprintf(stderr, "%s\n", SDLNet_GetError());
		system("pause");
		exit(-1);
	}
 
	SDLNet_FreeSocketSet(socket_set);
	SDLNet_TCP_Close(socket);
}
 
//-----------------------------------------------------------------------------
void Client::SendData(uint8_t* data, uint16_t length, uint16_t flag) {
	uint8_t temp_data[MAX_PACKETS];
 
	int offset = 0;
	memcpy(temp_data+offset, &flag, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(temp_data+offset, data, length);
	offset += length;

	int num_sent = SDLNet_TCP_Send(socket, temp_data, offset);
	if(num_sent < offset) {
		fprintf(stderr, "ER: SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		CloseSocket();
	}
}
 
//-----------------------------------------------------------------------------
uint8_t* Client::RecvData(uint16_t* length) {
	uint8_t temp_data[MAX_PACKETS];
	int num_recv = SDLNet_TCP_Recv(socket, temp_data, MAX_PACKETS);
	if(num_recv <= 0) {
		CloseSocket();
 
		const char* err = SDLNet_GetError();
		if(strlen(err) == 0) {
			printf("DB: server shutdown\n");
		} else {
			fprintf(stderr, "ER: SDLNet_TCP_Recv: %s\n", err);
		}
 
		return NULL;
	} else {
		*length = num_recv;
 
		uint8_t* data = (uint8_t*) malloc(num_recv*sizeof(uint8_t));
		memcpy(data, temp_data, num_recv);
 
		return data;
	}
}
 
//-----------------------------------------------------------------------------
void Client::ProcessData(uint8_t* data, uint16_t* offset) {
	if(data == NULL) return;
 
	uint16_t flag = *(uint16_t*) &data[*offset];
	*offset += sizeof(uint16_t);
 
	switch(flag) {
		case FLAG_NUMBER: { // update about other player: short int, float, float
			short int plId = *((short int*) (data + *offset));
			*offset += sizeof(short int);
			if(clients[plId].in_use == 1){
				clients[plId].x = *((float*) (data + *offset));
				*offset += sizeof(float);
				clients[plId].y = *((float*) (data + *offset));
				*offset += sizeof(float);
				//std::cout << "Receiving: " << plId << "_(x:" << clients[plId].x << ",y:" << clients[plId].y << ")\n";
			} else {
				std::cout << "Received update about unknown player.\n";
			}
		} break;
			
		case FLAG_SNUMBER: { // update about server player: float, float
			if(serverAsClient.in_use == 1){
				serverAsClient.x = *((float*) (data + *offset));
				*offset += sizeof(float);
				serverAsClient.y = *((float*) (data + *offset));
				*offset += sizeof(float);
				//std::cout << "Receiving: s_(x:" << serverAsClient.x << ",y:" << serverAsClient.y << ")\n";
			} else {
				std::cout << "Received update about inactive server.\n";
			}
		} break;
		
		case FLAG_NEWPLAYER: { // there's a new player: short int
			short int plId = *((short int*) (data + *offset));
			*offset += sizeof(short int);
			
			clients[plId].in_use = 1;
			clients[plId].x = 0;
			clients[plId].y = 0;
			
			std::cout << "New player has appeared.\n";
		} break;
		
		case FLAG_PLAYERQUIT: { // a player has left: short int
			short int plId = *((short int*) (data + *offset));
			*offset += sizeof(short int);
			
			clients[plId].in_use = 0;
			
			std::cout << "A player has quit.\n";
		} break;
		
		case FLAG_HELLO: { // server says hello: short int
			short int myId = *((short int*) (data + *offset));
			*offset += sizeof(short int);
			
			id = myId;
			
			std::cout << "Server says 'Hello'.\n";
		} break;
		
		default: { // unknown flag: no data
			std::cout << "Unrecognised flag recieved.\n";
			*offset -= 1;
		} break;
	}
}
 
//-----------------------------------------------------------------------------
void Client::InitNetwork(const char* pIP, int iPort) {
	IPaddress ip;
	if(SDLNet_ResolveHost(&ip, pIP, iPort) == -1) {
		fprintf(stderr, "%s\n", SDLNet_GetError());
		system("pause");
		exit(-1);
	}
 
	socket = SDLNet_TCP_Open(&ip);
	if(socket == NULL) {
		fprintf(stderr, "%s\n", SDLNet_GetError());
		system("pause");
		exit(-1);
	}
 
	socket_set = SDLNet_AllocSocketSet(1);
	if(socket_set == NULL) {
		fprintf(stderr, "%s\n", SDLNet_GetError());
		system("pause");
		exit(-1);
	}
 
	if(SDLNet_TCP_AddSocket(socket_set, socket) == -1) {
		fprintf(stderr, "%s\n", SDLNet_GetError());
		system("pause");
		exit(-1);
	}
	
	running = true;
}
 
//-----------------------------------------------------------------------------
bool Client::CheckSocket(void) {
	if(SDLNet_CheckSockets(socket_set, 0) == -1) {
		fprintf(stderr, "%s\n", SDLNet_GetError());
		system("pause");
		exit(-1);
	}
 
	return SDLNet_SocketReady(socket);
}

int Client::GetExternalData(short int ids[], float xs[], float ys[]){
	int num = 0;
	ids[num] = 0;
	xs[num] = serverAsClient.x;
	ys[num] = serverAsClient.y;
	num++;
	for(short int i=1; i<MAX_PACKETS; i++){
		if(clients[i].in_use == 1 && i != id){
			ids[num] = i;
			xs[num] = clients[i].x;
			ys[num] = clients[i].y;
			num++;
		}
	}
	return num;
}
