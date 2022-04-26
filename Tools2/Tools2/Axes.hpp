#ifndef Axes_hpp
#define Axes_hpp

#define COLOURDEFAULT {220, 0, 0}

#include "/Library/Frameworks/SDL2.framework/Headers/SDL.h"
#include "/Library/Frameworks/SDL2_ttf.framework/Headers/SDL_ttf.h"
#include "/Library/Frameworks/SDL2_image.framework/Headers/SDL_image.h"
#include "Header.hpp"

struct RGB {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

enum PlotType {line, scatter};

template <int size> struct Plot {
	int N;
	bool drawLines[size];
	float xValues[size];
	float yValues[size];
	RGB colour;
	PlotType type;
};


// ----------------------------------------------------------
// -------------------- Axes Parameters --------------------
// ----------------------------------------------------------

struct GAxis {
	bool log;
	float min;
	float max;
	char title[100];
};
struct GDims {
	int width;
	int height;
	int margin;
};
enum class TextAlign {
	positive = 0,
	centre = 1,
	negative = 2
};

template <int maxPlots, int size> class Axes {
public:
	
	// -------------------------------------------------------------
	// -------------------- Axes Function List --------------------
	// -------------------------------------------------------------
	
	// Axes object constructor
	Axes(GAxis _xaxis={false, NAN, NAN, "."}, GAxis _yaxis={false, NAN, NAN, "."}, GDims _dims={1028, 800, 64});
	// add a plot of 'y' against 'x' to the Axes, returning the plot index of the new plot
	int AddPlot(int N, float x[], float y[], PlotType type=line, RGB plotColour=COLOURDEFAULT, bool lines[]=nullptr);
	// highlight the 'number' points 'lerpvalues' beyond 'indices' on plot 'plotIndex'
	void HighlightIndices(int plotIndex, int number, LerpedIndex indices[]);
	// plot lines across indices on a function of 2 variables
	int PlotIndices2D(int N, float x[], float y[], LerpedIndex2D indices[], float label=NAN, RGB plotColour=COLOURDEFAULT);
	// highlight indices in a function of 2 variables
	void HighlightIndex2D(float xs[], float ys[], LerpedIndex2D index);
	// draw a 2D vector field
	void AddVectorField2D(int SIZE, int gridSize, Vector2D vecs[], float xs[], float ys[], RGB colour=COLOURDEFAULT);
	// display the Axes
	void Execute();
	// save an image of the axes to the following location; this is done automatiaclly every time the axes are plotted
	// "/Users/Edzen098/Library/Developer/Xcode/DerivedData/<your project>/Build/Products/Debug"
	void Save();
	
	
	// ----- rest of Axes class and function definition -----
	
	~Axes() { Clean(); }
	bool Running() { return isRunning; }
	
private:
	bool Init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
	void InitAxes(float xs[], float ys[], int N=size);
	
	void MainPlot();

	void Clean();
    
	float AxisRound(float width);
	
	float GetXDrawPos(float xpos);
	float GetYDrawPos(float ypos);
	
	void DrawText(const char *text, int _x, int _y, TextAlign alignHorizontal=TextAlign::centre, TextAlign alignVertical=TextAlign::centre);
	void DrawTextVert(const char *text, int _x, int _y, TextAlign alignHorizontal=TextAlign::centre, TextAlign alignVertical=TextAlign::centre);
	void DrawHighlight(float xpos, float ypos);
	void DrawArrow(float x1, float y1, float x2, float y2, RGB colour, float headRatio=0.3);
	
	// plots:
	bool axesInitiated = false;
	
	int plots_n;
	Plot<size> plots[maxPlots];
    
    // sdl stuff
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool isRunning;
    
    // defaulted values
	GAxis xaxis;
	GAxis yaxis;
	GDims dims;
	
    // ease of draw variables
    int ox; // origin x position
    int oy; // origin y position
	int zx; // 0 x position
	int zy; // 0 y position
    float fx; // x distance multiplication factor
    float fy; // y distance multiplication factor
    
    // text stuff
    TTF_Font *textFont;
    //TTF_Font *bla = TTF_OpenFont(, )
    SDL_Color textColour;
    int xAxisLabelx;
	int xAxisLabely;
    int yAxisLabelx;
	int yAxisLabely;
};

template <int maxPlots, int size>  Axes<maxPlots, size>::Axes(GAxis _xaxis, GAxis _yaxis, GDims _dims) {
	// initialising text drawing functionality
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	
	xaxis = _xaxis;
	yaxis = _yaxis;
	dims = _dims;
	
	textColour = {0, 0, 0};
	textFont = TTF_OpenFont("/Library/Frameworks/Python.framework/Versions/3.8/lib/python3.8/site-packages/matplotlib/mpl-data/fonts/ttf/cmr10.ttf", 18);
	if(textFont == NULL) {
		printf("TTF_OpenFont: %s\n", TTF_GetError());
		exit(2);
	}
	
	plots_n = 0;
	
	Init("Plot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dims.width, dims.height, false);
	
	SDL_RenderClear(renderer);
}

template <int maxPlots, int size> bool Axes<maxPlots, size>::Init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen){
	
	int flags = 0;
	if(fullscreen){
		flags = SDL_WINDOW_FULLSCREEN;
	}
	
	isRunning = true;
	
	// subsystems:
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "Subsystems failed to initialise.\n";
		isRunning = false;
		return false;
	}
	//std::cout << "Subsystems initialised.\n";
	
	// window:
	window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
	if(!window){
		std::cout << "Window creation failed.\n";
		isRunning = false;
		return false;
	}
	//std::cout << "Window created.\n";
	
	// renderer:
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer){
		std::cout << "Renderer creation failed.\n";
		isRunning = false;
		return false;
	}
	//std::cout << "Renderer created.\n";
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	return true;
}

template <int maxPlots, int size> void Axes<maxPlots, size>::InitAxes(float xs[], float ys[], int N){
	// setting axes limits
	if(std::isnan(yaxis.min)){
		yaxis.min = FindMin(N, ys);
		if(yaxis.min > 0 && !yaxis.log){ yaxis.min = 0; }
	}
	if(std::isnan(yaxis.max)){
		yaxis.max = FindMax(N, ys);
		if(yaxis.max < 0){ yaxis.max = 0; }
	}
	if(std::isnan(xaxis.min)){
		xaxis.min = FindMin(N, xs);
		//if(xaxis.min > 0 && !xaxis.log){ xaxis.min = 0; }
	}
	if(std::isnan(xaxis.max)){
		xaxis.max = FindMax(N, xs);
		//if(xaxis.max < 0){ xaxis.max = 0; }
	}
	if(yaxis.min <= 0 || yaxis.max <= 0){ yaxis.log = false; }
	if(xaxis.min <= 0 || xaxis.max <= 0){ xaxis.log = false; }
	
	// variables for ease of drawing
	if(xaxis.log){ ox = dims.margin; } else {
		zx = dims.margin + (dims.width - 2*dims.margin)*LerpOf(0.0f, (float)xaxis.min, (float)xaxis.max);
		ox = ((xaxis.min <= 0 && xaxis.max >= 0) ? zx : dims.margin);
	}
	if(yaxis.log){ oy = dims.height - dims.margin; } else {
		zy = dims.height - dims.margin - (dims.height - 2*dims.margin)*LerpOf(0.0f, (float)yaxis.min, (float)yaxis.max);
		oy = ((yaxis.min <= 0 && yaxis.max >= 0) ? zy : dims.height - dims.margin);
	}
	if(xaxis.log){ fx = (dims.width - 2*dims.margin) / (log(xaxis.max) - log(xaxis.min)); }
		else{ fx = (dims.width - 2*dims.margin) / (xaxis.max - xaxis.min); }
	if(yaxis.log){ fy = (dims.height - 2*dims.margin) / (log(yaxis.max) - log(yaxis.min)); }
		else{ fy = (dims.height - 2*dims.margin) / (yaxis.max - yaxis.min); }
	xAxisLabelx = dims.width - dims.margin;
	xAxisLabely = dims.height - dims.margin/3;
	yAxisLabelx = dims.margin/3;
	yAxisLabely = dims.margin;
	
	// drawing
	char res[15];
	// grid lines and axes scales
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
	int min_gap = 96;
	int minorN = 10;
	// ...along x-axis
	if(xaxis.log){
		float labelWidth_minus_one = pow((float)xaxis.max/xaxis.min, (float)min_gap/(dims.width - 2*dims.margin)) - 1; // const ratio (-1)
		labelWidth_minus_one = AxisRound(labelWidth_minus_one);
		float labelWidth = labelWidth_minus_one + 1;
		int roundLog;
		for(int i=0; i>=0; i++){
			// label
			float x = xaxis.min * pow(labelWidth, i);
			if(x > xaxis.max){ break; }
			roundLog = 1 - floor(log10(x));
			if(roundLog < 0){ roundLog = 0; }
			FloatToStr(x, res, roundLog);
			DrawText(res, GetXDrawPos(x), oy + dims.margin/8, TextAlign::centre, TextAlign::negative);
			// grid line
			SDL_RenderDrawLine(renderer, GetXDrawPos(x), dims.height - dims.margin, GetXDrawPos(x), dims.margin);
			// minor grid lines
			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 0); // grey
			float gap = xaxis.min * (pow(labelWidth, i+1) - pow(labelWidth, i)) / minorN;
			for(int j=1; j<minorN; j++){
				float x_ = x + gap*j;
				if(x_ > xaxis.max){ break; }
				SDL_RenderDrawLine(renderer, GetXDrawPos(x_), dims.height - dims.margin, GetXDrawPos(x_), dims.margin);
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
		}
	} else {
		float labelWidth = (float)(xaxis.max - xaxis.min)*min_gap/(dims.width - 2*dims.margin); // const val
		int roundLog = round(log10(labelWidth));
		float gap = pow(10, roundLog); // round to nearest power of 10
		labelWidth = gap*ceil(labelWidth/gap);// round to nearest multiple of a power of 10
		if(labelWidth == gap){ gap /= 10; }
		if(roundLog > 0){ roundLog = 0; } else { roundLog *= -1; }
		for(int i=0; i>=0; i++){
			// label
			float x = i*labelWidth;
			if(x > xaxis.max){ break; }
			if(x > xaxis.min){
				// text
				FloatToStr(x, res, roundLog);
				DrawText(res, GetXDrawPos(x), oy + dims.margin/8, TextAlign::centre, TextAlign::negative);
				// grid line
				SDL_RenderDrawLine(renderer, GetXDrawPos(x), dims.height - dims.margin, GetXDrawPos(x), dims.margin);
			}
			// minor grid lines
			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 0); // grey
			for(float x_=x+gap; x_ < xaxis.max && x_ < x+labelWidth; x_+=gap){
				if(x_ < xaxis.min){ continue; }
				SDL_RenderDrawLine(renderer, GetXDrawPos(x_), dims.height - dims.margin, GetXDrawPos(x_), dims.margin);
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
		}
		for(int i=0; i<=0; i--){
			// label
			float x = i*labelWidth;
			if(x < xaxis.min){ break; }
			if(i != 0 && x < xaxis.max){
				// text
				FloatToStr(x, res, roundLog);
				DrawText(res, GetXDrawPos(x), oy + dims.margin/8, TextAlign::centre, TextAlign::negative);
				// grid line
				SDL_RenderDrawLine(renderer, GetXDrawPos(x), dims.height - dims.margin, GetXDrawPos(x), dims.margin);
			}
			// minor grid lines
			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 0); // grey
			for(float x_=x-gap; x_ > xaxis.min && x_ > x-labelWidth; x_-=gap){
				if(x_ > xaxis.max){ continue; }
				SDL_RenderDrawLine(renderer, GetXDrawPos(x_), dims.height - dims.margin, GetXDrawPos(x_), dims.margin);
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
		}
	}
	// ...along y-axis
	if(yaxis.log){
		float labelWidth_minus_one = pow((float)yaxis.max/yaxis.min, (float)min_gap/(dims.height - 2*dims.margin)) - 1; // const ratio (-1)
		labelWidth_minus_one = AxisRound(labelWidth_minus_one);
		float labelWidth = labelWidth_minus_one + 1;
		int roundLog;
		for(int i=0; i>=0; i++){
			// label
			float y = yaxis.min * pow(labelWidth, i);
			if(y > yaxis.max){ break; }
			roundLog = 1 - floor(log10(y));
			if(roundLog < 0){ roundLog = 0; }
			FloatToStr(y, res, roundLog);
			DrawText(res, ox - dims.margin/8, GetYDrawPos(y), TextAlign::negative, TextAlign::centre);
			// grid line
			SDL_RenderDrawLine(renderer, dims.margin, GetYDrawPos(y), dims.width-dims.margin, GetYDrawPos(y));
			// minor grid lines
			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 0); // grey
			float gap = yaxis.min * (pow(labelWidth, i+1) - pow(labelWidth, i)) / minorN;
			for(int j=1; j<minorN; j++){
				float y_ = y + gap*j;
				if(y_ > yaxis.max){ break; }
				SDL_RenderDrawLine(renderer, dims.margin, GetYDrawPos(y_), dims.width-dims.margin, GetYDrawPos(y_));
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
		}
	} else {
		float labelWidth = (float)(yaxis.max - yaxis.min)*min_gap/(dims.height - 2*dims.margin); // const val
		int roundLog = round(log10(labelWidth));
		float gap = pow(10, roundLog); // round to nearest power of 10
		labelWidth = gap*ceil(labelWidth/gap);// round to nearest multiple of a power of 10
		if(labelWidth == gap){ gap /= 10; }
		if(roundLog > 0){ roundLog = 0; } else { roundLog *= -1; }
		for(int i=0; i>=0; i++){
			// label
			float y = i*labelWidth;
			if(y > yaxis.max){ break; }
			if(y > yaxis.min){
				// text
				FloatToStr(y, res, roundLog);
				DrawText(res, ox - dims.margin/8, GetYDrawPos(y), TextAlign::negative, TextAlign::centre);
				// grid line
				SDL_RenderDrawLine(renderer, dims.margin, GetYDrawPos(y), dims.width - dims.margin, GetYDrawPos(y));
			}
			// minor grid lines
			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 0); // grey
			for(float y_=y+gap; y_ < yaxis.max && y_ < y+labelWidth; y_+=gap){
				if(y_ < yaxis.min){ continue; }
				SDL_RenderDrawLine(renderer, dims.margin, GetYDrawPos(y_), dims.width - dims.margin, GetYDrawPos(y_));
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
		}
		for(int i=0; i<=0; i--){
			// label
			float y = i*labelWidth;
			if(y < yaxis.min){ break; }
			if(i != 0 && y < yaxis.max){
				// text
				FloatToStr(y, res, roundLog);
				DrawText(res, ox - dims.margin/8, GetYDrawPos(y), TextAlign::negative, TextAlign::centre);
				// grid line
				SDL_RenderDrawLine(renderer, dims.margin, GetYDrawPos(y), dims.width - dims.margin, GetYDrawPos(y));
			}
			// minor grid lines
			SDL_SetRenderDrawColor(renderer, 220, 220, 220, 0); // grey
			for(float y_=y-gap; y_ > yaxis.min && y_ > y-labelWidth; y_-=gap){
				if(y_ > yaxis.max){ continue; }
				SDL_RenderDrawLine(renderer, dims.margin, GetYDrawPos(y_), dims.width - dims.margin, GetYDrawPos(y_));
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
		}
	}
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // black
	// x axis
	SDL_RenderDrawLine(renderer, dims.margin, oy, dims.width - dims.margin, oy);
	SDL_RenderDrawLine(renderer, dims.margin, oy+1, dims.width - dims.margin, oy+1);
	SDL_RenderDrawLine(renderer, dims.margin, oy-1, dims.width - dims.margin, oy-1);
	// y axis
	SDL_RenderDrawLine(renderer, ox, dims.height - dims.margin, ox, dims.margin);
	SDL_RenderDrawLine(renderer, ox-1, dims.height - dims.margin, ox-1, dims.margin);
	SDL_RenderDrawLine(renderer, ox+1, dims.height - dims.margin, ox+1, dims.margin);
	
	// axes titles
	if(xaxis.title[0] == '.'){
		if(xaxis.log){
			DrawText("logarithmic x-axis", xAxisLabelx, xAxisLabely, TextAlign::negative, TextAlign::centre);
		} else {
			DrawText("x-axis", xAxisLabelx, xAxisLabely, TextAlign::negative, TextAlign::centre);
		}
	} else {
		DrawText(xaxis.title, xAxisLabelx, xAxisLabely, TextAlign::negative, TextAlign::centre);
	}
	if(yaxis.title[0] == '.'){
		if(yaxis.log){
			DrawTextVert("logarithmic y-axis", yAxisLabelx, yAxisLabely, TextAlign::centre, TextAlign::negative);
		} else {
			DrawTextVert("y-axis", yAxisLabelx, yAxisLabely, TextAlign::centre, TextAlign::negative);
		}
	} else {
		DrawTextVert(yaxis.title, yAxisLabelx, yAxisLabely, TextAlign::centre, TextAlign::negative);
	}
	
	axesInitiated = true;
}

template <int maxPlots, int size> float Axes<maxPlots, size>::AxisRound(float width){
	int roundLog = round(log10(width));
	double ptd = pow(10, roundLog); // round to nearest power of 10
	return ptd*ceil(width/ptd);// round to nearest multiple of a power of 10
}

template <int maxPlots, int size> float Axes<maxPlots, size>::GetXDrawPos(float xpos){
	if(xaxis.log){
		return ox + fx*(log(xpos) - log(xaxis.min));
	}
	return zx + fx*xpos;
}
template <int maxPlots, int size> float Axes<maxPlots, size>::GetYDrawPos(float ypos){
	if(yaxis.log){
		return oy - fy*(log(ypos) - log(yaxis.min));
	}
	return zy - fy*ypos;
}

template <int maxPlots, int size> void Axes<maxPlots, size>::MainPlot(){
	
	for(int i=0; i<plots_n; i++){
		SDL_SetRenderDrawColor(renderer, plots[i].colour.red, plots[i].colour.green, plots[i].colour.blue, 255);
		for(int j=0; j<plots[i].N; j++){
			if(plots[i].drawLines[j]){
				switch(plots[i].type){
					case line:
						if(j < plots[i].N-1){
							SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j]), GetYDrawPos(plots[i].yValues[j]), GetXDrawPos(plots[i].xValues[j+1]), GetYDrawPos(plots[i].yValues[j+1]));
						}
						break;
					case scatter:
						SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j])-4, GetYDrawPos(plots[i].yValues[j])-4, GetXDrawPos(plots[i].xValues[j])+5, GetYDrawPos(plots[i].yValues[j])+5);
						SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j])-4, GetYDrawPos(plots[i].yValues[j])-3, GetXDrawPos(plots[i].xValues[j])+4, GetYDrawPos(plots[i].yValues[j])+5);
						SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j])-3, GetYDrawPos(plots[i].yValues[j])-4, GetXDrawPos(plots[i].xValues[j])+5, GetYDrawPos(plots[i].yValues[j])+4);
						
						SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j])+4, GetYDrawPos(plots[i].yValues[j])-4, GetXDrawPos(plots[i].xValues[j])-5, GetYDrawPos(plots[i].yValues[j])+5);
						SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j])+3, GetYDrawPos(plots[i].yValues[j])-4, GetXDrawPos(plots[i].xValues[j])-5, GetYDrawPos(plots[i].yValues[j])+4);
						SDL_RenderDrawLine(renderer, GetXDrawPos(plots[i].xValues[j])+4, GetYDrawPos(plots[i].yValues[j])-3, GetXDrawPos(plots[i].xValues[j])-4, GetYDrawPos(plots[i].yValues[j])+5);
						break;
					default:
						break;
				}
			}
		}
	}
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0); // back to white
	
	Save();
	
	SDL_RenderPresent(renderer);
}

template <int maxPlots, int size> void Axes<maxPlots, size>::Clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

template <int maxPlots, int size> void Axes<maxPlots, size>::DrawText(const char *text, int _x, int _y, TextAlign alignHorizontal, TextAlign alignVertical){
	SDL_Rect destRect;
	SDL_Surface *surface = TTF_RenderText_Solid(textFont, text, textColour);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
	destRect.x = _x - (destRect.w*(int)alignHorizontal)/2;
	destRect.y = _y - (destRect.h*(2 - (int)alignVertical))/2;
	SDL_RenderCopy(renderer, texture, NULL, &destRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

template <int maxPlots, int size> void Axes<maxPlots, size>::DrawTextVert(const char *text, int _x, int _y, TextAlign alignHorizontal, TextAlign alignVertical){
	SDL_Rect destRect;
	SDL_Surface *surface = TTF_RenderText_Solid(textFont, text, textColour);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_QueryTexture(texture, NULL, NULL, &destRect.w, &destRect.h);
	SDL_Point point;
	point.x = (destRect.w*(int)alignVertical)/2;
	point.y = (destRect.h*(2 - (int)alignHorizontal))/2;
	destRect.x = _x - point.x;
	destRect.y = _y - point.y;
	SDL_RenderCopyEx(renderer, texture, NULL, &destRect, -90, &point, SDL_FLIP_NONE);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

template <int maxPlots, int size> void Axes<maxPlots, size>::DrawHighlight(float xpos, float ypos){
	SDL_RenderDrawLine(renderer, GetXDrawPos(xpos) - 12, GetYDrawPos(ypos), GetXDrawPos(xpos) + 12, GetYDrawPos(ypos));
	SDL_RenderDrawLine(renderer, GetXDrawPos(xpos), GetYDrawPos(ypos) - 12, GetXDrawPos(xpos), GetYDrawPos(ypos) + 12);
	char str[63];
	int i=0;
	str[i] = '('; i++;
	i += FloatToStr(xpos, &str[i], 4);
	str[i] = ','; i++;
	str[i] = ' '; i++;
	i += FloatToStr(ypos, &str[i], 4);
	str[i] = ')'; i++;
	str[i] = '\0'; i++;
	DrawText(str, GetXDrawPos(xpos) + 16, GetYDrawPos(ypos) + 16, TextAlign::positive, TextAlign::positive);
}




// ---------- user friendly function definitions ----------

template <int maxPlots, int size> int Axes<maxPlots, size>::AddPlot(int N, float x[], float y[], PlotType type, RGB plotColour, bool lines[]) {
	if(plots_n >= maxPlots){ std::cout << "Max plots already reached.\n"; return -1; }
	
	ArrCopy(N, plots[plots_n].xValues, x);
	ArrCopy(N, plots[plots_n].yValues, y);
	if(lines == nullptr){
		for(int i=0; i<N; i++){ plots[plots_n].drawLines[i] = true; }
	} else {
		ArrCopy(N, plots[plots_n].drawLines, lines);
	}
	plots[plots_n].N = N;
	plots[plots_n].type = type;
	plots[plots_n].colour = plotColour;
	plots_n++;
	
	if(!axesInitiated){
		InitAxes(x, y, N);
	}
	
	return plots_n-1;
}

// choose some points along the x axis to highlight on the Axes
template <int maxPlots, int size> void Axes<maxPlots, size>::HighlightIndices(int plotIndex, int number, LerpedIndex indices[]){
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
	float _x;
	float _y;
	for(int i=0; i<number; i++){
		_x = plots[plotIndex].xValues[indices[i].i1] + indices[i].lerpValue * (plots[plotIndex].xValues[indices[i].i2] - plots[plotIndex].xValues[indices[i].i1]);
		_y = plots[plotIndex].yValues[indices[i].i1] + indices[i].lerpValue * (plots[plotIndex].yValues[indices[i].i2] - plots[plotIndex].yValues[indices[i].i1]);
		
		DrawHighlight(_x, _y);
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // back to white
}


// highlight the 'number' points 'lerpvalues' beyond 'indices' on plot 'plotIndex'
template <int maxPlots, int size> int Axes<maxPlots, size>::PlotIndices2D(int N, float x[], float y[], LerpedIndex2D indices[], float label, RGB plotColour){
	if(plots_n >= maxPlots){ std::cout << "Max plots already reached.\n"; return -1; }
	
	// to fill
	float *_xs = (float*)calloc(2*N, sizeof(float));
	float *_ys = (float*)calloc(2*N, sizeof(float));
	bool *_lines = (bool*)calloc(2*N, sizeof(bool));
	// calculation variables
	float deltaY, deltaX;
	float Lsquared, l;
	double alpha, theta;
	float lerpValue;
	
	int pointIndex = 0;
	for(int n=0; n<N-1; n++){
		pointIndex = 2*n;
		
		lerpValue = indices[n].lerpValue;
		deltaY = y[indices[n].j2] - y[indices[n].j1];
		deltaX = x[indices[n].i2] - x[indices[n].i1];
		Lsquared = deltaY*deltaY + deltaX*deltaX;
		alpha = atan(deltaY/deltaX) + ( (deltaX < 0) ? PI : 0 );
		l = sqrt(Lsquared*(lerpValue*lerpValue + 1.0f/4.0f));
		theta = atan(1/(2*lerpValue)) +  + ( (lerpValue < 0) ? PI : 0 );
		
		_xs[pointIndex] = x[indices[n].i1] + l*cos(alpha + theta);
		_ys[pointIndex] = y[indices[n].j1] + l*sin(alpha + theta);
		_xs[pointIndex+1] = x[indices[n].i1] + l*cos(alpha - theta);
		_ys[pointIndex+1] = y[indices[n].j1] + l*sin(alpha - theta);
		_lines[pointIndex] = true;
		_lines[pointIndex+1] = false;
	}
	int plotIndex = AddPlot(2*N, _xs, _ys, line, plotColour, _lines);
	
	if(!std::isnan(label)){
		int ind = (int)(N * (rand() % 50)/25);
		char text[5];
		FloatToStr(label, text, 2);
		DrawText(text, GetXDrawPos(_xs[ind]), GetYDrawPos(_ys[ind]));
	}
	
	free(_xs);
	free(_ys);
	free(_lines);
	
	return plotIndex;
}

// finally plot the function and complete process
template <int maxPlots, int size> void Axes<maxPlots, size>::Execute(){
	
	MainPlot();
	
	while(isRunning == true){
		SDL_PollEvent(&event);
		if(event.type == SDL_QUIT){
			isRunning = false;
			break;
		}
	}
}

template <int maxPlots, int size> void Axes<maxPlots, size>::HighlightIndex2D(float xs[], float ys[], LerpedIndex2D index){
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
	float _x;
	float _y;
	_x = xs[index.i1] + index.lerpValue * (xs[index.i2] - xs[index.i1]);
	_y = ys[index.j1] + index.lerpValue * (ys[index.j2] - ys[index.j1]);
		
	DrawHighlight(_x, _y);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // back to white
}

template <int maxPlots, int size> void Axes<maxPlots, size>::AddVectorField2D(int SIZE, int gridSize, Vector2D vecs[], float xs[], float ys[], RGB colour){
	if(!axesInitiated){
		InitAxes(xs, ys, SIZE);
	}
	
	float cellSize = (xaxis.max - xaxis.min)/gridSize;
	// columns is gridSize
	int rows = (int)( (yaxis.max - yaxis.min)/cellSize );
	
	float maxLength = cellSize * ROOT2;
	
	int xIndex;
	int yIndex;
	float *sqMags = (float*)calloc(gridSize*rows, sizeof(float));
	for(int j=0; j<rows; j++){
		for(int i=0; i<gridSize; i++){
			xIndex = (int)( i*SIZE/gridSize );
			yIndex = (int)( j*SIZE/rows );
			sqMags[I2D(gridSize, i, j)] = vecs[I2D(SIZE, xIndex, yIndex)].SqMag();
		}
	}
	
	float factor = maxLength / sqrt( FindMax(gridSize*rows - 1, sqMags) );
	
	Vector2D vecW;
	for(int j=0; j<rows; j++){
		for(int i=0; i<gridSize; i++){
			xIndex = (int)( i*SIZE/gridSize );
			yIndex = (int)( j*SIZE/rows );
			vecW = vecs[I2D(SIZE, xIndex, yIndex)];
			DrawArrow(xs[xIndex], ys[yIndex], xs[xIndex] + vecW.x*factor, ys[yIndex] + vecW.y*factor, colour);
		}
	}
	
	free(sqMags);
}

template <int maxPlots, int size> void Axes<maxPlots, size>::DrawArrow(float x1, float y1, float x2, float y2, RGB colour, float headRatio){
	SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 255);
	
	SDL_RenderDrawLine(renderer, GetXDrawPos(x1), GetYDrawPos(y1), GetXDrawPos(x2), GetYDrawPos(y2));
	float len = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	double angle;
	angle = ATanRatio(y2 - y1, x2 - x1);
	float a = len * headRatio * 0.5f;
	float ax = x1 + len * (1 - headRatio) * cos(angle);
	float ay = y1 + len * (1 - headRatio) * sin(angle);
	float dx = a * sin(angle);
	float dy = a * cos(angle);
	SDL_RenderDrawLine(renderer, GetXDrawPos(x2), GetYDrawPos(y2), GetXDrawPos(ax + dx), GetYDrawPos(ay - dy));
	SDL_RenderDrawLine(renderer, GetXDrawPos(x2), GetYDrawPos(y2), GetXDrawPos(ax - dx), GetYDrawPos(ay + dy));
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // back to white
}

template <int maxPlots, int size> void Axes<maxPlots, size>::Save(){
	// Create SDL_Surface with depth of 32 bits
	SDL_Surface *surface = SDL_CreateRGBSurface( 0, dims.width, dims.height, 32, 0, 0, 0, 0 );
	if(surface == NULL){
		std::cout << "Cannot create SDL_Surface: " << SDL_GetError() << std::endl;
		return;
	}
	// Get data from SDL_Renderer and save them into surface
	if(SDL_RenderReadPixels( renderer, NULL, surface->format->format, surface->pixels, surface->pitch ) != 0){
		std::cout << "Cannot read data from SDL_Renderer: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(surface);
		return;
	}
	// Save screenshot as PNG file
	if(IMG_SavePNG(surface, "Axes_output.png") != 0){
		std::cout << "Cannot save PNG file: " << SDL_GetError() << std::endl;
		SDL_FreeSurface(surface);
		return;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

#endif /* Axes_hpp */
