#include "Hud.hpp"
#include "Player.hpp"

TTF_Font *textFont;
extern const SDL_Color textColour = {255, 0, 0};

extern const int portWidth;
extern const int portHeight;
extern const int scaledPortWidth;
extern const int scaledPortHeight;
extern const float scaledPortWidthInverse;
extern const float scaledPortHeightInverse;

static int hudPositionIndex[MAX_HUD_OBJECTS];
vec3 hudPositions[MAX_HUD_OBJECTS];

static HudData hudDataArray[MAX_HUD_OBJECTS];
static int hudDataN = 0;

// fixed texture:
static int primaryWord, secondaryWord;
static int beltNoWord[Player::beltSlotsN];

extern const Align hudFixedHAlign[HUD_FIXED_POSITIONS] = {
	al_centered,
	
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	
	al_raggedNegative,
	
	al_raggedNegative,
	al_raggedNegative,
	
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	
	al_raggedNegative,
	
	al_centered
};
extern const Align hudFixedVAlign[HUD_FIXED_POSITIONS] = {
	al_centered,
	
	al_raggedNegative,
	al_raggedNegative,
	al_raggedNegative,
	al_raggedNegative,
	
	al_raggedPositive,
	
	al_raggedPositive,
	al_raggedPositive,
	
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	al_raggedPositive,
	
	al_raggedNegative,
	
	al_centered
};

void Hud_Construct(){
	textFont = TTF_OpenFont("./usr/share/Timberneer/assets/Data/cmu.serif-roman.ttf", 16 * portHeight / scaledPortHeight);
	if(!textFont) printf("TTF_OpenFont: %s\n", TTF_GetError());
	
	for(int i=0; i<MAX_HUD_OBJECTS; i++){
		hudPositionIndex[i] = -1;
	}
	
	const float dpwi = 2.0f * scaledPortWidthInverse;
	const float dphi = 2.0f * scaledPortHeightInverse;
	const float border = 24.0f;
	const float gap = 16.0f;
	const float textHeight = 32.0f;
	
	// fixed position elements:
	hudPositions[hp_crosshair] = {0.0f, 0.0f, HUD_CROSSHAIR_DEPTH};
	hudPositions[hp_primaryWord] = {0.0f, 0.0f, HUD_TEXT_DEPTH};
	hudPositions[hp_primary] = {192.0f*dpwi, 0.0f, HUD_TEXT_DEPTH};
	hudPositions[hp_secondaryWord] = {0.0f, -(textHeight + gap)*dphi, HUD_TEXT_DEPTH};
	hudPositions[hp_secondary] = {192.0f*dpwi, -(textHeight + gap)*dphi, HUD_TEXT_DEPTH};
	
	hudPositions[hp_holdingNo] = {0.5f, -1.0f + border*dphi, HUD_TEXT_DEPTH};
	
	hudPositions[hp_info] = {1.0f - border*dpwi, -1.0f + (border + textHeight + gap)*dphi, HUD_TEXT_DEPTH};
	hudPositions[hp_infoNo] = {1.0f - border*dpwi, -1.0f + border*dphi, HUD_TEXT_DEPTH};
	
	const vec3 beltHudDelta = {0.0f, (gap + textHeight)*dphi, 0.0f};
	hudPositions[hp_beltNoWord1] = {-1.0f + border*dpwi, -1.0f + border*dphi, HUD_TEXT_DEPTH};
	hudPositions[hp_beltNo1] = {-1.0f + (border + 32.0f + gap)*dpwi, -1.0f + border*dphi, HUD_TEXT_DEPTH};
	hudPositions[hp_belt1] = {-1.0f + (border + 32.0f + gap + 32.0f + gap)*dpwi, -1.0f + border*dphi, HUD_TEXT_DEPTH};
	for(int i=1; i<Player::beltSlotsN; i++){
		hudPositions[hp_beltNoWord1 + i*3] = hudPositions[hp_beltNoWord1] + i*beltHudDelta;
		hudPositions[hp_beltNo1 + i*3] = hudPositions[hp_beltNo1] + i*beltHudDelta;
		hudPositions[hp_belt1 + i*3] = hudPositions[hp_belt1] + i*beltHudDelta;
	}
	
	hudPositions[hp_message] = {1.0f - border*dpwi, 1.0f - border*dphi, HUD_TEXT_DEPTH};
	
	hudPositions[hp_menu] = {0.0f, 0.0f, HUD_MENU_DEPTH};
	
	// fixed texture elements:
	Hud_Add(MakeHudDataFromTexture(LoadTexture("./usr/share/Timberneer/assets/Data/crosshair.png"), hudFixedHAlign[hp_crosshair], hudFixedVAlign[hp_crosshair], 64.0f, 64.0f, true), hp_crosshair);
	primaryWord = Hud_AddText("Primary: ", hp_primaryWord);
	secondaryWord = Hud_AddText("Secondary: ", hp_secondaryWord);
	for(int i=0; i<Player::beltSlotsN; i++){
		char str[10];
		sprintf(str, "%d:", i+1);
		beltNoWord[i] = Hud_Add(MakeHudDataFromTexture(MakeTextTexture(str), hudFixedHAlign[hp_beltNoWord1 + 3*i], hudFixedVAlign[hp_beltNoWord1 + 3*i]));
	}
}
void Hud_Destruct(){
	for(int i=0; i<hudDataN; i++) free(hudDataArray[i].vertices);
	TTF_CloseFont(textFont);
}

void Hud_Set(const int& position, const int& dataIndex){
	hudPositionIndex[position] = dataIndex;
	bool reset = (dataIndex < 0);
	if(position == hp_primary){
		hudPositionIndex[hp_primaryWord] = reset ? -1 : primaryWord;
		return;
	}
	if(position == hp_secondary){
		hudPositionIndex[hp_secondaryWord] = reset ? -1 : secondaryWord;
		return;
	}
	for(int i=0; i<Player::beltSlotsN; i++){
		if(position == hp_belt1 + i*3){
			hudPositionIndex[hp_beltNoWord1 + 3*i] = reset ? -1 : beltNoWord[i];
			return;
		}
	}
}
void Hud_Reset(const int& position, const int& oldDataIndex){
	if(hudPositionIndex[position] != oldDataIndex) return;
	hudPositionIndex[position] = -1;
	if(position == hp_primary){
		hudPositionIndex[hp_primaryWord] = -1;
		return;
	}
	if(position == hp_secondary){
		hudPositionIndex[hp_secondaryWord] = -1;
		return;
	}
	for(int i=0; i<Player::beltSlotsN; i++){
		if(position == hp_belt1 + i*3){
			hudPositionIndex[hp_beltNoWord1 + 3*i] = -1;
			return;
		}
	}
}
bool Hud_GetActive(const int& position){ return (hudPositionIndex[position] >= 0); }
HudData Hud_GetData(const int& position){ return hudDataArray[hudPositionIndex[position]]; }
int Hud_GetIndex(const int& position){ return hudPositionIndex[position]; }

int Hud_AddText(const char *text, int position, bool active){
	int ret = Hud_Add(MakeHudDataFromTexture(MakeTextTexture(text), hudFixedHAlign[position], hudFixedVAlign[position]));
	if(active) Hud_Set(position, ret);
	return ret;
}
int Hud_Add(HudData hudData, int activePosition){
	if(hudDataN >= MAX_HUD_OBJECTS){
		std::cout << "ERROR: Max hud objects reached." << std::endl;
		return -1;
	}
	hudDataArray[hudDataN] = hudData;
	if(activePosition >= 0) Hud_Set(activePosition, hudDataN);
	return hudDataN++;
}
void Hud_Replace(const int& index, HudData hudData, int position){
	if(index < hudDataN) free(hudDataArray[index].vertices);
	hudDataArray[index] = hudData;
	if(position >= 0) Hud_Set(position, index);
}
void Hud_ReplaceText(const int& index, const char *text, int position, bool active){
	if(index < hudDataN) free(hudDataArray[index].vertices);
	hudDataArray[index] = MakeHudDataFromTexture(MakeTextTexture(text), hudFixedHAlign[position], hudFixedVAlign[position]);
	if(active) Hud_Set(position, index);
}
int Hud_GetN(){ return hudDataN; }

void MakeHudRect(float width, float height, float out[30], Align hAlign, Align vAlign){
	float hShift = width * (float)hAlign * -0.5f;
	float vShift = height * (float)vAlign * -0.5f;
	// top left
	// position xyz
	out[0] = out[15] = hShift;
	out[1] = out[16] = vShift + height;
	out[2] = out[17] = 0.0f;
	// texCoord xy
	out[3] = out[18] = 0.0f;
	out[4] = out[19] = 0.0f;
	
	// bottom left
	// position xyz
	out[5] = hShift;
	out[6] = vShift;
	out[7] = 0.0f;
	// texCoord xy
	out[8] = 0.0f;
	out[9] = 1.0f;
	
	// bottom right
	// position xyz
	out[10] = out[20] = hShift + width;
	out[11] = out[21] = vShift;
	out[12] = out[22] = 0.0f;
	// texCoord xy
	out[13] = out[23] = 1.0f;
	out[14] = out[24] = 1.0f;
	
	// top right
	// position xyz
	out[25] = hShift + width;
	out[26] = vShift + height;
	out[27] = 0.0f;
	// texCoord xy
	out[28] = 1.0f;
	out[29] = 0.0f;
}
HudData MakeHudDataFromTexture(GLint texture, Align hAlign, Align vAlign, float width, float height, bool givenInPixels){
	if(givenInPixels){
		if(width) width *= 2.0f*scaledPortWidthInverse;
		if(height) height *= 2.0f*scaledPortHeightInverse;
	}
	HudData ret;
	ret.vertices_n = 6;
	ret.vertices = (GLfloat *)malloc(30 * sizeof(GLfloat));
	ret.texture = texture;
	if(!width || !height){
		glBindTexture(GL_TEXTURE_2D, texture);
		if(!width){
			GLint w;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
			width = 2.0f * (float)w * scaledPortWidthInverse;
		}
		if(!height){
			GLint h;
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
			height = 2.0f * (float)h * scaledPortHeightInverse;
		}
	}
	MakeHudRect(width, height, ret.vertices, hAlign, vAlign);
	return ret;
}
