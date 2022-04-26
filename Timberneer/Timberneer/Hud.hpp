#ifndef Hud_hpp
#define Hud_hpp

#include "Header.hpp"

/*
 HUD objects are draw directly into the clip space
 x and y between -1.0 and 1.0
 x +ve right
 y +ve up
 (0, 0) in centre of screen
 z is scaled in shader, set it as follows:
 z between 0.0 and 1.0
 z +ve into the screen 
 */

#define HUD_CROSSHAIR_DEPTH 1.0f
#define HUD_TEXT_DEPTH 0.7f
#define HUD_MENU_DEPTH 0.5f
#define HUD_MENU_TEXT_DEPTH 0.45f

#define HUD_FIXED_POSITIONS 22
#define HUD_VARIABLE_POSITIONS 64
#define MAX_HUD_OBJECTS (HUD_FIXED_POSITIONS+HUD_VARIABLE_POSITIONS)

enum HudPos {hp_crosshair, hp_primaryWord, hp_primary, hp_secondaryWord, hp_secondary, hp_holdingNo, hp_info, hp_infoNo, hp_beltNoWord1, hp_beltNo1, hp_belt1, hp_beltNoWord2, hp_beltNo2, hp_belt2, hp_beltNoWord3, hp_beltNo3, hp_belt3, hp_beltNoWord4, hp_beltNo4, hp_belt4, hp_message, hp_menu};

struct HudData {
	unsigned int vertices_n;
	GLfloat *vertices;
	GLuint texture;
};

void Hud_Construct();
void Hud_Destruct();

void Hud_Set(const int& position, const int& dataIndex);
void Hud_Reset(const int& position, const int& oldDataIndex);

HudData Hud_GetData(const int& position);
bool Hud_GetActive(const int& position);
int Hud_GetIndex(const int& position);

int Hud_AddText(const char *text, int position, bool active=false);
int Hud_Add(HudData hudData, int activePosition=-1);
void Hud_Replace(const int& index, HudData hudData, int position=-1);
void Hud_ReplaceText(const int& index, const char *text, int position, bool active=false);
int Hud_GetN();

void MakeHudRect(float width, float height, float out[30], Align hAlign=al_raggedPositive, Align vAlign=al_raggedPositive);
HudData MakeHudDataFromTexture(GLint texture, Align hAlign=al_raggedPositive, Align vAlign=al_raggedPositive, float width=0.0f, float height=0.0f, bool givenInPixels=false); // need to free returned HudData::vertices

#endif /* Hud_hpp */
