#include <iostream>
#include <random>

#include <SDL2/SDL.h>

const int W = 60;
const int H = 20;

enum class Dir { none, right, up, left, down };

struct TileCompatibility {
	uint8_t right, up, left, down;
	
	friend TileCompatibility operator|(const TileCompatibility &a, const TileCompatibility &b){
		return {(uint8_t)(a.right | b.right), (uint8_t)(a.up | b.up), (uint8_t)(a.left | b.left), (uint8_t)(a.down | b.down)};
	}
	
	TileCompatibility operator|=(const TileCompatibility &a){
		*this = *this | a;
		return *this;
	}
};

const int N = 7;
enum Tile : uint8_t {
	grass = 0x01,
	hWall = 0x02,
	vWall = 0x04,
	wallEndTop = 0x08,
	wallEndLeft = 0x10,
	wallEndBottom = 0x20,
	wallEndRight = 0x40
};

const char chars[N] = {'G', 'H', 'V', 't', 'l', 'b', 'r'};

const unsigned short tileWeightings[N] = {15, 5, 5, 1, 1, 1, 1};

TileCompatibility tileCompatibilities[N] = {
	// grass with all
	{
		grass | vWall | wallEndLeft | wallEndBottom | wallEndTop,
		grass | hWall | wallEndLeft | wallEndBottom | wallEndRight,
		grass | vWall | wallEndRight | wallEndBottom | wallEndTop,
		grass | hWall | wallEndLeft | wallEndTop | wallEndRight
	},
	// hWall with all but grass
	{
		hWall | wallEndRight,
		wallEndLeft | wallEndBottom | wallEndRight,
		hWall | wallEndLeft,
		wallEndLeft | wallEndTop | wallEndRight
	},
	// vWall with all but hWall & grass
	{
		wallEndLeft | wallEndBottom | wallEndTop,
		vWall | wallEndTop,
		wallEndRight | wallEndBottom | wallEndTop,
		vWall | wallEndBottom
	},
	// wallEndTop with all but grass, hWall & vWall
	{
		wallEndLeft | wallEndTop | wallEndBottom,
		wallEndLeft | wallEndRight | wallEndBottom,
		wallEndTop | wallEndBottom | wallEndRight,
		wallEndBottom
	},
	// wallEndLeft without wallEndTop etc.
	{
		wallEndRight,
		wallEndLeft | wallEndRight | wallEndBottom,
		wallEndRight | wallEndBottom,
		wallEndLeft | wallEndRight
	},
	// wallEndBottom without wallEndTop, wallEndLeft etc.
	{
		wallEndBottom,
		0x00,
		wallEndBottom | wallEndRight,
		wallEndRight
	},
	// wallEndRight with just itself
	{
		0x00,
		wallEndRight,
		0x00,
		wallEndRight
	}
};

bool HandleEvents(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT: return true;
			case SDL_KEYDOWN: {
				switch(event.key.keysym.sym){
					case SDLK_ESCAPE: return true;
					default: break;
				}
			}
			default: break;
		}
	}
	return false;
}

void Update(const float &dT){
	
}

void Render(SDL_Renderer *const &renderer){
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	
	//
	
	SDL_RenderPresent(renderer);
}

uint8_t ChooseTile(const uint8_t &possibilities){
	if(!possibilities){ std::cout << "Error: No tiles to choose from!\n"; return 0x01; }
	int buffer[N];
	int n = 0;
	int sum = 0;
	for(int i=0; i<N; i++){
		if(possibilities & (0x01 << i)){
			buffer[n++] = i;
			sum += tileWeightings[i];
		}
	}
	if(sum) sum = rand() % sum;
	for(int i=0; i<n; i++){
		if(sum < tileWeightings[buffer[i]]) return (0x01 << buffer[i]);
		sum -= tileWeightings[buffer[i]];
	}
	std::cout << "Choose tile error!\n";
	return 0x01;
}

TileCompatibility CombinedCompatibilities(const uint8_t &possibilities){
	TileCompatibility ret = {0x00, 0x00, 0x00, 0x00};
	uint8_t checkBit = 0x01;
	for(int i=0; i<N; i++){
		if(possibilities & checkBit) ret |= tileCompatibilities[i];
		checkBit <<= 1;
	}
	return ret;
}

template <int H, int W> int PropagatePossibilities(const int &r, const int &c, const uint8_t &newRestrictions, uint8_t tilePossibilities[H][W], bool tilesSet[H][W], const Dir &sourceDirection){
	// tile already set?
	if(tilesSet[r][c]) return 0;

	// changing tile
	const uint8_t old = tilePossibilities[r][c];
	tilePossibilities[r][c] &= newRestrictions;
	
	// tile not changed?
	if(!(old ^ tilePossibilities[r][c])) return 0;
	
	int ret = 0;
	
	// tile set?
	int count = 0;
	for(int i=0; i<N; i++) if(tilePossibilities[r][c] & (0x01 << i)) count++;
	if(count <= 1){
		tilesSet[r][c] = true;
		ret++;
	}
	
	TileCompatibility combinedCompatibilities = CombinedCompatibilities(tilePossibilities[r][c]);
	
	if(sourceDirection != Dir::right)	if(!tilesSet[r][c + 1]) if(c < W-1)	ret += PropagatePossibilities<H, W>(r, c + 1, combinedCompatibilities.right,	tilePossibilities, tilesSet, Dir::left);
	if(sourceDirection != Dir::up)		if(!tilesSet[r - 1][c]) if(r > 0)	ret += PropagatePossibilities<H, W>(r - 1, c, combinedCompatibilities.up,		tilePossibilities, tilesSet, Dir::down);
	if(sourceDirection != Dir::left)	if(!tilesSet[r][c - 1]) if(c > 0)	ret += PropagatePossibilities<H, W>(r, c - 1, combinedCompatibilities.left,	tilePossibilities, tilesSet, Dir::right);
	if(sourceDirection != Dir::down)	if(!tilesSet[r + 1][c]) if(r < H-1)	ret += PropagatePossibilities<H, W>(r + 1, c, combinedCompatibilities.down,	tilePossibilities, tilesSet, Dir::up);
	
	return ret;
}

int main(int argc, const char * argv[]) {
	// completing tile compatibilities:
	for(int i=0; i<N-1; i++){
		for(int j=i+1; j<N; j++){
			if(tileCompatibilities[i].left & (0x01 << j)) tileCompatibilities[j].right |= (0x01 << i);
			if(tileCompatibilities[i].down & (0x01 << j)) tileCompatibilities[j].up |= (0x01 << i);
			if(tileCompatibilities[i].right & (0x01 << j)) tileCompatibilities[j].left |= (0x01 << i);
			if(tileCompatibilities[i].up & (0x01 << j)) tileCompatibilities[j].down |= (0x01 << i);
		}
	}
	
	SDL_Window *window;
	SDL_Renderer *renderer;
	
	window = SDL_CreateWindow("Wave Function Collapse", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, NULL);
	
	renderer = SDL_CreateRenderer(window, NULL, NULL);
	
	bool tilesSet[H][W];
	uint8_t tilePossibilities[H][W];
	for(int r=0; r<H; r++){
		for(int c=0; c<W; c++){
			tilePossibilities[r][c] = 0xFF;
			tilesSet[r][c] = false;
		}
	}
	
	srand(0);
	
	int unsetLeft = W*H;
	while(unsetLeft){
		int chosenFromLeft = rand() % unsetLeft;
		int j = 0; int i_ = 0;
		while(i_ < chosenFromLeft || tilesSet[j / W][j % W]){
			if(!tilesSet[j / W][j % W]) i_++;
			j++;
		}
		const int r = j / W;
		const int c = j % W;
		unsetLeft -= PropagatePossibilities<H, W>(r, c, ChooseTile(tilePossibilities[r][c]), tilePossibilities, tilesSet, Dir::none);
	}
	
	for(int r=0; r<H; r++){
		for(int c=0; c<W; c++){
			int i=0;
			while(!((tilePossibilities[r][c] >> i) & 0x01)) i++;
			std::cout << chars[i];
		}
		std::cout << "\n";
	}
	
	int prevTime = SDL_GetTicks();
	while(true){
		if(HandleEvents()) break;
		
		int time = SDL_GetTicks();
		int ticks = time - prevTime;
		Update((float)ticks * 0.001f);
		prevTime = time;
		
		Render(renderer);
	}
	
	return 0;
}
