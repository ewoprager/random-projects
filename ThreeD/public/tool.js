import GameObject from "/object.js";
import Animation from "/anim.js";
import {m4} from "/mat4.js";
import {quat} from "/quat.js";

export default class Tool extends GameObject {
	constructor(renderer3d, position, rotation, scale, objFileORbufferData, textureFile, player){
		super(renderer3d, position, rotation, scale, objFileORbufferData, textureFile);
		
		this.player = player;
		
		this.localTranformMatrix = m4.zRotated(m4.translated(this.localTransformMatrix, 25, 5, -160), -1);
		
		this.defaultRotation = quat.identity;
		this.hitRotation = quat.product(this.defaultRotation, quat.makeRotation([-0.3, 0.8, -0.3], 0.7));
		
		this.animatedMatrix = this.localTranformMatrix;
		
		this.animation = new Animation([[[0], [-0.2], [-0.4], [-0.3], [-0.05], [0.4], [1], [0.9], [0.8], [0.4], [0.267], [0.133]]]);
		this.animation.triggers[1].push({index: 6, callBack: this.Swing.bind(this)});
		
		this.mouseDown = false;
		
		document.addEventListener("mousedown", event => {
			if(event.button != 0) return;
			this.mouseDown = true;
		});
		document.addEventListener("mouseup", event => {
			if(event.button != 0) return;
			this.mouseDown = false;
		});
		
		window.UpdateMe(this);
		
		// audio
		this.dinkSound = document.getElementById("sound_dink");
	}
	
	Swing(){
		if(this.renderer3d.objectInteracting === -1) return;
		this.dinkSound.currentTime = 0;
		this.dinkSound.play();
		console.log("dink");
	}
	
	Update(deltaTime){
		if(this.mouseDown){
			if(this.animation.currentAnim === 0){
				this.animation.loop = false;
				this.animation.Play(1);
			}
		}
		
		var animValues = this.animation.GetValues();
		var playerAnimValue = 4 * this.player.animation.GetValues()[1];
		var animatedLocalTransformMatrix = m4.translated(this.localTranformMatrix, 0, playerAnimValue, 0);
		this.animatedMatrix = m4.multiply(quat.matrix(quat.slerp(this.defaultRotation, this.hitRotation, animValues[0])), animatedLocalTransformMatrix);
		
		this.transformMatrix = m4.multiply(this.player.transformMatrix, this.animatedMatrix);
	}
	
	Destroy(){
		super.Destroy();
		this.renderer3d.DontCameraTransformMe(this);
		window.DontUpdateMe(this);
	}
}
