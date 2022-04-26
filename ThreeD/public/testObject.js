import GameObject from "/object.js";
import {m4} from "/mat4.js";

export default class TestObject extends GameObject {
	constructor(renderer3d, position, rotation, scale, objFileORbufferData, textureFile){
		super(renderer3d, position, rotation, scale, objFileORbufferData, textureFile);
		
		renderer3d.InteractableMe(this);
	}
	
	Destroy(){
		super.Destroy();
		this.renderer3d.DontInteractableMe(this);
	}
}
