import GameObject from "/object.js";
import TestObject from "/testObject.js";
import Player from "/player.js";
import Tool from "/tool.js";
import Tree from "/tree.js";

export default class World {
	constructor(renderer3d){
		this.renderer3d = renderer3d;
		
		this.cellSize = 75;
		var perlinScale = 0.03;
		var heightScale = 700;
		this.width = 150;
		this.height = 150;
		this.heightMap = window.MakeHeightMap(this.cellSize, perlinScale, heightScale, this.width, this.height);
		this.terrain = new GameObject(this.renderer3d, [0, 0, 0], [0, 0, 0], 1, window.MakeTerrain(this.cellSize, this.heightMap, this.width, this.height, 512, 512), "/Assets/grass 3.jpg");
		
		this.player = new Player(this.renderer3d, this, [0, 0, 300], [0, 0, 0], 1, false);
		
		new Tool(this.renderer3d, [0, 0, 0], [0, 0, 0], 20, "/Assets/BattleAxe1.obj", "/Assets/BattleAxe1_Color1.jpg", this.player);
		
		new TestObject(this.renderer3d, [300, 0, 100], [1.57, 0, 0], 20, "/Assets/part of the helmet _Low Poly_OBJ.obj", "/Assets/Tileable_metal_scratch_texture.jpg");
		
		// trees
		/*
		var fac = 10;
		this.treeMap = window.MakeHeightMap(this.cellSize*fac, perlinScale*2, heightScale, this.width/fac, this.height/fac);
		for(var j=0; j<this.height/fac; j++){
			for(var i=0; i<this.width/fac; i++){
				if(this.treeMap[j][i] > 0.5){
					var x = i*this.cellSize*fac;
					var y = j*this.cellSize*fac;
					new Tree(this.renderer3d, [x, y, this.GetHeight(x, y)], [0, 0, 0], 100, "/Assets/Spruce.obj", "/Assets/Spruce branches.png");
				}
			}
		}
		 */
	}
	
	GetHeight(x, y){
		var i = Math.floor(x / this.cellSize/1);
		var j = Math.floor(y / this.cellSize/1);
		if(i < 0) return 0;
		if(i > this.width-1) return 0;
		if(j < 0) return 0;
		if(j > this.height-1) return 0;
		var dx = x - i * this.cellSize;
		var dy = y - j * this.cellSize;
		var h00 = this.heightMap[j][i];
		var h10 = this.heightMap[j+1][i];
		var h01 = this.heightMap[j][i+1];
		var h11 = this.heightMap[j+1][i+1];
		var xLerp0 = h00 + (h01 - h00) * dx / this.cellSize/1;
		var xLerp1 = h10 + (h11 - h10) * dx / this.cellSize/1;
		return xLerp0 + (xLerp1 - xLerp0) * dy / this.cellSize/1;
	}
}
