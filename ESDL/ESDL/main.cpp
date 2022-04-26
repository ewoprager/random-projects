#include <iostream>
#include "ESDL_EventHandler.hpp"
#include "ESDL_OpenGL.hpp"

SDL_Window *window;

int scaledPortWidth;
int scaledPortHeight;
float scaledPortWidthInverse;
float scaledPortHeightInverse;
int portWidth;
int portHeight;
float portWidthInverse;
float portHeightInverse;

int lightPortWidth = 4096;
int lightPortHeight = 4096;

static const float angleOfView = 0.77f;
static float aspectRatio;
static const float zNear = 0.1f;
static const float zFar = 50.0f;

static float ppm_subWidth;
static float ppm_subHeight;
static float subLeft;
static float subBottom;

static const GLintptr vertex_position_offset = 0 * sizeof(GLfloat);
static const GLintptr vertex_normal_offset = 3 * sizeof(GLfloat);
static const GLintptr vertex_colour_offset = 6 * sizeof(GLfloat);
static const GLsizei vertex_stride = 9 * sizeof(GLfloat);

static const GLfloat lightWorldPos[3] = {-16.0f, 12.0f, -16.0f};
static const GLfloat lightColour[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const GLfloat colourMultiplier[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static const GLfloat specularColour[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLuint vbo;

static float projectionMatrix[4][4];

float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
float x = 0.0f;
float y = 0.0f;
float z = 4.0f;

float objectYaw = 0.0f;
float objectPitch = 0.0f;

GLfloat vertices[12*9] = {
	1.0f, 0.0f, -0.5f,		0.0f, 0.0f, -1.0f,			1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,			0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,			0.0f, 0.0f, 1.0f,
	
	0.0f, 0.0f, 1.0f,		0.577f, 0.577f, 0.577f,		1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, -0.5f,		0.577f, 0.577f, 0.577f,		1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f,		0.577f, 0.577f, 0.577f,		0.0f, 0.0f, 1.0f,
	
	0.0f, 0.0f, 1.0f,		0.577f, -0.577f, 0.577f,	1.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f,		0.577f, 0.577f, 0.577f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,	0.577f, 0.577f, 0.577f,		0.0f, 1.0f, 0.0f,
	
	0.0f, 0.0f, 1.0f,		-0.707f, 0.0f, 0.707f,		1.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,	-0.707f, 0.0f, 0.707f,		0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, -0.5f,		-0.707f, 0.0f, 0.707f,		1.0f, 0.0f, 0.0f
};

GLfloat floorVertices[6*9] = {
	10.0f, -2.0f, 10.0f,	0.0f, 1.0f, 0.0f,			1.0f, 0.5f, 0.31f,
	10.0f, -2.0f, -10.0f,	0.0f, 1.0f, 0.0f,			1.0f, 0.5f, 0.31f,
	-10.0f, -2.0f, -10.0f,	0.0f, 1.0f, 0.0f,			1.0f, 0.5f, 0.31f,
	
	10.0f, -2.0f, 10.0f,	0.0f, 1.0f, 0.0f,			1.0f, 0.5f, 0.31f,
	-10.0f, -2.0f, -10.0f,	0.0f, 1.0f, 0.0f,			1.0f, 0.5f, 0.31f,
	-10.0f, -2.0f, 10.0f,	0.0f, 1.0f, 0.0f,			1.0f, 0.5f, 0.31f,
};

GLuint depthTexture;
float lightViewProjection[4][4];

class Debug {
public:
	Debug(){
		SDL_Event event;
		event.type = SDL_MOUSEBUTTONDOWN;
		event.button.button = SDL_BUTTON_LEFT;
		ESDL::AddEventCallback<Debug>({this, &Debug::MouseDown}, event);
		event.type = SDL_MOUSEBUTTONUP;
		ESDL::AddEventCallback<Debug>({this, &Debug::MouseUp}, event);
		
		yawStart = cameraYaw;
		pitchStart = cameraPitch;
	}
	void MouseDown(SDL_Event event){
		yawStart = cameraYaw;
		pitchStart = cameraPitch;
		xStart = event.button.x;
		yStart = event.button.y;
		mouseDown = true;
	}
	void MouseUp(SDL_Event event){
		mouseDown = false;
	}
	
	void Update(){
		if(!mouseDown) return;
		int x, y;
		SDL_GetMouseState(&x, &y);
		cameraYaw = yawStart + yawSensitivity*(float)(x - xStart);
		cameraPitch = pitchStart + pitchSensitivity*(float)(y - yStart);
	}
	
private:
	static const float constexpr yawSensitivity = 0.003f;
	static const float constexpr pitchSensitivity = 0.003f;
	int xStart, yStart;
	float yawStart, pitchStart;
	bool mouseDown = false;
};

void Render(){
	// matrices
	float m[4][4];
	float cameraMatrix[4][4];
	float viewMatrix[4][4];
	float viewProjectionMatrix[4][4];
	
	// view from light
	glViewport(0, 0, lightPortWidth, lightPortHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(ESDL::shaderPrograms[1].handle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(ESDL::shaderPrograms[1].attribLocations[0], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_position_offset); // vbo needs to be bound for this
	glEnableVertexAttribArray(ESDL::shaderPrograms[1].attribLocations[0]);
	for(int i=0; i<4; i++) glDisableVertexAttribArray(ESDL::shaderPrograms[1].attribLocations[1] + i);
	
	// object
	float worldMatrix[4][4];
	M4x4_Identity(worldMatrix);
	M4x4_xRotation(objectPitch, m); // object pitch rotation
	M4x4_PreMultiply(worldMatrix, m);
	M4x4_yRotation(objectYaw, m); // object yaw rotation
	M4x4_PreMultiply(worldMatrix, m);
	ESDL::SetMat4x4Attribute(ESDL::shaderPrograms[1].attribLocations[1], worldMatrix);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // vbo needs to be bound for this
	glDrawArrays(GL_TRIANGLES, 0, 12);
	// floor
	M4x4_Identity(worldMatrix);
	ESDL::SetMat4x4Attribute(ESDL::shaderPrograms[1].attribLocations[1], worldMatrix);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW); // vbo needs to be bound for this
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	static GLfloat *depthBuffer = (GLfloat *)malloc(sizeof(GLfloat) * lightPortWidth * lightPortHeight);
	glReadPixels(0, 0, lightPortWidth, lightPortHeight, GL_RED, GL_FLOAT, depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, lightPortWidth, lightPortHeight, 0, GL_RED, GL_FLOAT, depthBuffer);
	
	// view from player
	glViewport(0, 0, portWidth, portHeight);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(ESDL::shaderPrograms[0].handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(ESDL::shaderPrograms[0].uniformLocations[8], 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(ESDL::shaderPrograms[0].attribLocations[0], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_position_offset); // vbo needs to be bound for this
	glVertexAttribPointer(ESDL::shaderPrograms[0].attribLocations[1], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_normal_offset); // vbo needs to be bound for 2
	glVertexAttribPointer(ESDL::shaderPrograms[0].attribLocations[2], 3, GL_FLOAT, GL_FALSE, vertex_stride, (GLvoid *)vertex_colour_offset); // vbo needs to be bound for this
	glEnableVertexAttribArray(ESDL::shaderPrograms[0].attribLocations[0]);
	glEnableVertexAttribArray(ESDL::shaderPrograms[0].attribLocations[1]);
	glEnableVertexAttribArray(ESDL::shaderPrograms[0].attribLocations[2]);
	for(int i=0; i<4; i++) glDisableVertexAttribArray(ESDL::shaderPrograms[0].attribLocations[3] + i);
	for(int i=0; i<4; i++) glDisableVertexAttribArray(ESDL::shaderPrograms[0].attribLocations[4] + i);
	
	M4x4_Identity(cameraMatrix);
	M4x4_xRotation(-cameraPitch, m);
	M4x4_PreMultiply(cameraMatrix, m);
	M4x4_yRotation(-cameraYaw, m);
	M4x4_PreMultiply(cameraMatrix, m);
	M4x4_Translation({x, y, z}, m);
	M4x4_PreMultiply(cameraMatrix, m);
	M4x4_Inverse(cameraMatrix, viewMatrix);
	M4x4_Multiply(projectionMatrix, viewMatrix, viewProjectionMatrix);
	glUniformMatrix4fv(ESDL::shaderPrograms[0].uniformLocations[1], 1, GL_FALSE, &viewProjectionMatrix[0][0]);
	glUniformMatrix4fv(ESDL::shaderPrograms[0].uniformLocations[2], 1, GL_FALSE, &cameraMatrix[0][0]); // view inverse
	
	// object
	float worldInverseTransposeMatrix[4][4];
	M4x4_Identity(worldMatrix);
	M4x4_xRotation(objectPitch, m); // object pitch rotation
	M4x4_PreMultiply(worldMatrix, m);
	M4x4_yRotation(objectYaw, m); // object yaw rotation
	M4x4_PreMultiply(worldMatrix, m);
	M4x4_Inverse(worldMatrix, m);
	M4x4_Transpose(m, worldInverseTransposeMatrix);
	ESDL::SetMat4x4Attribute(ESDL::shaderPrograms[0].attribLocations[3], worldMatrix);
	ESDL::SetMat4x4Attribute(ESDL::shaderPrograms[0].attribLocations[4], worldInverseTransposeMatrix);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // vbo needs to be bound for this
	glDrawArrays(GL_TRIANGLES, 0, 12);
	// floor
	M4x4_Identity(worldMatrix);
	M4x4_Identity(worldInverseTransposeMatrix);
	ESDL::SetMat4x4Attribute(ESDL::shaderPrograms[0].attribLocations[3], worldMatrix);
	ESDL::SetMat4x4Attribute(ESDL::shaderPrograms[0].attribLocations[4], worldInverseTransposeMatrix);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW); // vbo needs to be bound for this
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	SDL_GL_SwapWindow(window);
}

int main(int argc, const char *argv[]) {
	
	Debug debug = Debug();
	
	SDL_Init(SDL_INIT_VIDEO);
	
	ESDL::AddShaderProgram({
		"./Shaders/vert.vert",
		"./Shaders/frag.frag",
		5,
		{"a_position", "a_normal", "a_colour", "a_world", "a_worldInverseTranspose"},
		{1, 1, 1, 4, 4},
		12,
		{"u_lightWorldPos", "u_viewProjection", "u_viewInverse", "u_lightColour", "u_colourMult", "u_specular", "u_shininess", "u_specularFactor", "u_depthTexture", "u_portWidth", "u_portHeight", "u_lightProjectionMatrix"}
	});
	
	ESDL::AddShaderProgram({
		"./Shaders/depthVert.vert",
		"./Shaders/depthFrag.frag",
		2,
		{"a_position", "a_world"},
		{1, 4},
		1,
		{"u_viewProjection"}
	});
	
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	scaledPortWidth = dm.w;
	scaledPortHeight = dm.h;
	scaledPortWidthInverse = 1.0f / (float)scaledPortWidth;
	scaledPortHeightInverse = 1.0f / (float)scaledPortHeight;
	const Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP;
	window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scaledPortWidth, scaledPortHeight, flags);
	SDL_GL_GetDrawableSize(window, &portWidth, &portHeight);
	portWidthInverse = 1.0f / (float)portWidth;
	portHeightInverse = 1.0f / (float)portHeight;
	aspectRatio = (float)portWidth * portHeightInverse;
	ppm_subWidth = 0.002f * portWidthInverse;
	ppm_subHeight = 0.002f * portHeightInverse;
	subLeft = -0.001f * portHeightInverse;
	subBottom = -0.001f * portHeightInverse;
	ESDL::InitGL(window);
	
	ESDL::InitEventHandler();
	
	// projection matrix
	M4x4_Perspective(angleOfView, aspectRatio, zNear, zFar, projectionMatrix);
	
	float lightCamera[4][4];
	float lightView[4][4];
	float lightProjection[4][4];
	M4x4_Perspective(1.57f, (float)lightPortWidth/(float)lightPortHeight, zNear, zFar, lightProjection);
	M4x4_LookAt({lightWorldPos[0], lightWorldPos[1], lightWorldPos[2]}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, lightCamera);
	M4x4_Inverse(lightCamera, lightView);
	M4x4_Multiply(lightProjection, lightView, lightViewProjection);
	
	glGenTextures(1, &depthTexture);
	
	glGenBuffers(1, &vbo);
	
	glUseProgram(ESDL::shaderPrograms[0].handle); // must be called before setting attributes / uniforms
	glUniform3fv(ESDL::shaderPrograms[0].uniformLocations[0], 1, lightWorldPos);
	glUniform4fv(ESDL::shaderPrograms[0].uniformLocations[3], 1, lightColour);
	glUniform4fv(ESDL::shaderPrograms[0].uniformLocations[4], 1, colourMultiplier);
	glUniform4fv(ESDL::shaderPrograms[0].uniformLocations[5], 1, specularColour);
	glUniform1f(ESDL::shaderPrograms[0].uniformLocations[6], 1000.0f); // shininess: 0 to 500, lower is more shiny
	glUniform1f(ESDL::shaderPrograms[0].uniformLocations[7], 0.5f); // specular factor: 0 to 1
	glUniform1i(ESDL::shaderPrograms[0].uniformLocations[9], portWidth); // shininess: 0 to 500, lower is more shiny
	glUniform1i(ESDL::shaderPrograms[0].uniformLocations[10], portHeight); // specular factor: 0 to 1
	glUniformMatrix4fv(ESDL::shaderPrograms[0].uniformLocations[11], 1, GL_FALSE, &lightViewProjection[0][0]);
	
	glUseProgram(ESDL::shaderPrograms[1].handle); // must be called before setting attributes / uniforms
	glUniformMatrix4fv(ESDL::shaderPrograms[1].uniformLocations[0], 1, GL_FALSE, &lightViewProjection[0][0]);
	
	//glEnable(GL_TEXTURE_COMPARE_MODE);
	//glEnable(GL_COMPARE_REF_TO_TEXTURE);
	
	while(!ESDL::HandleEvents()){
		static const float speed = 0.15f;
		const float thisForwardSpeed = speed*(float)(ESDL::GetKeyDown(SDLK_w) - ESDL::GetKeyDown(SDLK_s));
		if(thisForwardSpeed){
			x += thisForwardSpeed*sin(cameraYaw)*cos(cameraPitch);
			y -= thisForwardSpeed*sin(cameraPitch);
			z -= thisForwardSpeed*cos(cameraYaw)*cos(cameraPitch);
		}
		const float thisUpwardSpeed = speed*(float)(ESDL::GetKeyDown(SDLK_SPACE) - ESDL::GetKeyDown(SDLK_LSHIFT));
		if(thisUpwardSpeed){
			x += thisUpwardSpeed*sin(cameraYaw)*sin(cameraPitch);
			y += thisUpwardSpeed*cos(cameraPitch);
			z -= thisUpwardSpeed*cos(cameraYaw)*sin(cameraPitch);
		}
		const float thisLateralSpeed = speed*(float)(ESDL::GetKeyDown(SDLK_d) - ESDL::GetKeyDown(SDLK_a));
		if(thisLateralSpeed){
			x += thisLateralSpeed*cos(cameraYaw);
			z += thisLateralSpeed*sin(cameraYaw);
		}
		
		static const float objectRotateSpeed = 0.04f;
		if(ESDL::GetKeyDown(SDLK_RIGHT)) objectYaw += objectRotateSpeed;
		if(ESDL::GetKeyDown(SDLK_LEFT)) objectYaw -= objectRotateSpeed;
		if(ESDL::GetKeyDown(SDLK_UP)) objectPitch += objectRotateSpeed;
		if(ESDL::GetKeyDown(SDLK_DOWN)) objectPitch -= objectRotateSpeed;
		
		debug.Update();
		
		Render();
	}
	
	return 0;
}
