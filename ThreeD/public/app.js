import GL from "/gl.js";
import {m4} from "/mat4.js";
import {v3} from "/vec3.js";
import {perlin2} from "/perlin.js";
import World from "/world.js";

/*window.CANVAS_WIDTH;
window.CANVAS_HEIGHT;
window.mousePos = {x: 0, y: 0};*/

window.canvas;
var renderer3d;
var rect;

var scrollAmount = 0;
var scrollAmount2 = 0;

var world;

var updateObjects = [];
window.UpdateMe = function(object){ updateObjects.push(object); }
window.DontUpdateMe = function(object){ updateObjects.splice(updateObjects.indexOf(object), 1); }

document.addEventListener("DOMContentLoaded", event => {
	const app = firebase.app();
	//console.log(app);
	
	window.canvas = document.getElementById("gameScreen");
	window.canvas.setAttribute('width', window.innerWidth - 32);
	window.canvas.setAttribute('height', window.innerHeight - 32);
	rect = window.canvas.getBoundingClientRect();
	window.CANVAS_WIDTH = window.canvas.width;
	window.CANVAS_HEIGHT = window.canvas.height;
	
	renderer3d = new GL(canvas);
	
	world = new World(renderer3d);
	
	Loop(0);
});

let lastTime = 0;
function Loop(timeStamp){
	let deltaTime = (timeStamp - lastTime) / 1000/1;
	lastTime = timeStamp;
	if(deltaTime){
		// updating
		updateObjects.forEach(object => { object.Update(deltaTime); });
		
		// rendering
		renderer3d.Render(world.player.transformMatrix);
	}
	
	// calling the loop again
	requestAnimationFrame(Loop);
};

/*
window.addEventListener("wheel", event => {
	scrollAmount += event.deltaY;
	scrollAmount2 += event.deltaX;
});*/

window.parseOBJ = function(text) {
	// because indices are base 1 let's just fill in the 0th data
 const objPositions = [[0, 0, 0]];
 const objTexcoords = [[0, 0]];
 const objNormals = [[0, 0, 0]];

 // same order as `f` indices
 const objVertexData = [
   objPositions,
   objTexcoords,
   objNormals,
 ];

 // same order as `f` indices
 let webglVertexData = [
   [],   // positions
   [],   // texcoords
   [],   // normals
 ];

 function newGeometry() {
   // If there is an existing geometry and it's
   // not empty then start a new one.
   if (geometry && geometry.data.position.length) {
	 geometry = undefined;
   }
   setGeometry();
 }

 function addVertex(vert) {
   const ptn = vert.split('/');
   ptn.forEach((objIndexStr, i) => {
	 if (!objIndexStr) {
	   return;
	 }
	 const objIndex = parseInt(objIndexStr);
	 const index = objIndex + (objIndex >= 0 ? 0 : objVertexData[i].length);
	 webglVertexData[i].push(...objVertexData[i][index]);
   });
 }

 const keywords = {
   v(parts) {
	 objPositions.push(parts.map(parseFloat));
   },
   vn(parts) {
	 objNormals.push(parts.map(parseFloat));
   },
   vt(parts) {
	 // should check for missing v and extra w?
	 objTexcoords.push(parts.map(parseFloat));
   },
   f(parts) {
	 const numTriangles = parts.length - 2;
	 for (let tri = 0; tri < numTriangles; ++tri) {
	   addVertex(parts[0]);
	   addVertex(parts[tri + 1]);
	   addVertex(parts[tri + 2]);
	 }
   },
 };

 const keywordRE = /(\w*)(?: )*(.*)/;
 const lines = text.split('\n');
 for (let lineNo = 0; lineNo < lines.length; ++lineNo) {
   const line = lines[lineNo].trim();
   if (line === '' || line.startsWith('#')) {
	 continue;
   }
   const m = keywordRE.exec(line);
   if (!m) {
	 continue;
   }
   const [, keyword, unparsedArgs] = m;
   const parts = line.split(/\s+/).slice(1);
   const handler = keywords[keyword];
   if (!handler) {
	 console.warn('unhandled keyword:', keyword);  // eslint-disable-line no-console
	 continue;
   }
   handler(parts, unparsedArgs);
 }
	
	// generating normal data if necessary
	if(webglVertexData[2].length == 0){
		console.log("No normal data.");
		var num = webglVertexData[0].length;
		for(var i=0; i<num; i+=9){
			var vec = v3.normalise(v3.cross(v3.subtractVectors(webglVertexData[0].slice(i+3, i+6), webglVertexData[0].slice(i, i+3)), v3.subtractVectors(webglVertexData[0].slice(i+6, i+9), webglVertexData[0].slice(i, i+3))));
			vec.forEach(value => { webglVertexData[2].push(value); });
			vec.forEach(value => { webglVertexData[2].push(value); });
			vec.forEach(value => { webglVertexData[2].push(value); });
		}
	}
	
	return {
		position: {size: 3, value: webglVertexData[0]},
		textureCoord: {size: 2, value: webglVertexData[1]},
		normal: {size: 3, value: webglVertexData[2]}
	};
}

window.MakeHeightMap = function(cellSize, perlinScale, heightScale, width, height){
	if(perlinScale > cellSize / width/1) perlinScale = cellSize / width/1;
	if(perlinScale > cellSize / height/1) perlinScale = cellSize / height/1;
	var heightMap = [];
	for(var j=0; j<=height; j++){
		var row = []
		for(var i=0; i<=width; i++){
			row.push(heightScale * perlin2(i * perlinScale, j * perlinScale));
		}
		heightMap.push(row);
	}
	return heightMap;
}

window.MakeTerrain = function(cellSize, heightMap, width, height, textureDrawWidth, textureDrawHeight){
	var posValues = [];
	var norValues = [];
	var tcValues = [];
	var x0 = 0;
	var y0 = 0;
	// generating positions and texture coordinates
	for(var i=0; i<width; i++){
		var x = x0 + i*cellSize;
		var tLeft = (x % textureDrawWidth) / textureDrawWidth/1;
		var tRight = tLeft + cellSize / textureDrawWidth/1;
		
		for(var j=0; j<height; j++){
			var y = y0 + j*cellSize;
			var tTop = (y % textureDrawHeight) / textureDrawHeight/1;
			var tBottom = tTop + cellSize / textureDrawHeight/1;
			
			posValues.push(x);				posValues.push(y + cellSize);	posValues.push(heightMap[j+1][i]);
			posValues.push(x);				posValues.push(y);				posValues.push(heightMap[j][i]);
			posValues.push(x + cellSize);	posValues.push(y + cellSize);	posValues.push(heightMap[j+1][i+1]);
			posValues.push(x + cellSize);	posValues.push(y);				posValues.push(heightMap[j][i+1]);
			posValues.push(x + cellSize);	posValues.push(y + cellSize);	posValues.push(heightMap[j+1][i+1]);
			posValues.push(x);				posValues.push(y);				posValues.push(heightMap[j][i]);
			
			tcValues.push(tLeft); tcValues.push(tBottom);
			tcValues.push(tLeft); tcValues.push(tTop);
			tcValues.push(tRight); tcValues.push(tBottom);
			tcValues.push(tRight); tcValues.push(tTop);
			tcValues.push(tRight); tcValues.push(tBottom);
			tcValues.push(tLeft); tcValues.push(tTop);
		}
	}
	// generating normals
	var num = posValues.length;
	for(var i=0; i<num; i+=9){
		var vec = v3.normalise(v3.cross(v3.subtractVectors(posValues.slice(i+3, i+6), posValues.slice(i, i+3)), v3.subtractVectors(posValues.slice(i+6, i+9), posValues.slice(i, i+3))));
		vec.forEach(value => { norValues.push(value); });
		vec.forEach(value => { norValues.push(value); });
		vec.forEach(value => { norValues.push(value); });
	}
	
	return {
		position: {size: 3, value: posValues},
		normal: {size: 3, value: norValues},
		textureCoord: {size: 2, value: tcValues}
	};
}

window.MakePlane = function(textureWidth, textureHeight, width, height, flip=false){
	var posValues = [];
	var norValues = [];
	var tcValues = [];
	var x0 = 0;
	var y0 = 0;
	for(var i=0; i<width; i++){
		var x = x0 + i*textureWidth;
		for(var j=0; j<height; j++){
			var y = y0 + j*textureHeight;
			
			posValues.push(x);					posValues.push(y + textureHeight);	posValues.push(0);
			posValues.push(x);					posValues.push(y);					posValues.push(0);
			posValues.push(x + textureWidth);	posValues.push(y + textureHeight);	posValues.push(0);
			posValues.push(x + textureWidth);	posValues.push(y);					posValues.push(0);
			posValues.push(x + textureWidth);	posValues.push(y + textureHeight);	posValues.push(0);
			posValues.push(x);					posValues.push(y);					posValues.push(0);
			
			norValues.push(0); norValues.push(0); norValues.push(1);
			norValues.push(0); norValues.push(0); norValues.push(1);
			norValues.push(0); norValues.push(0); norValues.push(1);
			norValues.push(0); norValues.push(0); norValues.push(1);
			norValues.push(0); norValues.push(0); norValues.push(1);
			norValues.push(0); norValues.push(0); norValues.push(1);
			
			var flipX = flip ? (i % 2 === 1) : false;
			var flipY = flip ? (j % 2 === 1) : false;
			tcValues.push(flipX ? 1 : 0); tcValues.push(flipY ? 1 : 0);
			tcValues.push(flipX ? 1 : 0); tcValues.push(flipY ? 0 : 1);
			tcValues.push(flipX ? 0 : 1); tcValues.push(flipY ? 1 : 0);
			tcValues.push(flipX ? 0 : 1); tcValues.push(flipY ? 0 : 1);
			tcValues.push(flipX ? 0 : 1); tcValues.push(flipY ? 1 : 0);
			tcValues.push(flipX ? 1 : 0); tcValues.push(flipY ? 0 : 1);
		}
	}
	return {
		position: {size: 3, value: posValues},
		normal: {size: 3, value: norValues},
		textureCoord: {size: 2, value: tcValues}
	};
}

window.MakeCube = function(r){
	return {
		position: {size: 3, value: [-r, -r, -r, // front lower left
									-r, r, -r,
									r, r, -r,
									
									r, -r, -r, // front upper right
									-r, -r, -r,
									r, r, -r,
									
									r, -r, r, // rear lower left
									r, r, r,
									-r, r, r,
									
									-r, -r, r, // rear upper right
									r, -r, r,
									-r, r, r,
									
									-r, -r, r, // left lower left
									-r, r, r,
									-r, r, -r,
									
									-r, -r, -r, // left upper right
									-r, -r, r,
									-r, r, -r,
									
									r, -r, -r, // right lower left
									r, r, -r,
									r, r, r,
									
									r, -r, r, // right upper right
									r, -r, -r,
									r, r, r,
									
									-r, -r, r, // top lower left
									-r, -r, -r,
									r, -r, -r,
									
									r, -r, r, // top upper right
									-r, -r, r,
									r, -r, -r,
									
									-r, r, -r, // bottom lower left
									-r, r, r,
									r, r, r,
									
									r, r, -r, // bottom upper right
									-r, r, -r,
									r, r, r]},
		normal: {size: 3, value: [0, 0, -1,
								  0, 0, -1,
								  0, 0, -1,
								  0, 0, -1,
								  0, 0, -1,
								  0, 0, -1,
								  
								  0, 0, 1,
								  0, 0, 1,
								  0, 0, 1,
								  0, 0, 1,
								  0, 0, 1,
								  0, 0, 1,
								  
								  -1, 0, 0,
								  -1, 0, 0,
								  -1, 0, 0,
								  -1, 0, 0,
								  -1, 0, 0,
								  -1, 0, 0,
								  
								  1, 0, 0,
								  1, 0, 0,
								  1, 0, 0,
								  1, 0, 0,
								  1, 0, 0,
								  1, 0, 0,
								  
								  0, -1, 0,
								  0, -1, 0,
								  0, -1, 0,
								  0, -1, 0,
								  0, -1, 0,
								  0, -1, 0,
								  
								  0, 1, 0,
								  0, 1, 0,
								  0, 1, 0,
								  0, 1, 0,
								  0, 1, 0,
								  0, 1, 0]},
		textureCoord: {size: 2, value: [1, 0,
										1, 1,
										0, 1,
										0, 0,
										1, 0,
										0, 1,
										
										1, 0,
										1, 1,
										0, 1,
										0, 0,
										1, 0,
										0, 1,
										
										1, 0,
										1, 1,
										0, 1,
										0, 0,
										1, 0,
										0, 1,
										
										1, 0,
										1, 1,
										0, 1,
										0, 0,
										1, 0,
										0, 1,
										
										1, 0,
										1, 1,
										0, 1,
										0, 0,
										1, 0,
										0, 1,
										
										1, 0,
										1, 1,
										0, 1,
										0, 0,
										1, 0,
										0, 1]}
	};
}

window.DrawLine = function(context, colour, thickness, pos1, pos2){
	context.strokeStyle = colour;
	context.lineWidth = thickness;
	context.beginPath();
	context.moveTo(pos1.x, pos1.y);
	context.lineTo(pos2.x, pos2.y);
	context.stroke();
}
window.DrawWedge = function(context, colour, position, radius, fraction){
	context.fillStyle = colour;
	context.beginPath();
	context.moveTo(position.x, position.y);
	context.arc(position.x, position.y, radius, - Math.PI * 0.5, -Math.PI * 0.5 + 2 * Math.PI * fraction);
	context.closePath();
	context.fill();
}

function unlockAudio() {
	let sound = new Audio();
	sound.play();
	sound.pause();
	document.body.removeEventListener('click', unlockAudio)
	document.body.removeEventListener('touchstart', unlockAudio)
}
document.body.addEventListener('click', unlockAudio);
document.body.addEventListener('touchstart', unlockAudio);
