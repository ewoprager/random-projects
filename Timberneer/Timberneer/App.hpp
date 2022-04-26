#ifndef App_hpp
#define App_hpp

#include "Header.hpp"

#define TICKS_PER_FRAME 17

#define MAX_OBJECTS 1024

#define MAIN_IP_N 7
enum IPMainEnum {ip_cube, ip_bush, ip_berryBush, ip_tree, ip_treeStump, ip_graveyard, ip_undead};

#define BUFFERS_N 1
enum VBOEnum {vertex_buffer};

#define WARNINGS_N 5
enum WarningEnum {w_insufficient_items, w_no_pile_room, w_holding_wrong, w_wrong_tool, w_wrong_tool_number};

#define MAX_SHADER_CONSTANTS 30
struct SPBuildStruct {
	const char *attribNames[MAX_SHADER_CONSTANTS];
	const int attribSizes[MAX_SHADER_CONSTANTS];
	const char *uniformNames[MAX_SHADER_CONSTANTS];
};
struct SPStruct {
	GLuint handle;
	GLint attribLocations[MAX_SHADER_CONSTANTS];
	GLint uniformLocations[MAX_SHADER_CONSTANTS];
	unsigned char attributesN, uniformsN;
};

#define PROGRAMS_N 3
enum SPEnum {sp_main, sp_pick, sp_hud};

void App_Init();
void App_Loop();
void App_Destruct();
void App_DisplayWarning(WarningEnum warning);

#endif /* App_hpp */
