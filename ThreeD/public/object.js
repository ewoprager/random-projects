import GL from "/gl.js";
import {m4} from "/mat4.js";

export default class GameObject {
	constructor(renderer3d, position, euler, scale, objFileORbufferData, textureFile) {
		this.renderer3d = renderer3d;
		
		this.localTransformMatrix = m4.translated(m4.scale(m4.euler(m4.identity, euler), scale, scale, scale), position[0], position[1], position[2])
		this.transformMatrix = this.localTransformMatrix;
		
		if(objFileORbufferData === false){
			this.rendered = false;
		} else {
			if(typeof objFileORbufferData === "string"){
				this.objFile = objFileORbufferData;
				this.bufferData = window.MakeCube(1);
				this.objFileORbufferData = false;
			} else {
				this.bufferData = objFileORbufferData;
				this.objFileORbufferData = true;
			}
			
			this.materialData = {
				colorMultiplier: [1, 1, 1, 1],
				diffuseTextureUnit: [1, 1, 1],
				specularColor: [0, 0, 0, 1],
				shininess: 100, // 0 to 500, lower is more shiny
				specularFactor: 0.5 // 0 to 1
			};
			
			// Asynchronously load an image
			this.image = new Image();
			this.image.src = textureFile;
			this.image.object = this;
			this.image.addEventListener('load', function() { this.object.GetModelData(); });
		}
	}
	
	async GetModelData(){
		// image is ready,
		//console.log(this.image);
		// get buffer data
		if(!this.objFileORbufferData){
			const response = await fetch(this.objFile);
			const text = await response.text();
			this.bufferData = window.parseOBJ(text);
		}
		
		// buffer data and image are both ready, so add this to be rendered
		this.renderer3d.RenderMe(this, this.image);
		this.rendered = true;
	}
	
	Destroy(){
		if(this.rendered) this.renderer3d.DontRenderMe(this);
	}
}
