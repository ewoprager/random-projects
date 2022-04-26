#include "ESDL_OpenGL.hpp"

namespace ESDL {

SDL_GLContext context;

int shaderProgramsN = 0;
static ShaderProgramBuildStruct shaderProgramBuilds[MAX_SHADER_PROGRAMS];
ShaderProgramStruct shaderPrograms[MAX_SHADER_PROGRAMS];

// I took this function from somewhere on the internet a long time ago and have adapted it since
bool CompileShader(const GLuint& shader, const char *sourceFile){
	int compileSuccess;
	int maxLength;
	GLchar *shaderSource = filetobuf(sourceFile);
	glShaderSource(shader, 1, (const GLchar**)&shaderSource, 0);
	glCompileShader(shader);
	free(shaderSource);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
	if(compileSuccess == GL_FALSE){
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength); // The maxLength includes the NULL character
		char *infoLog = (char *)malloc(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);
		return false;
	}
	return true;
}

// I took this function from somewhere on the internet a long time ago and have adapted it since
bool LinkProgram(const GLuint& program){
	int linkSuccess;
	int maxLength;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&linkSuccess);
	if(linkSuccess == GL_FALSE){
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = (char *)malloc(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);
		return false;
	}
	return true;
}

ShaderProgramStruct BuildShaderProgram(const ShaderProgramBuildStruct& programBuild){
	ShaderProgramStruct ret;
	ret.attribsN = programBuild.attribsN;
	ret.uniformsN = programBuild.uniformsN;
	
	ret.vertexHandle = glCreateShader(GL_VERTEX_SHADER);
	if(!CompileShader(ret.vertexHandle, programBuild.vertexShaderFile)) std::cout << "ERROR: Vertex shader failed to compile." << std::endl;
	ret.fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
	if(!CompileShader(ret.fragmentHandle, programBuild.fragmentShaderFile)) std::cout << "ERROR: Vertex shader failed to compile." << std::endl;
	
	ret.handle = glCreateProgram();
	glAttachShader(ret.handle, ret.vertexHandle);
	glAttachShader(ret.handle, ret.fragmentHandle);
	
	glUseProgram(ret.handle);
	
	// binding attribute locations BEFORE LINKING
	int location = 1; // we avoid location 0 here to guarantee its use
	for(int i=0; i<ret.attribsN; i++){
		glBindAttribLocation(ret.handle, location, programBuild.attribNames[i]);
		ret.attribLocations[i] = location;
		location += programBuild.attribSizes[i];
	}
	
	if(!LinkProgram(ret.handle)) std::cout << "ERROR: Program failed to link!" << std::endl;
	glDetachShader(ret.handle, ret.vertexHandle);
	glDetachShader(ret.handle, ret.fragmentHandle);
	
	// finding uniform locations
	for(int i=0; i<ret.uniformsN; i++){
		ret.uniformLocations[i] = glGetUniformLocation(ret.handle, programBuild.uniformNames[i]);
		if(ret.uniformLocations[i] == -1) std::cout << "ERROR: uniform not found: " << programBuild.uniformNames[i] << "; compiler may have removed it if it is unused in shader." << std::endl;
	}
	
	return ret;
}

int AddShaderProgram(const ShaderProgramBuildStruct& programBuild){
	shaderProgramBuilds[shaderProgramsN] = programBuild;
	return shaderProgramsN++;
}

void InitGL(SDL_Window *const &window){
	context = SDL_GL_CreateContext(window);
	
	// setting OpenGL version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	
	// turning on double buffering
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
	// enabling multisampling for a nice antialiased effect
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	
	// shaders
	for(int i=0; i<shaderProgramsN; i++) shaderPrograms[i] = BuildShaderProgram(shaderProgramBuilds[i]);
	
	// guaranteeing we use attribute 0
	glEnableVertexAttribArray(0);
	
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void SetMat4x4Attribute(const GLuint& index, float matrix[4][4]){
	glVertexAttrib4fv(index + 0, matrix[0]);
	glVertexAttrib4fv(index + 1, matrix[1]);
	glVertexAttrib4fv(index + 2, matrix[2]);
	glVertexAttrib4fv(index + 3, matrix[3]);
}

}
