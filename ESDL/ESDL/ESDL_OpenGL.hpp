#ifndef ESDL_OpenGL_hpp
#define ESDL_OpenGL_hpp

#include "ESDL_General.hpp"

#define GL_SILENCE_DEPRECATION

/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>

#define MAX_SHADER_PROGRAMS 16
#define MAX_SHADER_ATTRIBUTES 64
#define MAX_SHADER_UNIFORMS 64
#define MAX_VERTEX_BUFFERS 16

/*
 Basics required for use:
 
Initialisation:
	- Start with 'SDL_Init()' with at least the 'SDL_INIT_VIDEO' flag
	- Create an 'SDL_Window'
	- Add your shader programs data with 'ESDL:AddShaderProgram()'
	- Call 'ESDL::InitGL()'
	- Generate your vertex buffer(s)
	- Set values of quasi-constant uniforms

To render with a glDraw... function, you must have:
	- Set program with 'glUseProgram()'
	- Bound your vertex buffer object(s) with 'glBindBuffer()' (main target will likely be 'GL_ARRAY_BUFFER')
	- Set how the vertex buffer(s) is/are to be read with 'glVertexAttribPointer()' along with 'glEnableVertexAttribArray()'
	- For attributes not read from a buffer, call 'glDisableVertexAttribArray()'
	- Set the viewport with 'glViewport()'
	- Set the clear colour with 'glClearColor()'

 Render loop:
	- Clear the screen with 'glClear()' (main flags being 'GL_COLOUR_BUFFER_BIT' and 'GL_DEPTH_BUFFER_BIT')
	- Set values of per-object/less constant uniforms
	- Set values of attributes that aren't read from a buffer
 	- Buffer vertex/index data with 'glBufferData()'
	- Make your draw call, e.g. 'glDrawArrays' (direct vertex data) or 'glDrawElements' (indexed vertices)
	- Call 'SDL_GL_SwapWindow()' with your 'SDL_Window'
 
 */

namespace ESDL {

struct ShaderProgramStruct {
	GLuint handle;
	
	GLuint vertexHandle, fragmentHandle;
	
	unsigned char attribsN;
	GLint attribLocations[MAX_SHADER_ATTRIBUTES];
	
	unsigned char uniformsN;
	GLint uniformLocations[MAX_SHADER_UNIFORMS];
};

// All information about shader programs.
extern int shaderProgramsN;
extern ShaderProgramStruct shaderPrograms[MAX_SHADER_PROGRAMS];

struct ShaderProgramBuildStruct {
	
	const char *vertexShaderFile;
	const char *fragmentShaderFile;
	
	unsigned char attribsN;
	const char *attribNames[MAX_SHADER_ATTRIBUTES];
	int attribSizes[MAX_SHADER_ATTRIBUTES];
	
	unsigned char uniformsN;
	const char *uniformNames[MAX_SHADER_UNIFORMS];
};

// Provide information about a program to be built in 'InitGL()'.
int AddShaderProgram(const ShaderProgramBuildStruct& programBuild);

// Initialise OpenGL and build shader programs according to info passed by 'AddShaderProgram()'.
void InitGL(SDL_Window *const &window);

void SetMat4x4Attribute(const GLuint& index, float matrix[4][4]);

}

#endif /* ESDL_OpenGL_hpp */
