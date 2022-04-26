import GameObject from "/object.js";
import Animation from "/anim.js";
import {m4} from "/mat4.js";
import {linAlg} from "/linAlg.js";

export default class Player extends GameObject {
	constructor(renderer3d, world, position, rotation, scale, objFileORbufferData, textureFile){
		super(renderer3d, position, rotation, scale, objFileORbufferData, textureFile);
		
		this.world = world;
		this.height = position[2];
		
		this.dPressed = false;
		this.wPressed = false;
		this.aPressed = false;
		this.sPressed = false;
		this.ePressed = false;
		this.qPressed = false;
		
		this.speed = 1200;
		
		this.viewSensitivity = {x: 0.003, y: 0.003};
		
		document.addEventListener("keydown", event => {
			switch(event.keyCode){
				case 68: this.dPressed = true; break; // D
				case 87: this.wPressed = true; break; // W
				case 65: this.aPressed = true; break; // A
				case 83: this.sPressed = true; break; // S
				case 69: this.ePressed = true; break; // E
				case 81: this.qPressed = true; break; // Q
				default: console.log(event.keyCode); break;
			}
		});
		document.addEventListener("keyup", event => {
			switch(event.keyCode){
				case 68: this.dPressed = false; break; // D
				case 87: this.wPressed = false; break; // W
				case 65: this.aPressed = false; break; // A
				case 83: this.sPressed = false; break; // S
				case 69: this.ePressed = false; break; // E
				case 81: this.qPressed = false; break; // Q
				default:  break;
			}
		});
		document.addEventListener("mousedown", event => {
			if(event.button != 2) return;
			this.eulerAngles = [0, 0, 0];
		});
		// pointer lock
		this.mouseDeltaX = 0;
		this.mouseDeltaY = 0;
		window.canvas.requestPointerLock = window.canvas.requestPointerLock || window.canvas.mozRequestPointerLock;
		document.exitPointerLock = document.exitPointerLock || document.mozExitPointerLock;
		document.addEventListener('pointerlockerror', this.LockError);
		document.addEventListener('mozpointerlockerror', this.LockError);
		document.addEventListener('pointerlockchange', this.LockChangeAlert.bind(this));
		document.addEventListener('mozpointerlockchange', this.LockChangeAlert.bind(this));
		window.canvas.onclick = function() {
			window.canvas.requestPointerLock();
		};
		
		/* mouse smoothing
		this.smoothN = 5;
		this.tPrevs = []; this.xPrevs = []; this.yPrevs = [];
		for(var i=0; i<this.smoothN; i++){
			this.tPrevs.push(0);
			this.xPrevs.push(0);
			this.yPrevs.push(0);
		}
		 */
		
		this.translationMatrix = m4.translation(position[0], position[1], position[2]);
		this.eulerAngles = [0, 0, 0];
		
		// animations: 0 is default, 1 is bobbing, 2 is breathing. index 0 is for the player movement, index 1 is for the tool movement
		this.animation = new Animation([
										[[0, -0.707], [0.383, -0.383], [0.707, 0], [0.924, 0.383], [1, 0.707], [0.924, 0.924], [0.707, 1], [0.383, 0.924], [0, 0.707], [-0.383, 0.383], [-0.707, 0], [-0.924, -0.383], [-1, -0.707], [-0.924, -0.924], [-0.707, -1], [-0.383, -0.924]],
										[[0, 0], [0.15, 0.075], [0.25, 0.125], [0.35, 0.175], [0.4, 0.2], [0.45, 0.225], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.5, 0.25], [0.45, 0.225], [0.4, 0.2], [0.35, 0.175], [0.25, 0.125], [0.15, 0.075], [0, 0], [-0.15, -0.075], [-0.25, -0.125], [-0.35, -0.175], [-0.4, -0.2], [-0.45, -0.225], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.5, -0.25], [-0.45, -0.225], [-0.4, -0.2], [-0.35, -0.175], [-0.25, -0.125], [-0.15, -0.075]]
										]);
		this.animation.loop = true;
		this.animation.Play(2);
		this.animScale = 25;
		
		window.UpdateMe(this);
	}
	
	/* for quadratic smoothing
	QuadraticCoefficients(t){
		return [1, t, t*t];
	}
	ValueQuadratic(coefficients, t){
		return coefficients[0] + t*coefficients[1] + t*t*coefficients[2];
	}
	MakeAMatrix(times){
		var A = [];
		times.forEach(time => {
			A.push(this.QuadraticCoefficients(time));
		});
		return A;
	}
	*/
	/* for smoothing
	ShiftVector(vector, newValue){
		vector.shift();
		vector.push(newValue);
	}
	 */
	
	Update(deltaTime){
		var tNow = (Date.now() + 1)/1000/1;
		// mouse movement
		
		// mouse smoothing
		/*this.ShiftVector(this.tPrevs, tNow);
		this.ShiftVector(this.xPrevs, this.xPrevs[this.smoothN-1]);
		this.ShiftVector(this.yPrevs, this.yPrevs[this.smoothN-1]);*/
		/* quadratic
		var A = this.MakeAMatrix(this.tPrevs);
		var mouseDeltaX = this.ValueQuadratic(linAlg.leastSquares(A, this.xPrevs), tNow);
		var mouseDeltaY = this.ValueQuadratic(linAlg.leastSquares(A, this.yPrevs), tNow);
		if(!mouseDeltaX) mouseDeltaX = 0; if(!mouseDeltaY) mouseDeltaY = 0;
		 */
		/* constant
		var mouseDeltaX = 0;
		this.xPrevs.forEach(x => { mouseDeltaX += x; });
		mouseDeltaX /= this.smoothN/1;
		var mouseDeltaY = 0;
		this.yPrevs.forEach(y => { mouseDeltaY += y; });
		mouseDeltaY /= this.smoothN/1;
		 */
		
		// yaw, pitch and roll (euler angles)
		this.eulerAngles[0] += - this.viewSensitivity.x * this.mouseDeltaX; // yaw
		this.eulerAngles[1] += this.viewSensitivity.y * this.mouseDeltaY; // pitch
		this.mouseDeltaX = 0; this.mouseDeltaY = 0;
		//this.eulerAngles[0] = - this.viewSensitivity.x * mouseDeltaX; // yaw, for smoothing
		//this.eulerAngles[1] = this.viewSensitivity.y * mouseDeltaY; // pitch, for smoothing
		this.eulerAngles[2] = 0.39 * (this.ePressed - this.qPressed); // roll
		
		// movement
		var mag = this.speed * deltaTime;
		var forward = this.wPressed - this.sPressed;
		var left = this.aPressed - this.dPressed;
		if(left && forward) mag *= 0.707;
		var c = mag * Math.cos(this.eulerAngles[0]);
		var s = mag * Math.sin(this.eulerAngles[0]);
		
		// bobbing animation
		if(left || forward){
			if(this.animation.currentAnim != 1) this.animation.StartTransition(1); // bobbing animation
		} else {
			if(this.animation.currentAnim != 2) this.animation.StartTransition(2); // breathing animation
		}
		
		// getting animation values
		var animValues = this.animation.GetValues();
		 
		// moving, and using animation values
		if(left){
			this.translationMatrix = m4.translated(this.translationMatrix, -s * left, c * left, 0);
			this.eulerAngles[2] += - 0.01 * animValues[0];
		}
		if(forward){
			this.translationMatrix = m4.translated(this.translationMatrix, c * forward, s * forward, 0);
			this.eulerAngles[1] += - 0.003 * animValues[0];
		}
		
		this.translationMatrix[14] = this.animScale * animValues[0] + this.height + this.world.GetHeight(this.translationMatrix[12], this.translationMatrix[13]);
		
		// euler matrix
		this.eulerMatrix = m4.eulered([0, -1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 0, 0, 1], this.eulerAngles);
		
		this.transformMatrix = m4.multiply(this.translationMatrix, this.eulerMatrix);
	}
	
	LockChangeAlert(event){
		if(document.pointerLockElement === canvas || document.mozPointerLockElement === canvas){
			document.addEventListener("mousemove", this.MouseMove.bind(this));
		} else {
			document.removeEventListener("mousemove", this.MouseMove.bind(this));
		}
	}
	LockError(event){ console.log(event); }
	MouseMove(event){
		this.mouseDeltaX += event.movementX;
		this.mouseDeltaY += event.movementY;
		/* for smoothing
		this.ShiftVector(this.tPrevs, Date.now()/1000/1);
		this.ShiftVector(this.xPrevs, this.xPrevs[this.smoothN-1] + event.movementX);
		this.ShiftVector(this.yPrevs, this.yPrevs[this.smoothN-1] + event.movementY);
		 */
	}
	Destroy(){
		super.Destroy();
		window.DontUpdateMe(this);
	}
}
