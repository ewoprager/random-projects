export default class Animation {
	constructor(animations){
		this.animations = animations;
		
		// default animation
		var defaultAnim = []; var defaultValues = [];
		for(var i=0; i<this.animations[0][0].length; i++) defaultValues.push(0);
		defaultAnim.push(defaultValues);
		this.animations.unshift(defaultAnim);
		
		this.triggers = [];
		for(var i=0; i<this.animations.length; i++) this.triggers.push([]);
		this.lastIndex = 0;
		this.lastAnim = 0;
		
		this.currentAnim = 0;
		this.time = 0;
		// anim transitions
		this.transitionOverlap = 0.25; // seconds
		this.transitioning = false;
		this.oldTime = 0;
		this.oldAnim = 0;
		
		this.loop = true;
		this.revertTo = 0;
		
		window.UpdateMe(this);
	}
	
	VectorLerp(vA, vB, frac){
		var ret = [];
		var nA = vA.length;
		var nB = vB.length;
		var i;
		var max = (nA < nB ? nA : nB);
		for(i=0; i<max; i++){
			ret.push(vA[i] + frac * (vB[i] - vA[i]));
		}
		var vL = (nA > nB ? vA : vB);
		while(i < vL.length){
			ret.push(vL[i]);
			i++;
			console.log("Warning: changing animation index can lead to ban transistioning.");
		}
		return ret;
	}
	
	AnimIndex(time, anim, fps){
		return Math.floor(time * fps) % this.animations[anim].length;
	}
	
	AnimLerp(time, anim, fps){
		if(!this.animations[anim].length) return 0;
		if(this.animations[anim].length === 1) return this.animations[anim][0];
		var i = Math.floor(time * fps);
		var d = time - i/fps/1;
		i = i % this.animations[anim].length;
		return this.VectorLerp(this.animations[anim][i], this.animations[anim][(i+1) % this.animations[anim].length], d * fps);
	}
	
	GetValues(fps=24){
		if(!this.animations[this.currentAnim].length) return 0;
		if(this.transitioning){
			if(this.time >= this.transitionOverlap){
				this.transitioning = false;
			} else {
				var valuesOld = this.AnimLerp(this.oldTime, this.oldAnim, fps);
				var valuesNew = this.AnimLerp(this.time, this.currentAnim, fps);
				return this.VectorLerp(valuesOld, valuesNew, this.time / this.transitionOverlap/1);
			}
		}
		if(!this.loop){
			if(this.time*fps > this.animations[this.currentAnim].length){
				this.time = 0;
				this.currentAnim = this.revertTo;
				this.loop = true;
			}
		}
		
		// deal with triggers
		var index = this.AnimIndex(this.time, this.currentAnim, fps);
		this.triggers[this.currentAnim].forEach(function(trigger){
			if(trigger.index === -1) return;
			if(this.lastAnim != this.currentAnim) return;
			//console.log("trigger :", trigger, "last index: ", this.lastIndex, "index: ", index);
			if(trigger.index > this.lastIndex && trigger.index <= index) trigger.callBack();
		}, this);
		this.lastIndex = index;
		this.lastAnim = this.currentAnim;
		
		return this.AnimLerp(this.time, this.currentAnim, fps);
	}
	
	StartTransition(newAnim){
		this.oldTime = this.time;
		this.oldAnim = this.currentAnim;
		this.time = 0;
		this.currentAnim = newAnim;
		this.transitioning = true;
	}
	Play(newAnim){
		this.time = 0;
		this.currentAnim = newAnim;
		this.transitioning = false;
	}
	
	Update(deltaTime){
		this.time += deltaTime;
		this.oldTime += deltaTime;
	}
	
	Destroy(){
		window.DontUpdateMe(this);
	}
}
