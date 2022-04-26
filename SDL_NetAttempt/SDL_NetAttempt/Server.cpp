#include "Server.hpp"

Server::Server() : Network() {
	// initialise SDL subsystems
	if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_EVENTS) != 0) {
		fprintf(stderr, "ER: SDL_Init: %s\n", SDL_GetError());
		exit(-1);
	}
	
	// initialise SDLNet subsystems
	if(SDLNet_Init() == -1) {
		fprintf(stderr, "ER: SDLNet_Init: %s\n", SDLNet_GetError());
		exit(-1);
	}
	
	// open server’s TCP socket on port 3211
	if(SDLNet_ResolveHost(&ip, NULL, 3211) == -1) {
		fprintf(stderr, "ER: SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(-1);
	}
	/* use the IP as a Uint8[4] */
	Uint8 *ipaddr=(Uint8*)&ip.host;
	/* output the IP address nicely */
	printf("IP Address : %d.%d.%d.%d\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
	
	server_socket = SDLNet_TCP_Open(&ip);
	if(server_socket == NULL) {
		fprintf(stderr, "ER: SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(-1);
	}
	
	// setting up a socket set
	socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS+1); // the +1 is to account for the server socket, the others sockets are for clients
	if(socket_set == NULL) {
		fprintf(stderr, "ER: SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		exit(-1);
	}
	// adding 'server_socket' to the socket set
	if(SDLNet_TCP_AddSocket(socket_set, server_socket) == -1) {
		fprintf(stderr, "ER: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
		exit(-1);
	}
	
	id = 0;
	running = true;
}

void Server::Send(){
	int _offset;
	int i;
	uint8_t *array = (uint8_t*) malloc(sizeof(short int) + 2 * sizeof(float));
	short int otherInd;
	for(i=0; i<MAX_SOCKETS; i++) {
		if(clients[i].in_use){
			_offset = 0;
			array = (uint8_t*) malloc(2 * sizeof(float));
			memcpy(array + _offset, reinterpret_cast<uint8_t*>(&x), sizeof(float));
			_offset += sizeof(float);
			memcpy(array + _offset, reinterpret_cast<uint8_t*>(&y), sizeof(float));
			_offset += sizeof(float);
			SendData(i, array, _offset, FLAG_SNUMBER);
			//std::cout << "Sending " << i << ": me_(x:" << x << ",y:" << y << ")\n";
			free(array);
			for(otherInd=0; otherInd<MAX_SOCKETS; otherInd++){
				if(clients[otherInd].in_use == 1 && i != otherInd){
					_offset = 0;
					array = (uint8_t*) malloc(sizeof(short int) + 2 * sizeof(float));
					//array = (uint8_t*)(&otherInd);
					memcpy(array + _offset, reinterpret_cast<uint8_t*>(&otherInd), sizeof(short int));
					_offset += sizeof(short int);
					memcpy(array + _offset, reinterpret_cast<uint8_t*>(&clients[otherInd].x), sizeof(float));
					_offset += sizeof(float);
					memcpy(array + _offset, reinterpret_cast<uint8_t*>(&clients[otherInd].y), sizeof(float));
					_offset += sizeof(float);
					SendData(i, array, _offset, FLAG_NUMBER);
					//std::cout << "Sending " << i << ": " << otherInd << "_(x:" << clients[otherInd].x << ",y:" << clients[otherInd].y << ")\n";
					free(array);
				}
			}
		}
	}
}

void Server::Recv() {
	int num = SDLNet_CheckSockets(socket_set, 10);
	
	// checking server socket for new connections
	if(SDLNet_SocketReady(server_socket)) {
		int got_socket = AcceptSocket(next_ind);
		if(got_socket) {
			NewPlayer(next_ind);
			std::cout << "New connection, id: " << next_ind << "\n";
			
			// Get a new index
			int chk_count;
			for(chk_count=0; chk_count<MAX_SOCKETS; ++chk_count) {
				if(sockets[(next_ind+chk_count)%MAX_SOCKETS] == NULL) break;
			}
			next_ind = (next_ind+chk_count)%MAX_SOCKETS;
		}
	}
	// checking client sockets for data they've sent
	int ind;
	for(ind=0; (ind<MAX_SOCKETS); ++ind) {
		if(sockets[ind] == NULL) continue;
		if(!SDLNet_SocketReady(sockets[ind])) continue;
	 
		uint16_t length;
		uint8_t *data = RecvData(ind, &length);
		if(data != NULL) {
			uint16_t offset = 0;
			while(offset < length) {
				ProcessData(ind, data, &offset);
			}
			free(data);
		} else {
			std::cout << "NULL data received.\n";
		}
	}
}

Server::~Server(){
	if(SDLNet_TCP_DelSocket(socket_set, server_socket) == -1) {
		fprintf(stderr, "ER: SDLNet_TCP_DelSocket: %s\n", SDLNet_GetError());
		exit(-1);
	} SDLNet_TCP_Close(server_socket);
	 
	int i;
	for(i=0; i<MAX_SOCKETS; ++i) {
		if(sockets[i] == NULL) continue;
		CloseSocket(i);
	}
	
	running = false;
	SDLNet_FreeSocketSet(socket_set);
	SDLNet_Quit();
}







void Server::CloseSocket(int index) {
	if(sockets[index] == NULL) {
		fprintf(stderr, "ER: Attempted to delete a NULL socket.\n");
		return;
	}
	if(SDLNet_TCP_DelSocket(socket_set, sockets[index]) == -1) {
		fprintf(stderr, "ER: SDLNet_TCP_DelSocket: %s\n", SDLNet_GetError());
		exit(-1);
	}
	memset(&clients[index], 0x00, sizeof(OtherClient));
	SDLNet_TCP_Close(sockets[index]);
	sockets[index] = NULL;
}

int Server::AcceptSocket(int index) {
	if(sockets[index]) {
		fprintf(stderr, "ER: Overriding socket at index %d.\n", index);
		CloseSocket(index);
	}
 
	sockets[index] = SDLNet_TCP_Accept(server_socket);
	if(sockets[index] == NULL) {
		printf("ER: Accepted NULL socket.\n");
		return 0;
	}
	
	if(SDLNet_TCP_AddSocket(socket_set, sockets[index]) == -1) {
		fprintf(stderr, "ER: SDLNet_TCP_AddSocket: %s\n", SDLNet_GetError());
		exit(-1);
	}
	return 1;
}

uint8_t* Server::RecvData(int index, uint16_t* length) {
	uint8_t temp_data[MAX_PACKETS];
	int num_recv = SDLNet_TCP_Recv(sockets[index], temp_data, MAX_PACKETS);
 
	if(num_recv <= 0) {
		CloseSocket(index);
 
		const char* err = SDLNet_GetError();
		if(strlen(err) == 0) {
			printf("DB: client disconnected\n");
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

void Server::SendData(int index, uint8_t* data, uint16_t length, uint16_t flag) {
	uint8_t temp_data[MAX_PACKETS];
 
	int offset = 0;
	memcpy(temp_data+offset, &flag, sizeof(uint16_t));
	offset += sizeof(uint16_t);
	memcpy(temp_data+offset, data, length);
	offset += length;
	int num_sent = SDLNet_TCP_Send(sockets[index], temp_data, offset);
	if(num_sent < offset) {
		fprintf(stderr, "ER: SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		CloseSocket(index);
	}
}

void Server::ProcessData(int index, uint8_t* data, uint16_t* offset) {
	if(data == NULL) return;
 
	uint16_t flag = *(uint16_t*) &data[*offset];
	*offset += sizeof(uint16_t);
 
	switch(flag) {
		case FLAG_QUIT: {
			std::cout << "Client:" << index << " has quit.\n";
			clients[index].in_use = 0;
			PlayerQuit(index);
		} break;
		
		case FLAG_NUMBER: {
			clients[index].x = *((float*) (data + *offset));
			*offset += sizeof(float);
			clients[index].y = *((float*) (data + *offset));
			*offset += sizeof(float);
			//std::cout << "Receiving: " << index << "_(x:" << clients[index].x << ",y:" << clients[index].y << ")\n";
		} break;
		
		default: {
			std::cout << "Unrecognised flag recieved.\n";
			*offset -= 1;
		} break;
	}
}


void Server::NewPlayer(short int ind) {
	// Initialise new player data;
	clients[ind].in_use = 1;
	clients[ind].x = 0;
	clients[ind].y = 0;
	
	int offset = 0;
	uint8_t *array;
	array = (uint8_t*)(&ind);
	offset += sizeof(short int);
	
	int _offset = 0;
	uint8_t *_array;
	
	for(short int i=0; i<MAX_SOCKETS; i++) {
		if(clients[i].in_use && i != ind){
			// Tell all the other players about this new one
			SendData(i, array, offset, FLAG_NEWPLAYER);
			// Tell new player about this other one
			_offset = 0;
			_array = (uint8_t*)(&i);
			_offset += sizeof(short int);
			SendData(ind, _array, _offset, FLAG_NEWPLAYER);
		}
	}
	
	// Say hello to new player
	_offset = 0;
	_array = (uint8_t*)(&ind);
	_offset += sizeof(short int);
	SendData(ind, _array, _offset, FLAG_HELLO);
	
	//new player active
	clients[ind].in_use = 1;
}

void Server::PlayerQuit(short int ind) {
	int offset = 0;
	uint8_t *array;
	array = (uint8_t*)(&ind);
	offset += sizeof(short int);
	for(int i=0; i<MAX_SOCKETS; i++) {
		if(clients[i].in_use && i != ind){
			SendData(i, array, offset, FLAG_PLAYERQUIT);
		}
	}
}

int Server::GetExternalData(short int ids[], float xs[], float ys[]){
	int num = 0;
	for(short int i=0; i<MAX_PACKETS; i++){
		if(clients[i].in_use == 1){
			ids[num] = i;
			xs[num] = clients[i].x;
			ys[num] = clients[i].y;
			num++;
		}
	}
	return num;
}
