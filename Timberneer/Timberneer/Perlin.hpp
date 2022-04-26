#ifndef Perlin_h
#define Perlin_h

#include "Header.hpp"

// -- Perlin Noise Stuff
template <typename T> T Ease(T x){
	return (T)(3.0f*x*x - 2.0f*x*x*x);
}
// find value at 'fraction' of way from 'start' to 'stop'
template <typename T> T Lerp(float fraction, T start, T stop){
	return start + (T)(fraction*(stop - start));
}
template <int size> class PerlinNoise2D {
public:
	PerlinNoise2D() {
		for(int j=0; j<size; j++){
			for(int i=0; i<size; i++){
				field[j][i] = V2_RandomUnit();
			}
		}
	}
	
	// produces value between -0.5f and 0.5f
	float GetVal(const float& x, const float& y){
		// Determine grid cell coordinates
		const int x0 = (int)x;
		const int y0 = (int)y;
		if(x0 < 0 || x0 >= size ||
		   y0 < 0 || y0 >= size){
			std::cout << "ERROR: Perlin position out of range." << std::endl;
			return 0.0f;
		}
		const int x1 = x0 + 1;
		const int y1 = y0 + 1;
		
		// Could also use higher order polynomial/s-curve here
		const float sx = Ease(x - (float)x0);
		const float sy = Ease(y - (float)y0);
		
		float n0, n1;
		
		n0 = DotGridGradient(x0, y0, x, y);
		n1 = DotGridGradient(x1, y0, x, y);
		const float ix0 = Lerp(sx, n0, n1);

		n0 = DotGridGradient(x0, y1, x, y);
		n1 = DotGridGradient(x1, y1, x, y);
		const float ix1 = Lerp(sx, n0, n1);
		
		return Lerp(sy, ix0, ix1);
	}
	
	vec2 field[size][size];
	
private:
	// Computes the dot product of the distance and gradient vectors.
	float DotGridGradient(const int& ix, const int& iy, const float& x, const float& y) {
		// Compute the distance vector
		const float dx = x - (float)ix;
		const float dy = y - (float)iy;
		vec2 vec = field[iy % size][ix % size];
		// Compute the dot-product
		return (dx*vec.x + dy*vec.y);
	}
};

template <int perlinSize, int mapSizeCells> void MakeHeightMap(PerlinNoise2D<perlinSize> *perlin, const float& heightScale, float out[mapSizeCells][mapSizeCells]){
	float perlinScale = (float)perlinSize / (float)mapSizeCells;
	for(int j=0; j<mapSizeCells; j++) for(int i=0; i<mapSizeCells; i++) out[j][i] = heightScale * perlin->GetVal((float)i * perlinScale, (float)j * perlinScale);
}

template <int size> ObjectData MakeTerrain(const float heightMap[size][size], const float& cellSize, const char *usemtl, const int& texSize, vec3 normalMapOut[size][size]){
	ObjectData ret;
	ret.divisionsN = 1;
	ret.vertices_n = 6 * (size - 1) * (size - 1);
	
	// positions
	float x0, y0;
	x0 = y0 = - (float)size * 0.5f * cellSize;
	vec3 posVecs[size][size];
	for(unsigned int r=0; r<size; r++){
		for(unsigned int c=0; c<size; c++){
			posVecs[r][c] = {x0 + cellSize*(float)c, heightMap[r][c], y0 + cellSize*(float)r};
		}
	}
	
	// texture coordinates
	const float texSizeInv = 1.0f / (float)(texSize);
	float tcYs[size];
	for(unsigned int r=0; r<size; r++) tcYs[r] = (float)(r % texSize) * texSizeInv;
	float tcXs[size];
	for(unsigned int c=0; c<size; c++) tcXs[c] = (float)(c % texSize) * texSizeInv;
	
	// normals
	for(unsigned int r=0; r<size; r++){
		for(unsigned int c=0; c<size; c++){
			vec3 v1 = posVecs[r][(c + 1) % size] - posVecs[r][(c - 1) % size];
			vec3 v2 = posVecs[(r + 1) % size][c] - posVecs[(r - 1) % size][c];
			normalMapOut[r][c] = V3_Cross(v1, v2).Normalise();
			if(normalMapOut[r][c].y < 0) normalMapOut[r][c] *= -1.0f;
		}
	}
	
	// vertex data
	obj_v positions[ret.vertices_n];
	obj_vt texCoords[ret.vertices_n];
	obj_vn normals[ret.vertices_n];
	unsigned int index = 0;
	for(unsigned int r=0; r<size-1; r++){
		for(unsigned int c=0; c<size-1; c++){
			float tcx0 = tcXs[c];
			float tcx1 = tcXs[c + 1] < tcXs[c] ? 1.0f : tcXs[c + 1];
			float tcy0 = tcYs[r];
			float tcy1 = tcYs[r + 1] < tcYs[r] ? 1.0f : tcYs[r + 1];
			
			positions[index] = VectorToOBJV(posVecs[r][c]);
			normals[index] = VectorToOBJVN(normalMapOut[r][c]);
			texCoords[index][0] = tcx0;
			texCoords[index][1] = tcy0;
			index++;
			
			positions[index] = VectorToOBJV(posVecs[r + 1][c]);
			normals[index] = VectorToOBJVN(normalMapOut[r + 1][c]);
			texCoords[index][0] = tcx0;
			texCoords[index][1] = tcy1;
			index++;
			
			positions[index] = VectorToOBJV(posVecs[r][c + 1]);
			normals[index] = VectorToOBJVN(normalMapOut[r][c + 1]);
			texCoords[index][0] = tcx1;
			texCoords[index][1] = tcy0;
			index++;
			
			positions[index] = VectorToOBJV(posVecs[r + 1][c]);
			normals[index] = VectorToOBJVN(normalMapOut[r + 1][c]);
			texCoords[index][0] = tcx0;
			texCoords[index][1] = tcy1;
			index++;
			
			positions[index] = VectorToOBJV(posVecs[r + 1][c + 1]);
			normals[index] = VectorToOBJVN(normalMapOut[r + 1][c + 1]);
			texCoords[index][0] = tcx1;
			texCoords[index][1] = tcy1;
			index++;
			
			positions[index] = VectorToOBJV(posVecs[r][c + 1]);
			normals[index] = VectorToOBJVN(normalMapOut[r][c + 1]);
			texCoords[index][0] = tcx1;
			texCoords[index][1] = tcy0;
			index++;
		}
	}
	
	// storing in output
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

#endif /* Perlin_h */
