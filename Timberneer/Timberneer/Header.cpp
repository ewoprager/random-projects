#include "Header.hpp"

double AngleDifference(const double& angle1, const double& angle2){
	double ret = angle2 - angle1;
	while(ret > PI) ret -= PI+PI;
	while(ret < -PI) ret += PI+PI;
	return ret;
}
float RandomAngle(){
	return (float)(rand() % 6283)*0.001f;
}
float IntegerPow(const float& x, const unsigned int& p){
	if(p == 0) return 1.0f;
	if(p == 1) return x;
	float tmp = IntegerPow(x, p/2);
	if(p % 2){
		return x * tmp * tmp;
	} else {
		return tmp * tmp;
	}
}

bool StringEquality(const char *string1, const char *string2){
	unsigned int i = 0;
	while(string1[i] != '\0'){
		if(string2[i] == '\0') return false;
		if(string1[i] != string2[i]) return false;
		i++;
	}
	if(string2[i] != '\0') return false;
	return true;
}

bool EventEquality(SDL_Event event1, SDL_Event event2){
	if(event1.type != event2.type) return false;
	switch(event1.type){
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if(event1.key.keysym.sym != event2.key.keysym.sym) return false;
			break;
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			if(event1.button.button != event2.button.button) return false;
			break;
		default:
			break;
	}
	return true;
}

/* A simple function that will read a file into an allocated char pointer buffer */
GLchar* filetobuf(const char *file){
	FILE *fptr;
	long length;
	GLchar *buf;
	
	fptr = fopen(file, "rb"); // Open file for reading
	if(!fptr) return NULL; // Return NULL on failure
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (GLchar *)malloc(length+1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator

	return buf; // Return the buffer
}

bool CompileShader(GLuint shader, const char *sourceFile){
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

bool LinkProgram(GLuint program){
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

GLuint LoadTexture(const char *file){	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// load and generate the texture
	unsigned int width, height;
	unsigned char *data = 0;
	unsigned int error = lodepng_decode32_file(&data, &width, &height, file);
	if(error){
		std::cout << "ERROR: ";
		printf("%u: %s\n", error, lodepng_error_text(error));
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	free(data);
	return texture;
}

extern TTF_Font *textFont;
extern const SDL_Color textColour;
GLuint MakeTextTexture(const char *text){
	SDL_Surface *surface = TTF_RenderUTF8_Blended(textFont, text, textColour);
	GLenum texture_format;
	int colours = surface->format->BytesPerPixel;
	if(colours == 4){	// alpha
		if(surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA;
		else texture_format = GL_BGRA;
	} else {	// no alpha
		if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGB;
		else texture_format = GL_BGR;
	}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, colours, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	return texture;
}

GLuint GetDefaultTextureId(){
	return LoadTexture("./usr/share/Timberneer/assets/Data/debugTexture.png");
}
GLuint GetTextureIdFromMtl(const char *usemtl){
	const int nameLength = (int)strlen(usemtl);
	char buffer[512];
	static const char *prefix = "./usr/share/Timberneer/assets/Data/";
	static const int prefixLength = (int)strlen(prefix);
	static const char *suffix = ".png";
	static const int suffixLength = (int)strlen(suffix);
	memcpy(buffer, prefix, prefixLength * sizeof(char));
	memcpy(buffer + prefixLength, usemtl, nameLength * sizeof(char));
	memcpy(buffer + prefixLength + nameLength, suffix, (suffixLength + 1) * sizeof(char));
	return LoadTexture(buffer);
}

// obj file interpreting
#include "TextRead.hpp"
#define MAX_CORNERS 128
struct obj_corner {
	GLuint vIndex, vtIndex, vnIndex;
};
struct obj_smoothVertex {
	obj_v v;
	obj_vn vn;
	obj_vt vt;
};
vec3 OBJVToVector(obj_v OBJvertex){
	return {OBJvertex[0], OBJvertex[1], OBJvertex[2]};
}
obj_v VectorToOBJV(vec3 vec){
	obj_v ret;
	ret[0] = vec.x;
	ret[1] = vec.y;
	ret[2] = vec.z;
	return ret;
}
obj_vn VectorToOBJVN(vec3 vec){
	obj_vn ret;
	ret[0] = vec.x;
	ret[1] = vec.y;
	ret[2] = vec.z;
	return ret;
}

ObjectData ReadProcessedOBJFile(const char *file){
	ObjectData ret;
	
	FILE *fptr;
	fptr = fopen(file, "rb");
	if(!fptr){ std::cout << "ERROR: Unable to open file for reading." << std::endl; return ret; }
	fread(&ret.vertices_n, sizeof(uint32_t), 1, fptr);
	ret.vertices = (GLfloat *)malloc(ret.vertices_n * 8 * sizeof(uint32_t));
	fread(ret.vertices, sizeof(uint32_t), ret.vertices_n * 8, fptr);
	fread(&ret.divisionsN, sizeof(uint32_t), 1, fptr);
	FileObjectDivisionData *fileDivData = (FileObjectDivisionData *)malloc(ret.divisionsN * sizeof(FileObjectDivisionData));
	fread(fileDivData, sizeof(FileObjectDivisionData), ret.divisionsN, fptr);
	ret.divisionData = (ObjectDivisionData *)malloc(ret.divisionsN * sizeof(ObjectDivisionData));
	for(int i=0; i<ret.divisionsN; i++){
		ret.divisionData[i].start = fileDivData[i].start;
		ret.divisionData[i].count = (GLsizei)fileDivData[i].count;
		ret.divisionData[i].texture = GetTextureIdFromMtl((const char *)fileDivData[i].usemtl);
	}
	fclose(fptr);
	return ret;
}

/*
ObjectData ReadOBJFile(const char *file){
	static const unsigned int STRUCT_BUFFER_SIZE = 30000;
	
	obj_v vBuffer[STRUCT_BUFFER_SIZE];
	unsigned int vBufferN = 0;
	obj_vn vnBuffer[STRUCT_BUFFER_SIZE];
	unsigned int vnBufferN = 0;
	obj_vt vtBuffer[STRUCT_BUFFER_SIZE];
	unsigned int vtBufferN = 0;
	
	obj_corner mainBuffer[STRUCT_BUFFER_SIZE];
	unsigned int mainBufferN = 0;
	GLint divisionStarts[MAX_DIVISIONS];
	
	GLuint divisionTextures[MAX_DIVISIONS];
	GLint divisionsN = 0;
	
	// text reader
	TextRead *tr = new TextRead();
	tr->BeginRead(file);
	
	// read through once for position and texCoord data
	char stringBuffer[50];
	while(true){
		if(tr->SkipLinesUntil("v")) break;
		tr->SkipNumber(1);
		switch(tr->GetCharacter(false)){
			case ' ': {
				obj_v vValue;
				for(int d=0; d<3; d++){
					tr->SkipWhile(tr->whiteSpaceHorizontalString);
					tr->ReadUntil(stringBuffer, 50, tr->whiteSpaceString, true);
					vValue[d] = strtof(stringBuffer, NULL);
				}
				vBuffer[vBufferN++] = vValue; // saving values to buffer
				break;
			}
			case 't': {
				tr->SkipNumber(1);
				obj_vt vtValue;
				for(int d=0; d<2; d++){
					tr->SkipWhile(tr->whiteSpaceHorizontalString);
					tr->ReadUntil(stringBuffer, 50, tr->whiteSpaceString, true);
					vtValue[d] = strtof(stringBuffer, NULL);
				}
				vtBuffer[vtBufferN++] = vtValue; // saving values to buffer
				break;
			}
			case 'n': {
				tr->SkipNumber(1);
				obj_vn vnValue;
				for(int d=0; d<3; d++){
					tr->SkipWhile(tr->whiteSpaceHorizontalString);
					tr->ReadUntil(stringBuffer, 50, tr->whiteSpaceString, true);
					vnValue[d] = strtof(stringBuffer, NULL);
				}
				vnBuffer[vnBufferN++] = vnValue; // saving values to buffer
				break;
			}
			default: break;
		}
	}
	
	// read through again for face data
	bool givenNormals = false;
	obj_corner cornerBuffer[MAX_CORNERS];
	GLuint cornersN;
	tr->SetPlace(0);
	while(true){
		if(tr->SkipLinesUntil("fu")) break;
		if(tr->GetCharacter(false) == 'u'){ // 'u', looking for 'usemtl'
			if(!tr->CheckString("usemtl")){ tr->SkipToNextLine(); continue; }
			tr->SkipNumber(6); // skip the 'usemtl'
			tr->SkipWhile(tr->whiteSpaceHorizontalString);
			tr->ReadUntil(stringBuffer, 50, tr->whiteSpaceString, true);
			const char *materialName = stringBuffer;
			divisionTextures[divisionsN] = GetTextureIdFromMtl(materialName);
			divisionStarts[divisionsN] = mainBufferN;
			divisionsN += 1;
		} else { // 'f'
			if(divisionsN == 0){ // haven't yet seen a 'usemtl'
				divisionTextures[divisionsN] = GetDefaultTextureId();
				divisionStarts[divisionsN] = mainBufferN;
				divisionsN += 1;
			}
			cornersN = 0;
			tr->SkipNumber(1); // skip the 'f'
			// read the corner data
			while(true){
				// read the position index
				tr->SkipWhile(tr->whiteSpaceHorizontalString);
				tr->ReadUntil(stringBuffer, 50, "/", true);
				cornerBuffer[cornersN].vIndex = strtod(stringBuffer, NULL) - 1; // read string as integer
				// read the texture coordinate index
				tr->SkipNumber(1); // skip the '/'
				tr->ReadUntil(stringBuffer, 50, " /\t\r\n", true);
				cornerBuffer[cornersN].vtIndex = strtod(stringBuffer, NULL) - 1; // read string as integer
				// read the normal index if there is one
				if(tr->CheckCharacter("/")){
					givenNormals = true;
					tr->SkipNumber(1); // skip the '/'
					tr->ReadUntil(stringBuffer, 50, tr->whiteSpaceString, true);
					cornerBuffer[cornersN].vnIndex = strtod(stringBuffer, NULL) - 1; // read string as integer
				} else if(givenNormals){
					std::cout << "ERROR: Inconsistency in normal data availability: " << cornerBuffer[cornersN].vIndex << std::endl;
				}
				cornersN++;
				if(tr->SkipWhile(tr->whiteSpaceHorizontalString)) break;
				if(tr->CheckCharacter(tr->newLineString)) break;
				if(tr->CheckCharacter("#")) break;
			}
			// store corner data as triangles
			GLuint trianglesN;
			if(cornersN >= 3){
				trianglesN = cornersN - 2;
			} else {
				std::cout << "ERROR: Invalid number of sides." << std::endl;
				trianglesN = 0;
			}
			for(unsigned int t=0; t<trianglesN; t++){
				mainBuffer[mainBufferN++] = cornerBuffer[0];
				mainBuffer[mainBufferN++] = cornerBuffer[t+1];
				mainBuffer[mainBufferN++] = cornerBuffer[t+2];
			}
		}
	}
	
	// divisionsN is now set
	
	// generating normals
	if(!givenNormals){
		vec3 v1, v2, v3, vnVector;
		unsigned int c;
		// calculating normals for each vertex for each face
		obj_vn vn;
		for(unsigned int t=0; t<mainBufferN / 3; t++){
			c = t * 3;
			v1 = OBJVToVector(vBuffer[mainBuffer[c + 0].vIndex]);
			v2 = OBJVToVector(vBuffer[mainBuffer[c + 1].vIndex]);
			v3 = OBJVToVector(vBuffer[mainBuffer[c + 2].vIndex]);
			vnVector = V3_Cross(v2 - v1, v3 - v1).Normalise();
			vn = VectorToOBJVN(vnVector);
			vnBuffer[vnBufferN++] = vn;
			mainBuffer[c + 0].vnIndex = mainBuffer[c + 1].vnIndex = mainBuffer[c + 2].vnIndex = vnBufferN - 1;
		}
	}
	
	// finished with text reader
	tr->FinishRead();
	delete tr;
	
	// setting output
	ObjectData ret;
	
	ret.vertices_n = mainBufferN;
	
	ret.vertices = (GLfloat *)malloc(ret.vertices_n * 8 * sizeof(GLfloat));
	
	ret.divisionsN = divisionsN;
	
	ret.divisionData = (ObjectDivisionData *)malloc(ret.divisionsN * sizeof(ObjectDivisionData));
	
	for(int d=0; d<divisionsN; d++){
		ret.divisionData[d].texture = divisionTextures[d];
		ret.divisionData[d].start = divisionStarts[d];
		ret.divisionData[d].count = ( d == divisionsN-1 ? mainBufferN - divisionStarts[d] : divisionStarts[d+1] - divisionStarts[d] );
	}
	
	for(unsigned int c=0; c<mainBufferN; c++){
		unsigned int i = c * 8;
		obj_corner corner = mainBuffer[c];
		ret.vertices[i + 0] = vBuffer[corner.vIndex][0];
		ret.vertices[i + 1] = vBuffer[corner.vIndex][1];
		ret.vertices[i + 2] = vBuffer[corner.vIndex][2];
		ret.vertices[i + 3] = vnBuffer[corner.vnIndex][0];
		ret.vertices[i + 4] = vnBuffer[corner.vnIndex][1];
		ret.vertices[i + 5] = vnBuffer[corner.vnIndex][2];
		ret.vertices[i + 6] = vtBuffer[corner.vtIndex][0];
		ret.vertices[i + 7] = vtBuffer[corner.vtIndex][1];
	}
	
	return ret;
}
*/

ObjectData MakePlane(unsigned int columns, unsigned int rows, float columnWidth, float rowHeight, const char *usemtl){
	ObjectData ret;
	ret.divisionsN = 1;
	ret.vertices_n = 6 * columns * rows;
	
	float x0 = - (float)columns * 0.5f * columnWidth;
	float y0 = - (float)rows * 0.5f * rowHeight;
	obj_v positions[ret.vertices_n];
	obj_vt texCoords[ret.vertices_n];
	obj_vn normals[ret.vertices_n];
	unsigned int index = 0;
	for(unsigned int c=0; c<columns-1; c++){
		for(unsigned int r=0; r<rows-1; r++){
			positions[index][0] = x0 + columnWidth*(c + 0);
			positions[index][1] = 0.0f;
			positions[index][2] = y0 + rowHeight*(r + 0);
			normals[index][0] = 0.0f;
			normals[index][1] = 1.0f;
			normals[index][2] = 0.0f;
			texCoords[index][0] = 0.0f;
			texCoords[index][1] = 0.0f;
			index++;
			
			positions[index][0] = x0 + columnWidth*(c + 0);
			positions[index][1] = 0.0f;
			positions[index][2] = y0 + rowHeight*(r + 1);
			normals[index][0] = 0.0f;
			normals[index][1] = 1.0f;
			normals[index][2] = 0.0f;
			texCoords[index][0] = 0.0f;
			texCoords[index][1] = 1.0f;
			index++;
			
			positions[index][0] = x0 + columnWidth*(c + 1);
			positions[index][1] = 0.0f;
			positions[index][2] = y0 + rowHeight*(r + 0);
			normals[index][0] = 0.0f;
			normals[index][1] = 1.0f;
			normals[index][2] = 0.0f;
			texCoords[index][0] = 1.0f;
			texCoords[index][1] = 0.0f;
			index++;
			
			positions[index][0] = x0 + columnWidth*(c + 0);
			positions[index][1] = 0.0f;
			positions[index][2] = y0 + rowHeight*(r + 1);
			normals[index][0] = 0.0f;
			normals[index][1] = 1.0f;
			normals[index][2] = 0.0f;
			texCoords[index][0] = 0.0f;
			texCoords[index][1] = 1.0f;
			index++;
			
			positions[index][0] = x0 + columnWidth*(c + 1);
			positions[index][1] = 0.0f;
			positions[index][2] = y0 + rowHeight*(r + 1);
			normals[index][0] = 0.0f;
			normals[index][1] = 1.0f;
			normals[index][2] = 0.0f;
			texCoords[index][0] = 1.0f;
			texCoords[index][1] = 1.0f;
			index++;
			
			positions[index][0] = x0 + columnWidth*(c + 1);
			positions[index][1] = 0.0f;
			positions[index][2] = y0 + rowHeight*(r + 0);
			normals[index][0] = 0.0f;
			normals[index][1] = 1.0f;
			normals[index][2] = 0.0f;
			texCoords[index][0] = 1.0f;
			texCoords[index][1] = 0.0f;
			index++;
		}
	}
	
	ret.vertices = (GLfloat *)malloc(8 * ret.vertices_n * sizeof(GLfloat));
	for(unsigned int v=0; v<ret.vertices_n; v++){
		unsigned int i = v * 8;
		ret.vertices[i + 0] = positions[v][0];
		ret.vertices[i + 1] = positions[v][1];
		ret.vertices[i + 2] = positions[v][2];
		ret.vertices[i + 3] = normals[v][0];
		ret.vertices[i + 4] = normals[v][1];
		ret.vertices[i + 5] = normals[v][2];
		ret.vertices[i + 6] = texCoords[v][0];
		ret.vertices[i + 7] = texCoords[v][1];
	}
	
	ret.divisionData = (ObjectDivisionData *)malloc(sizeof(ObjectDivisionData));
	ret.divisionData[0].start = 0;
	ret.divisionData[0].count = ret.vertices_n;
	ret.divisionData[0].texture = GetTextureIdFromMtl(usemtl);
	
	return ret;
}
