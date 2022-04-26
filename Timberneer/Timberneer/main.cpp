#include "App.hpp"

int main(int argc, char *argv[]){
	App_Init();
	
	App_Loop();
	
	App_Destruct();
	
	return 0;
}
