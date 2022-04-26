#include "App.hpp"

#include "Protocols.hpp"
#include "EventHandler.hpp"
#include "InstancedParent.hpp"
#include "Hud.hpp"
#include "Item.hpp"

#include "Player.hpp"
#include "World.hpp"
#include "River.hpp"
#include "Building.hpp"
#include "Bush.hpp"
#include "Tree.hpp"
#include "Wheelbarrow.hpp"

static void Private_InitGL();
static void Private_InitSDL(Uint32 flags);
static void Private_Render(const float cameraMatrix[4][4]);
static void Private_DrawObjects(bool drawWater);
static void Private_Update(float deltaTime);
static void Private_SetMat4x4Attribute(const GLuint& index, float matrix[4][4]);
SPStruct Private_BuildShaderProgram(GLuint vertexShader, GLuint fragmentShader, SPBuildStruct programBuild, int attributesN, int uniformsN){
	SPStruct ret;
	ret.attributesN = attributesN;
	ret.uniformsN = uniformsN;
	
	ret.handle = glCreateProgram();
	glAttachShader(ret.handle, vertexShader);
	glAttachShader(ret.handle, fragmentShader);
	
	// binding attribute locations BEFORE LINKING
	int location = 1; // we avoid location 0
	for(int i=0; i<attributesN; i++){
		glBindAttribLocation(ret.handle, location, programBuild.attribNames[i]);
		ret.attribLocations[i] = location;
		location += programBuild.attribSizes[i];
	}
	
	if(!LinkProgram(ret.handle)){
		std::cout << "ERROR: Program failed to link!" << std::endl;
	}
	glDetachShader(ret.handle, vertexShader);
	glDetachShader(ret.handle, fragmentShader);
	
	// finding uniform locations
	for(int i=0; i<uniformsN; i++){
		ret.uniformLocations[i] = glGetUniformLocation(ret.handle, programBuild.uniformNames[i]);
		if(ret.uniformLocations[i] == -1){
			std::cout << "ERROR: uniform not found." << std::endl;
		}
	}
	return ret;
}

int scaledPortWidth;
int scaledPortHeight;
float scaledPortWidthInverse;
float scaledPortHeightInverse;
int portWidth;
int portHeight;
float portWidthInverse;
float portHeightInverse;

static SDL_Window* window;
static SDL_GLContext context;

SmartArray<MAX_OBJECTS, Updated *> updatedArray = SmartArray<MAX_OBJECTS, Updated *>();
SmartArray<MAX_OBJECTS, RenderedParent *> renderedParentArray = SmartArray<MAX_OBJECTS, RenderedParent *>();
SmartArray<MAX_OBJECTS, Rendered *> renderedArray = SmartArray<MAX_OBJECTS, Rendered *>();

static GLuint vbo[BUFFERS_N]; // vertex buffer object handles
static GLuint shaderHandles[PROGRAMS_N * 2]; // shader handles
static SPStruct sps[PROGRAMS_N]; // shader programs

/*
 Texcoord are measured 0.0 to 1.0 from top left to bottom right:
 0,0	1,0
 
 
 0,1	1,1
 */

enum MainAttributes {m_a_position, m_a_normal, m_a_texCoord, m_a_world, m_a_worldInverseTranspose};
enum MainUniforms {m_u_waterHeight, m_u_portWidth, m_u_portHeight, m_u_halfOverTanHalfAngleOfView, m_u_viewProjection, m_u_lightWorldPos, m_u_viewInverse, m_u_lightColour, m_u_colourMult, m_u_texture, m_u_specular, m_u_shininess, m_u_specularFactor, m_u_water, m_u_reflectionTexture, m_u_riverNormalMapTexture, m_u_riverNormalMapOffset};
enum PickAttributes {p_a_position, p_a_world};
enum PickUniforms {p_u_waterHeight, p_u_viewProjection, p_u_id};

enum HudAttributes {h_a_position, h_a_texCoord, h_a_translation};
enum HudUniforms {h_u_texture};

static const short programAttributesN[PROGRAMS_N] = {5, 2, 3};
static const short programUniformsN[PROGRAMS_N] = {17, 3, 1};
static const SPBuildStruct programBuilds[PROGRAMS_N] = {
	{
		{"a_position", "a_normal", "a_texCoord", "a_world", "a_worldInverseTranspose"},
		{1, 1, 1, 4, 4},
		{"u_waterHeight", "u_portWidth", "u_portHeight", "u_halfOverTanHalfAngleOfView", "u_viewProjection", "u_lightWorldPos", "u_viewInverse", "u_lightColour", "u_colourMult", "u_texture", "u_specular", "u_shininess", "u_specularFactor", "u_water", "u_reflectionTexture", "u_riverNormalMapTexture", "u_riverNormalMapOffset"}
	},
	{
		{"a_position", "a_world"},
		{1, 4},
		{"u_waterHeight", "u_viewProjection", "u_id"}
	},
	{
		{"a_position", "a_texCoord", "a_translation"},
		{1, 1, 1},
		{"u_texture"}
	}
};
static const char * const shaderFileNames[PROGRAMS_N * 2] = {
	"./usr/share/Timberneer/assets/Shaders/vs_main.vert",
	"./usr/share/Timberneer/assets/Shaders/fs_main.frag",
	"./usr/share/Timberneer/assets/Shaders/vs_pick.vert",
	"./usr/share/Timberneer/assets/Shaders/fs_pick.frag",
	"./usr/share/Timberneer/assets/Shaders/vs_hud.vert",
	"./usr/share/Timberneer/assets/Shaders/fs_hud.frag"
};

static const GLintptr vertex_position_offset = 0 * sizeof(GLfloat);
static const GLintptr vertex_normal_offset = 3 * sizeof(GLfloat);
static const GLintptr vertex_texcoord_offset = 6 * sizeof(GLfloat);
static const GLsizei vertex_stride = 8 * sizeof(GLfloat);

static const GLintptr hud_vertex_position_offset = 0 * sizeof(GLfloat);
static const GLintptr hud_vertex_texcoord_offset = 3 * sizeof(GLfloat);
static const GLsizei hud_vertex_stride = 5 * sizeof(GLfloat);

static const GLfloat lightWorldPos[3] = {-40.0f, 60.0f, -40.0f};
static const GLfloat lightColour[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const GLfloat colourMultiplier[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const GLfloat specularColour[4] = {1.0f, 1.0f, 1.0f, 1.0f};

static GLuint reflectionTexture;
GLuint riverNormalMapTexture;

static float projectionMatrix[4][4];
static float pickProjectionMatrix[4][4];

static const float angleOfView = 0.77f;
static float aspectRatio;
static const float zNear = 0.1f - 0.0001f;
static const float zFar = 100.0f;

// pick constants
static const float pickZFar = 2.5f;
static float ppm_subWidth;
static float ppm_subHeight;
static float subLeft;
static float subBottom;

// game object stuff
extern Player *player;
extern const float waterHeight;
extern const float reflectionMatrix[4][4];
extern const vec4 waterClipPlane;
extern const int riverNormalMapSize;
extern uint8_t riverNormalMap[];
vec2 riverNormalMapOffset = {0.0f, 0.0f};
vec2 windVector;

// picking
static int hoveringIdNew = 0;
int hoveringId = 0;
/*
 hovering ids:
 0: nothing (or the ground, which is RenderedParent object of id 0)
 0xFF << 24: the river
 otherwise, < 0 or >= MAX_OBJECTS: nothing
 otherwise: id of instance of RenderedParent object of id id
 */

// instancing
extern const char *itemDropSrcs[ITEMS_N];
extern const char *buildingSrcs[BUILDINGS_N];
#define INSTANCED_PARENTS_N ( MAIN_IP_N + ITEMS_N + BUILDINGS_N )
InstancedParent *instancedParents[INSTANCED_PARENTS_N];

// hud
extern const vec3 hudPositions[MAX_HUD_OBJECTS];
// warnings
static const char *warningText[WARNINGS_N] = {
	"Insufficient items for crafting recipe",
	"Insufficient space for pile to get any bigger",
	"Holding incompatible items for crafting recipe",
	"Not holding correct tool",
	"Holding wrong number of tools"
};
static int warningTextHDs[WARNINGS_N];
static float warningTimer = 0.0f;
static const float warningTime = 2.0f; // seconds

void App_Init(){
	Private_InitGL();
	
	EventHandler_Init();
	
	windVector = 0.5f * V2_RandomUnit();
	
	// instanced parents
	// main instanced parents
	instancedParents[ip_cube] = new InstancedParent("./usr/share/Timberneer/assets/Data/cube.bin");
	instancedParents[ip_bush] = new InstancedParent("./usr/share/Timberneer/assets/Data/bush.bin");
	instancedParents[ip_berryBush] = new InstancedParent("./usr/share/Timberneer/assets/Data/berryBush.bin");
	instancedParents[ip_tree] = new InstancedParent("./usr/share/Timberneer/assets/Data/tree.bin");
	instancedParents[ip_treeStump] = new InstancedParent("./usr/share/Timberneer/assets/Data/treeStump.bin");
	instancedParents[ip_graveyard] = new InstancedParent("./usr/share/Timberneer/assets/Data/graveyard.bin");
	instancedParents[ip_undead] = new InstancedParent("./usr/share/Timberneer/assets/Data/undead.bin");
	// item instanced parents
	for(int i=0; i<ITEMS_N; i++) instancedParents[MAIN_IP_N + i] = new InstancedParent(itemDropSrcs[i]);
	// building instanced parents
	for(int i=0; i<BUILDINGS_N; i++) instancedParents[MAIN_IP_N + ITEMS_N + i] = new InstancedParent(buildingSrcs[i]);
	
	// individual constructors (mostly for hud data initialisation)
	Hud_Construct();
	Items_Construct();
	ItemDrop::Construct();
	Building_Construct();
	Wheelbarrow::Construct();
	WaterPurifier::Construct();
	Bush::Construct();
	Tree::Construct();
	River_Construct();
	
	// hud
	for(int i=0; i<WARNINGS_N; i++) warningTextHDs[i] = Hud_AddText(warningText[i], hp_message);
	
	World_Construct();
}
void App_Destruct(){
	World_Destruct();
	
	for(int p=0; p<INSTANCED_PARENTS_N; p++) delete instancedParents[p];
	
	Hud_Destruct();
	
	for(int i=0; i<PROGRAMS_N; i++){
		glDeleteProgram(sps[i].handle);
		glDeleteShader(shaderHandles[2*i + 0]);
		glDeleteShader(shaderHandles[2*i + 1]);
	}	
	glDeleteBuffers(BUFFERS_N, vbo);
	
	TTF_Quit();
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Private_InitSDL(Uint32 flags){
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	
	// setting OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	// turning on double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	// enabling multisampling for a nice antialiased effect
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	scaledPortWidth = dm.w;
	scaledPortHeight = dm.h;
	scaledPortWidthInverse = 1.0f / (float)scaledPortWidth;
	scaledPortHeightInverse = 1.0f / (float)scaledPortHeight;
	
	window = SDL_CreateWindow("Timberneer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scaledPortWidth, scaledPortHeight, flags);
	context = SDL_GL_CreateContext(window);
	
	SDL_GL_GetDrawableSize(window, &portWidth, &portHeight);
	
	std::cout << "Window opened at resolution " << portWidth << " / " << portHeight << std::endl;
	
	portWidthInverse = 1.0f / (float)portWidth;
	portHeightInverse = 1.0f / (float)portHeight;
	aspectRatio = (float)portWidth * portHeightInverse;
	ppm_subWidth = 0.002f * portWidthInverse;
	ppm_subHeight = 0.002f * portHeightInverse;
	subLeft = -0.001f * portHeightInverse;
	subBottom = -0.001f * portHeightInverse;
	
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Private_InitGL(){
	Uint32 contextFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP;
	Private_InitSDL(contextFlags);
	
	// projection matrix
	M4x4_Perspective(angleOfView, aspectRatio, zNear, zFar, projectionMatrix);
	
	// picking projection matrix
	M4x4_Frustum(subLeft, subLeft + ppm_subWidth, subBottom, subBottom + ppm_subHeight, zNear, pickZFar, pickProjectionMatrix);
	
	// Allocate and assign Vertex Buffer Objects to our handle
	glGenBuffers(BUFFERS_N, vbo);
	
	// shaders
	for(int i=0; i<PROGRAMS_N; i++){
		shaderHandles[2*i + 0] = glCreateShader(GL_VERTEX_SHADER);
		if(!CompileShader(shaderHandles[2*i + 0], shaderFileNames[2*i + 0])) return;
		shaderHandles[2*i + 1] = glCreateShader(GL_FRAGMENT_SHADER);
		if(!CompileShader(shaderHandles[2*i + 1], shaderFileNames[2*i + 1])) return;
		sps[i] = Private_BuildShaderProgram(shaderHandles[2*i + 0], shaderHandles[2*i + 1], programBuilds[i], programAttributesN[i], programUniformsN[i]);
	}
	
	// dealing with attribute 0
	for(int i=0; i<BUFFERS_N; i++){
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_position_offset); // 0
		glEnableVertexAttribArray(0);
	}
	
	// main shader program
	glUseProgram(sps[sp_main].handle); // must be called before setting attributes / uniforms
	glUniform1f(sps[sp_main].uniformLocations[m_u_waterHeight], waterHeight);
	glUniform1i(sps[sp_main].uniformLocations[m_u_portWidth], portWidth);
	glUniform1i(sps[sp_main].uniformLocations[m_u_portHeight], portHeight);
	glUniform1f(sps[sp_main].uniformLocations[m_u_halfOverTanHalfAngleOfView], 0.5f/tan(0.5f*angleOfView));
	glUniform3fv(sps[sp_main].uniformLocations[m_u_lightWorldPos], 1, lightWorldPos);
	glUniform4fv(sps[sp_main].uniformLocations[m_u_lightColour], 1, lightColour);
	glUniform4fv(sps[sp_main].uniformLocations[m_u_colourMult], 1, colourMultiplier);
	glUniform4fv(sps[sp_main].uniformLocations[m_u_specular], 1, specularColour); // specular colour
	glUniform1f(sps[sp_main].uniformLocations[m_u_shininess], 1000.0f); // shininess: 0 to 500, lower is more shiny
	glUniform1f(sps[sp_main].uniformLocations[m_u_specularFactor], 0.5f); // specular factor: 0 to 1*/
	glUniform1i(sps[sp_main].uniformLocations[m_u_texture], 0);
	glUniform1i(sps[sp_main].uniformLocations[m_u_reflectionTexture], 1);
	glUniform1i(sps[sp_main].uniformLocations[m_u_riverNormalMapTexture], 2);
	glGenTextures(1, &reflectionTexture);
	
	// pick shader program
	glUseProgram(sps[sp_pick].handle); // must be called before setting attributes / uniforms
	glUniform1f(sps[sp_pick].uniformLocations[p_u_waterHeight], waterHeight);
	
	// hud shader program
	glUseProgram(sps[sp_hud].handle); // must be called before setting attributes / uniforms
	glUniform1i(sps[sp_hud].uniformLocations[h_u_texture], 0);
	
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void Private_Render(const float cameraMatrix[4][4]){
	static uint8_t *frameBuffer = (uint8_t *)malloc(sizeof(uint8_t) * portWidth*portHeight*4);
	// matrices
	float viewMatrix[4][4];
	float viewProjectionMatrix[4][4];
	float pickViewProjectionMatrix[4][4];
	M4x4_Inverse(cameraMatrix, viewMatrix);
	M4x4_Multiply(projectionMatrix, viewMatrix, viewProjectionMatrix);
	M4x4_Multiply(pickProjectionMatrix, viewMatrix, pickViewProjectionMatrix);
	
	// ----- picking program; interactable objects -----
	glDisable(GL_BLEND);
	glViewport(0, 0, 1, 1);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(sps[sp_pick].handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vertex_buffer]);
	glVertexAttribPointer(sps[sp_pick].attribLocations[p_a_position], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_position_offset); // vbo needs to be bound for this
	glEnableVertexAttribArray(sps[sp_pick].attribLocations[p_a_position]);
	for(int i=0; i<4; i++) glDisableVertexAttribArray(sps[sp_pick].attribLocations[p_a_world] + i);
	
	// global, variable uniforms
	glUniformMatrix4fv(sps[sp_pick].uniformLocations[p_u_viewProjection], 1, GL_FALSE, &pickViewProjectionMatrix[0][0]);
	for(int i=0; i<renderedArray.GetN(); i++){
		if(!renderedArray[i]->active) continue;
		if(!renderedArray[i]->interactable) continue;
		
		const ObjectData objectData = renderedArray[i]->GetData();
		
		Private_SetMat4x4Attribute(sps[sp_pick].attribLocations[p_a_world], renderedArray[i]->worldMatrix);
		
		glUniform4fv(sps[sp_pick].uniformLocations[p_u_id], 1, (GLfloat *)renderedArray[i]->GetParentIdEncoded());
		
		glBufferData(GL_ARRAY_BUFFER, objectData.vertices_n * 8 * sizeof(GLfloat), objectData.vertices, GL_STATIC_DRAW);
		
		for(unsigned int d=0; d<objectData.divisionsN; d++){
			glDrawArrays(GL_TRIANGLES, objectData.divisionData[d].start, objectData.divisionData[d].count);
		}
	};
	// instanced objects
	for(int p=0; p<INSTANCED_PARENTS_N; p++){
		const ObjectData objectData = instancedParents[p]->GetData();
		glBufferData(GL_ARRAY_BUFFER, objectData.vertices_n * 8 * sizeof(GLfloat), objectData.vertices, GL_STATIC_DRAW); // vbo needs to be bound for this
		for(int i=0; i<instancedParents[p]->GetN(); i++){
			if(!(*instancedParents[p])[i]->active) continue;
			if(!(*instancedParents[p])[i]->interactable) continue;
			
			Private_SetMat4x4Attribute(sps[sp_pick].attribLocations[p_a_world], (*instancedParents[p])[i]->worldMatrix);

			glUniform4fv(sps[sp_pick].uniformLocations[p_u_id], 1, (GLfloat *)(*instancedParents[p])[i]->GetParentIdEncoded());
			
			for(unsigned int d=0; d<objectData.divisionsN; d++){
				glDrawArrays(GL_TRIANGLES, objectData.divisionData[d].start, objectData.divisionData[d].count);
			}
		}
	}
	uint8_t data[4];
	glReadPixels(0,			// GLint x
				 0,			// GLint y
				 1,			// GLsizei width
				 1,			// GLsizei height
				 GL_RGBA,	// GLenum format
				 GL_UNSIGNED_BYTE, // GLenum type
				 data		// void *data
				 );
	const uint32_t readId = (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
	hoveringIdNew = (int)readId;
	if(hoveringIdNew != hoveringId){
		if(hoveringId != 0){ // not nothing
			if(hoveringId == (0xFF << 24)){ // water
				River_StopHover();
			} else {
				RenderedParent *ptr = renderedParentArray(hoveringId);
				if(ptr) ptr->StopHover();
			}
		}
		if(hoveringIdNew != 0){ // not nothing
			if(hoveringIdNew == (0xFF << 24)){ // water
				River_StartHover();
			} else { // not nothing
				RenderedParent *ptr = renderedParentArray(hoveringIdNew);
				if(ptr) ptr->StartHover();
			}
		}
		hoveringId = hoveringIdNew;
	}
	glEnable(GL_BLEND);
	
	// drawing to screen
	glViewport(0, 0, portWidth, portHeight);
	
	// ----- main program; individual objects and instanced objects -----
	glUseProgram(sps[sp_main].handle); // must be called before setting uniforms
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vertex_buffer]);
	glVertexAttribPointer(sps[sp_main].attribLocations[m_a_position], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_position_offset); // vbo needs to be bound for this
	glVertexAttribPointer(sps[sp_main].attribLocations[m_a_normal], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_normal_offset); // vbo needs to be bound for this
	glVertexAttribPointer(sps[sp_main].attribLocations[m_a_texCoord], 2, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_texcoord_offset); // vbo needs to be bound for this
	glEnableVertexAttribArray(sps[sp_main].attribLocations[m_a_position]);
	glEnableVertexAttribArray(sps[sp_main].attribLocations[m_a_normal]);
	glEnableVertexAttribArray(sps[sp_main].attribLocations[m_a_texCoord]);
	for(int i=0; i<4; i++) glDisableVertexAttribArray(sps[sp_main].attribLocations[m_a_world] + i);
	for(int i=0; i<4; i++) glDisableVertexAttribArray(sps[sp_main].attribLocations[m_a_worldInverseTranspose] + i);
	
	// drawing reflected image
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float reflectionCameraMatrix[4][4];
	float reflectionViewMatrix[4][4];
	float reflectionProjectionMatrix[4][4];
	float reflectionViewProjectionMatrix[4][4];
	memcpy(reflectionCameraMatrix, cameraMatrix, 16 * sizeof(GLfloat));
	M4x4_PreMultiply(reflectionCameraMatrix, reflectionMatrix);
	M4x4_Inverse(reflectionCameraMatrix, reflectionViewMatrix);
	float reflectionViewInverseTransposeMatrix[4][4];
	M4x4_Transpose(reflectionCameraMatrix, reflectionViewInverseTransposeMatrix);
	vec4 reflectionClipPlane = M4x4_Multiply(reflectionViewInverseTransposeMatrix, waterClipPlane);
	memcpy(reflectionProjectionMatrix, projectionMatrix, 16 * sizeof(float));
	M4x4_ModifyProjectionNearClippingPlane(reflectionProjectionMatrix, reflectionClipPlane);
	M4x4_Multiply(reflectionProjectionMatrix, reflectionViewMatrix, reflectionViewProjectionMatrix);
	glUniformMatrix4fv(sps[sp_main].uniformLocations[m_u_viewProjection], 1, GL_FALSE, &reflectionViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(sps[sp_main].uniformLocations[m_u_viewInverse], 1, GL_FALSE, &reflectionCameraMatrix[0][0]); // view inverse
	glCullFace(GL_FRONT);
	Private_DrawObjects(false);
	glCullFace(GL_BACK);
	// storing reflected image in the reflection texture
	glReadPixels(0, 0, portWidth, portHeight, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, portWidth, portHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer);
	
	// drawing final image
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(sps[sp_main].uniformLocations[m_u_viewProjection], 1, GL_FALSE, &viewProjectionMatrix[0][0]);
	glUniformMatrix4fv(sps[sp_main].uniformLocations[m_u_viewInverse], 1, GL_FALSE, &cameraMatrix[0][0]); // view inverse
	glUniform2f(sps[sp_main].uniformLocations[m_u_riverNormalMapOffset], riverNormalMapOffset.x, riverNormalMapOffset.y);
	Private_DrawObjects(true);
	
	
	// ----- hud program; 2d heads up display -----
	glUseProgram(sps[sp_hud].handle); // must be called before setting uniforms
	glBindBuffer(GL_ARRAY_BUFFER, vbo[vertex_buffer]);
	glVertexAttribPointer(sps[sp_hud].attribLocations[h_a_position], 3, GL_FLOAT, GL_FALSE, hud_vertex_stride, (GLvoid *)hud_vertex_position_offset); // vbo needs to be bound for this
	glVertexAttribPointer(sps[sp_hud].attribLocations[h_a_texCoord], 2, GL_FLOAT, GL_FALSE, hud_vertex_stride, (GLvoid *)hud_vertex_texcoord_offset); // vbo needs to be bound for this
	glEnableVertexAttribArray(sps[sp_hud].attribLocations[h_a_position]);
	glEnableVertexAttribArray(sps[sp_hud].attribLocations[h_a_texCoord]);
	glDisableVertexAttribArray(sps[sp_hud].attribLocations[h_a_translation]);
	glActiveTexture(GL_TEXTURE0);
	for(int i=0; i<HUD_FIXED_POSITIONS; i++){
		if(!Hud_GetActive(i)) continue;
		
		const HudData hudData = Hud_GetData(i);
		const vec3 translation = hudPositions[i];
		
		glVertexAttrib3f(sps[sp_hud].attribLocations[h_a_translation], translation.x, translation.y, translation.z);
		glBufferData(GL_ARRAY_BUFFER, hudData.vertices_n * hud_vertex_stride, hudData.vertices, GL_STATIC_DRAW); // vbo needs to be bound for this
		glBindTexture(GL_TEXTURE_2D, hudData.texture);
		glDrawArrays(GL_TRIANGLES, 0, hudData.vertices_n);
	}
	
	SDL_GL_SwapWindow(window);
}

void Private_DrawObjects(bool drawWater){
	glUniform1i(sps[sp_main].uniformLocations[m_u_water], drawWater);
	
	float m[4][4];
	float worldInverseTransposeMatrix[4][4];
	
	// individual objects
	for(int i=0; i<renderedArray.GetN(); i++){
		if(!renderedArray[i]->active) continue;
		
		const ObjectData objectData = renderedArray[i]->GetData();
		
		M4x4_Inverse(renderedArray[i]->worldMatrix, m);
		M4x4_Transpose(m, worldInverseTransposeMatrix);
		
		Private_SetMat4x4Attribute(sps[sp_main].attribLocations[m_a_world], renderedArray[i]->worldMatrix);
		Private_SetMat4x4Attribute(sps[sp_main].attribLocations[m_a_worldInverseTranspose], worldInverseTransposeMatrix);
		
		glBufferData(GL_ARRAY_BUFFER, objectData.vertices_n * vertex_stride, objectData.vertices, GL_STATIC_DRAW); // vbo needs to be bound for this
		glActiveTexture(GL_TEXTURE0);
		for(unsigned int d=0; d<objectData.divisionsN; d++){
			glBindTexture(GL_TEXTURE_2D, objectData.divisionData[d].texture);
			if(drawWater){
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, reflectionTexture);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, riverNormalMapTexture);
				glActiveTexture(GL_TEXTURE0);
			}
			glDrawArrays(GL_TRIANGLES, objectData.divisionData[d].start, objectData.divisionData[d].count);
		}
	}
	
	// instanced objects
	for(int p=0; p<INSTANCED_PARENTS_N; p++){
		const ObjectData objectData = instancedParents[p]->GetData();
		glBufferData(GL_ARRAY_BUFFER, objectData.vertices_n * vertex_stride, objectData.vertices, GL_STATIC_DRAW); // vbo needs to be bound for this
		for(int i=0; i<instancedParents[p]->GetN(); i++){
			if(!(*instancedParents[p])[i]->active) continue;
			
			M4x4_Inverse((*instancedParents[p])[i]->worldMatrix, m);
			M4x4_Transpose(m, worldInverseTransposeMatrix);
			
			Private_SetMat4x4Attribute(sps[sp_main].attribLocations[m_a_world], (*instancedParents[p])[i]->worldMatrix);
			Private_SetMat4x4Attribute(sps[sp_main].attribLocations[m_a_worldInverseTranspose], worldInverseTransposeMatrix);
			
			glActiveTexture(GL_TEXTURE0);
			for(unsigned int d=0; d<objectData.divisionsN; d++){
				glBindTexture(GL_TEXTURE_2D, objectData.divisionData[d].texture);
				if(drawWater){
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, reflectionTexture);
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, riverNormalMapTexture);
					glActiveTexture(GL_TEXTURE0);
				}
				glDrawArrays(GL_TRIANGLES, objectData.divisionData[d].start, objectData.divisionData[d].count);
			}
		}
	}
}

void App_Loop(){
	int time, deltaTime;
	int prevTime = SDL_GetTicks();
	float dT;
	while(true){
		// handling events
		if(EventHandler_HandleEvents()) return;
		
		// updating
		time = SDL_GetTicks();
		deltaTime = time - prevTime;
		if(deltaTime > 0){
			prevTime = time;
			if(deltaTime < TICKS_PER_FRAME){
				SDL_Delay(TICKS_PER_FRAME - deltaTime);
				deltaTime = TICKS_PER_FRAME;
			}
			dT = (float)deltaTime * 0.001f;
			Private_Update(dT);
		}
		
		// rendering
		float cameraTransform[4][4];
		player->CameraTransform(cameraTransform);
		Private_Render(cameraTransform);
	}
}

void Private_Update(float deltaTime){
	if(warningTimer){
		warningTimer -= deltaTime;
		if(warningTimer <= 0.0f){
			Hud_Set(hp_message, -1);
			warningTimer = 0.0f;
		}
	}
	riverNormalMapOffset += deltaTime * windVector;
	for(int i=0; i<updatedArray.GetN(); i++){
		updatedArray[i]->Update(deltaTime);
	}
}

static void Private_SetMat4x4Attribute(const GLuint& index, float matrix[4][4]){
	glVertexAttrib4fv(index + 0, matrix[0]);
	glVertexAttrib4fv(index + 1, matrix[1]);
	glVertexAttrib4fv(index + 2, matrix[2]);
	glVertexAttrib4fv(index + 3, matrix[3]);
}

void App_DisplayWarning(WarningEnum warning){
	warningTimer = warningTime;
	Hud_Set(hp_message, warningTextHDs[warning]);
}
