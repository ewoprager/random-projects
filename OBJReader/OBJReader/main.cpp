#include <stdio.h>

#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

void ReadOBJFile(const char *objFile, const char *saveLocation);

class MyApp : public wxApp {
public:
	virtual bool OnInit();
	
private:
	// wxWidgets has the concept of a "current locale". It is the one returned
	// by wxGetLocale() and implicitly used by wxGetTranslation.
	// But there is no way to explicitly set this current locale! Rather, it is
	// always set to the last constructed locale(!), and is modified when a
	// locale is destroyed. In such a way that the current locale points to
	// invalid memory after you do
	//    wxLocale *a = new wxLocale;
	//    wxLocale *b = new wxLocale;
	//    delete a;
	//    delete b;
	// So, to avoid problems, we use exactly one instance of wxLocale, and keep
	// it alive for the entire application lifetime.
	wxLocale appLocale;
};

class MyFrame : public wxFrame {
public:
	MyFrame();
	
	void OnOpen(wxCommandEvent& event);
	void OnDir(wxCommandEvent& event);
	void OnProcess(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	
	// menu bar
	wxMenuBar *menuBar;
	
	wxMenu *fileMenu;
	wxMenu *runMenu;
	
	wxMenuItem *menuOpen;
	wxMenuItem *menuDir;
	wxMenuItem *menuProcess;
	
	char saveDirectoryBuffer[512];
	bool saveDirSet = false;
	wxArrayString openFilePathArray;
	bool openPathsSet = false;
};

// This defines the main() function.
IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(){
	// First, register the base directory where to look up .mo files.
	wxLocale::AddCatalogLookupPathPrefix(wxT("/opt/local/share/locale"));
	// Second, initialize the locale and set the application-wide message domain.
	appLocale.Init();
	appLocale.AddCatalog(wxT("hello-c++-wxwidgets"));
	// Now wxGetLocale() is initialized appropriately.

	// Then only start building the GUI elements of the application.

	// Create the main frame window.
	MyFrame *frame = new MyFrame();

	// Show the frame.
	frame->Show(true);
	SetTopWindow(frame);

	return true;
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, _T("OBJ interpreter for basic OpenGL drawing")) {
	wxStaticText *label1 = new wxStaticText(this, wxID_ANY, _("This program converts .obj files into .bin files in a format compatible with a simple OpenGL drawing program."));
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(label1);
	SetSizer(topSizer);
	
	menuBar = new wxMenuBar;
	fileMenu = new wxMenu;
	runMenu = new wxMenu;
	
	menuOpen = new wxMenuItem(fileMenu, wxID_OPEN, wxT("&Open\tCtrl+O"));
	fileMenu->Append(menuOpen);
	
	menuDir = new wxMenuItem(fileMenu, wxID_SAVE, wxT("&Choose save directory\tCtrl+D"));
	fileMenu->Append(menuDir);
	
	menuProcess = new wxMenuItem(runMenu, wxID_FILE, wxT("&Process and save files\tCtrl+P"));
	runMenu->Append(menuProcess);
	
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(runMenu, wxT("&Run"));
	
	Bind(wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(MyFrame::OnOpen), this, wxID_OPEN);
	Bind(wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(MyFrame::OnDir), this, wxID_SAVE);
	Bind(wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(MyFrame::OnProcess), this, wxID_FILE);
	
	SetMenuBar(menuBar);
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event)){
	wxFileDialog openFileDialog(this, _("Open object file(s)"), "", "", "Object files (*.obj)|*.obj", wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	if(openFileDialog.ShowModal() == wxID_CANCEL) return;
	
	wxArrayString pathArray;
	openFileDialog.GetPaths(openFilePathArray);
	
	openPathsSet = true;
}

void MyFrame::OnDir(wxCommandEvent& WXUNUSED(event)){
	wxDirDialog saveDirDialog(this, _("Choose output directory"));
	if(saveDirDialog.ShowModal() == wxID_CANCEL) return;
	
	wxString saveDir = saveDirDialog.GetPath();
	memcpy(saveDirectoryBuffer, saveDir, strlen(saveDir) + 1);
	
	saveDirSet = true;
}

void MyFrame::OnProcess(wxCommandEvent& WXUNUSED(event)){
	if(!openPathsSet){
		wxMessageDialog(this, _("You have not chosen any files to process. Open them with File -> Open."), "").ShowModal();
		return;
	}
	if(!saveDirSet){
		wxMessageDialog(this, _("You have not set a desination directory for the ouput. Choose this with File -> Choose save directory."), "").ShowModal();
		return;
	}
	
	size_t count = openFilePathArray.GetCount();
	for(int i=0; i<count; i++){
		char *openFilePath = openFilePathArray[i].char_str();
		size_t nameStart = strlen(openFilePathArray[i]) - 1;
		while(openFilePathArray[i][nameStart] != '/') nameStart--;
		size_t nameLength = 0;
		while(openFilePathArray[i][nameStart + nameLength] != '.') nameLength++;
		char saveFileBuffer[512];
		memcpy(saveFileBuffer, saveDirectoryBuffer, strlen(saveDirectoryBuffer));
		memcpy(saveFileBuffer + strlen(saveDirectoryBuffer), openFilePath + nameStart, nameLength);
		static const char *suffix = ".bin";
		memcpy(saveFileBuffer + strlen(saveDirectoryBuffer) + nameLength, suffix, strlen(suffix) + 1);
	
		ReadOBJFile(openFilePathArray[i], saveFileBuffer);
	}
	
	wxMessageDialog(this, _("Files processed and saved to '")+wxString(saveDirectoryBuffer)+_("'."), "").ShowModal();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(true);
}

// obj file interpreting
#include "TextRead.hpp"
#include "vec3.hpp"
#define MAX_DIVISIONS 64
struct ObjectDivisionData {
	int32_t start;
	size_t count;
	uint8_t usemtl[512];
};
template <unsigned int d, typename T> struct obj_array_struct {
	T array[d];
	T &operator[](unsigned int index) {
		return array[index];
	}
	obj_array_struct<d, T> &operator=(obj_array_struct<d, T> other){
		for(unsigned int i=0; i<d; i++) array[i] = other[i];
		return *this;
	}
};
struct obj_v : obj_array_struct<3, float> {};
struct obj_vt : obj_array_struct<2, float> {};
struct obj_vn : obj_array_struct<3, float> {};
vec3 OBJVToVector(obj_v OBJvertex);
obj_v VectorToOBJV(vec3 vec);
obj_vn VectorToOBJVN(vec3 vec);
#define MAX_CORNERS 128
struct obj_corner {
	unsigned int vIndex, vtIndex, vnIndex;
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

void ReadOBJFile(const char *objFile, const char *saveLocation){
	static const unsigned int STRUCT_BUFFER_SIZE = 30000;
	
	obj_v vBuffer[STRUCT_BUFFER_SIZE];
	unsigned int vBufferN = 0;
	obj_vn vnBuffer[STRUCT_BUFFER_SIZE];
	unsigned int vnBufferN = 0;
	obj_vt vtBuffer[STRUCT_BUFFER_SIZE];
	unsigned int vtBufferN = 0;
	
	obj_corner mainBuffer[STRUCT_BUFFER_SIZE];
	uint32_t mainBufferN = 0;
	
	ObjectDivisionData divisionData[MAX_DIVISIONS];
	uint32_t divisionsN = 0;
	
	// text reader
	TextRead *tr = new TextRead();
	tr->BeginRead(objFile);
	
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
	unsigned int cornersN;
	tr->SetPlace(0);
	while(true){
		if(tr->SkipLinesUntil("fu")) break;
		if(tr->GetCharacter(false) == 'u'){ // 'u', looking for 'usemtl'
			if(!tr->CheckString("usemtl")){ tr->SkipToNextLine(); continue; }
			tr->SkipNumber(6); // skip the 'usemtl'
			tr->SkipWhile(tr->whiteSpaceHorizontalString);
			tr->ReadUntil(stringBuffer, 50, tr->whiteSpaceString, true);
			const char *materialName = stringBuffer;
			memcpy(divisionData[divisionsN].usemtl, materialName, strlen(materialName) + 1);
			divisionData[divisionsN].start = mainBufferN;
			divisionsN += 1;
		} else { // 'f'
			if(divisionsN == 0){ // haven't yet seen a 'usemtl'
				const char *defaultMaterialName = "debugTexture";
				memcpy(divisionData[divisionsN].usemtl, defaultMaterialName, strlen(defaultMaterialName) + 1);
				divisionData[divisionsN].start = mainBufferN;
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
			unsigned int trianglesN;
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
	
	uint32_t *vertices = (uint32_t *)malloc(mainBufferN * 8 * sizeof(uint32_t));
	for(unsigned int c=0; c<mainBufferN; c++){
		unsigned int i = c * 8;
		obj_corner corner = mainBuffer[c];
		vertices[i + 0] = *(uint32_t *)&vBuffer[corner.vIndex][0];
		vertices[i + 1] = *(uint32_t *)&vBuffer[corner.vIndex][1];
		vertices[i + 2] = *(uint32_t *)&vBuffer[corner.vIndex][2];
		vertices[i + 3] = *(uint32_t *)&vnBuffer[corner.vnIndex][0];
		vertices[i + 4] = *(uint32_t *)&vnBuffer[corner.vnIndex][1];
		vertices[i + 5] = *(uint32_t *)&vnBuffer[corner.vnIndex][2];
		vertices[i + 6] = *(uint32_t *)&vtBuffer[corner.vtIndex][0];
		vertices[i + 7] = *(uint32_t *)&vtBuffer[corner.vtIndex][1];
	}
	
	for(int d=0; d<divisionsN; d++) divisionData[d].count = ( d == divisionsN-1 ? mainBufferN - divisionData[d].start : divisionData[d+1].start - divisionData[d].start );
	
	FILE *fptr;
	fptr = fopen(saveLocation, "wb");
	if(!fptr){ std::cout << "ERROR: Can't open save file!" << std::endl; return; }// Return on failure
	fwrite(&mainBufferN, sizeof(uint32_t), 1, fptr);
	fwrite(vertices, sizeof(uint32_t), mainBufferN * 8, fptr);
	fwrite(&divisionsN, sizeof(uint32_t), 1, fptr);
	fwrite(divisionData, sizeof(ObjectDivisionData), divisionsN, fptr);
	fclose(fptr);
}
