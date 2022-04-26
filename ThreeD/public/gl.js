import {m4} from "/mat4.js";

export default class GL {
	constructor(canvas){
		this.gl = canvas.getContext("webgl");
		//console.log(gl);
		
		this.renderObjects = [];
		this.interactableObjects = [];
		this.objectInteracting = -1;
		
		var vertexShader = this.CreateShader(this.gl.VERTEX_SHADER, document.getElementById("vertex-shader-3d").text);
		var fragmentShader = this.CreateShader(this.gl.FRAGMENT_SHADER, document.getElementById("fragment-shader-3d").text);
		this.program = this.CreateProgram(vertexShader, fragmentShader);
		// Tell it to use our program (pair of shaders)
		this.gl.useProgram(this.program);
		this.gl.enable(this.gl.CULL_FACE); // cull faces
		this.gl.enable(this.gl.DEPTH_TEST); // draw things in z-order
		this.gl.clearColor(0, 0, 0, 0); // setting the clear color
		
		
		// picking shaders
		this.oldPickNdx = -1;
		this.frameCount = 0;
		var pickVertexShader = this.CreateShader(this.gl.VERTEX_SHADER, document.getElementById("pick-vertex-shader").text);
		var pickFragmentShader = this.CreateShader(this.gl.FRAGMENT_SHADER,  document.getElementById("pick-fragment-shader").text);
		this.pickProgram = this.CreateProgram(pickVertexShader, pickFragmentShader);
		// Create a texture to render to
		const targetTexture = this.gl.createTexture();
		this.gl.bindTexture(this.gl.TEXTURE_2D, targetTexture);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.CLAMP_TO_EDGE);
		this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.CLAMP_TO_EDGE);
		// create a depth renderbuffer
		const depthBuffer = this.gl.createRenderbuffer();
		this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, depthBuffer);
		// setting frame buffer attachment sizes
		// define size and format of level 0
		var level = 0;
		var internalFormat = this.gl.RGBA;
		var width = 1;
		var height = 1;
		var border = 0;
		var format = this.gl.RGBA;
		var type = this.gl.UNSIGNED_BYTE;
		var data = null;
		this.gl.texImage2D(this.gl.TEXTURE_2D, level, internalFormat, width, height, border, format, type, data);
		this.gl.bindRenderbuffer(this.gl.RENDERBUFFER, depthBuffer);
		this.gl.renderbufferStorage(this.gl.RENDERBUFFER, this.gl.DEPTH_COMPONENT16, width, height);
		// Create and bind the framebuffer
		this.frameBuffer = this.gl.createFramebuffer();
		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.frameBuffer);
		// attach the texture as the first color attachment
		var attachmentPoint = this.gl.COLOR_ATTACHMENT0;
		var level = 0;
		this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, attachmentPoint, this.gl.TEXTURE_2D, targetTexture, level);
		// make a depth buffer and the same size as the targetTexture
		this.gl.framebufferRenderbuffer(this.gl.FRAMEBUFFER, this.gl.DEPTH_ATTACHMENT, this.gl.RENDERBUFFER, depthBuffer);
		// attribute locations
		this.pickAttributeLocations = {
			position: this.gl.getAttribLocation(this.pickProgram, "a_position")
		};
		// uniform locations
		this.pickUniformLocations = {
			id: this.gl.getUniformLocation(this.pickProgram, "u_id"),
			viewProjection: this.gl.getUniformLocation(this.pickProgram, "u_viewProjection"),
			world: this.gl.getUniformLocation(this.pickProgram, "u_world")
		};
		//
		
		
		// attribute locations
		this.attributeLocations = {
			position: this.gl.getAttribLocation(this.program, "a_position"),
			normal: this.gl.getAttribLocation(this.program, "a_normal"),
			textureCoord: this.gl.getAttribLocation(this.program, "a_texcoord")
		};
		Object.values(this.attributeLocations).forEach(location => {
			this.gl.enableVertexAttribArray(location); // should techincally be in Render()?
		});
		// uniform locations
		this.uniformLocations = {
			lightWorldPos: this.gl.getUniformLocation(this.program, "u_lightWorldPos"), // const, set just below here
			viewInverse: this.gl.getUniformLocation(this.program, "u_viewInverse"), // variable but global, set at beginning of Render() equal to 'cameraMatrix'
			lightColor: this.gl.getUniformLocation(this.program, "u_lightColor"), // const, set just below here
			// variable and per object, set within Render() per object
			viewProjection: this.gl.getUniformLocation(this.program, "u_viewProjection"),
			world: this.gl.getUniformLocation(this.program, "u_world"),
			worldInverseTranspose: this.gl.getUniformLocation(this.program, "u_worldInverseTranspose"),
			colorMult: this.gl.getUniformLocation(this.program, "u_colorMult"),
			diffuse: this.gl.getUniformLocation(this.program, "u_diffuse"),
			specular: this.gl.getUniformLocation(this.program, "u_specular"),
			shininess: this.gl.getUniformLocation(this.program, "u_shininess"),
			specularFactor: this.gl.getUniformLocation(this.program, "u_specularFactor")
		};
		// constant uniforms
		this.gl.uniform3fv(this.uniformLocations.lightWorldPos, [-10000, -10000, 10000]);
		this.gl.uniform4fv(this.uniformLocations.lightColor, [1, 1, 1, 1]);
		
		// attribute buffers
		this.attributeBuffers = {
			position: this.gl.createBuffer(),
			normal: this.gl.createBuffer(),
			textureCoord: this.gl.createBuffer()
		};
		
		// Calculate the projection matrix
		var fieldOfViewRadians = 0.77;
		var aspect = this.gl.canvas.clientWidth / this.gl.canvas.clientHeight/1;
		this.zNear = 1;
		this.zFar = 15000;
		this.projectionMatrix = m4.perspective(fieldOfViewRadians, aspect, this.zNear, this.zFar);
		
		// variables for pick projection matrix
		this.ppm_subWidth = 1 / this.gl.canvas.width/1;
		this.ppm_subHeight = 1 / this.gl.canvas.height/1;
		// compute the rectangle the near plane of our frustum covers
		var top = Math.tan(fieldOfViewRadians * 0.5) * this.zNear;
		this.ppm_bottom = -top;
		this.ppm_left = aspect * this.ppm_bottom;
		var right = aspect * top;
		this.ppm_width = Math.abs(right - this.ppm_left);
		this.ppm_height = Math.abs(top - this.ppm_bottom);
	}
	
	Render(cameraMatrix){
		// Compute the matrices
		// Compute a matrix for the camera. m4.identity would be at (0, 0, 0), looking in negative z-direction
		// [0, -1, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 0, 0, 1] would be at (0, 0, 0), looking in positive x-direction
		
		// Make a view matrix from the camera matrix.
		var viewMatrix = m4.inverse(cameraMatrix);
		
		// drawing to picking texture
		// compute the portion of the near plane covers the 1 pixel under the mouse and making projection matrix
		/* // for picking with mouse
		var pixelX = window.mousePos.x * this.gl.canvas.width / this.gl.canvas.clientWidth/1;
		var pixelY = this.gl.canvas.height - window.mousePos.y * this.gl.canvas.height / this.gl.canvas.clientHeight/1 - 1;
		 */
		// for picking with centre of screen
		var pixelX = this.gl.canvas.width*0.5;
		var pixelY = this.gl.canvas.height*0.5;
		
		var subLeft = this.ppm_left + pixelX * this.ppm_width / this.gl.canvas.width/1;
		var subBottom = this.ppm_bottom + pixelY * this.ppm_height / this.gl.canvas.height/1;
		// make a frustum for that 1 pixel
		var pickProjectionMatrix = m4.frustum(subLeft, subLeft + this.ppm_subWidth, subBottom, subBottom + this.ppm_subHeight, this.zNear, this.zFar);
		var viewProjectionMatrix = m4.multiply(pickProjectionMatrix, viewMatrix);
		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, this.frameBuffer);
		this.gl.viewport(0, 0, 1, 1);
		this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);
		var id = 1;
		this.gl.useProgram(this.pickProgram);
		// global uniforms
		this.gl.uniformMatrix4fv(this.pickUniformLocations.viewProjection, false, viewProjectionMatrix);
		// drawing objects to pick texture
		this.interactableObjects.forEach(obj => {
			// object data
			var attributeValues = obj.bufferData;
			
			// define object specific uniforms
			// transformation matrix for the object
			var worldMatrix = obj.transformMatrix;
			
			//var worldViewProjectionMatrix = m4.multiply(viewProjectionMatrix, worldMatrix);
			var worldInverseTransposeMatrix = m4.transpose(m4.inverse(worldMatrix));
			
			// set object specific uniforms
			this.gl.uniformMatrix4fv(this.pickUniformLocations.world, false, worldMatrix);
			this.gl.uniform4fv(this.pickUniformLocations.id, [((id >>  0) & 0xFF) / 0xFF/1,
															  ((id >>  8) & 0xFF) / 0xFF/1,
															  ((id >> 16) & 0xFF) / 0xFF/1,
															  ((id >> 24) & 0xFF) / 0xFF/1]);
			
			// set the attributes
			Object.keys(this.pickAttributeLocations).forEach(key => {
				// Bind the buffer.
				this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.attributeBuffers[key]);
				// buffer the data
				this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(attributeValues[key].value), this.gl.STATIC_DRAW);
				// Tell the texcoord attribute how to get data out of texcoordBuffer (ARRAY_BUFFER)
				var size = attributeValues[key].size;	// no. of components per iteration
				this.gl.vertexAttribPointer(this.pickAttributeLocations[key], size, this.gl.FLOAT, false, 0, 0);
			});
			
			// draw
			var primitiveType = this.gl.TRIANGLES;
			var offset = 0;
			var count = attributeValues.position.value.length / 3/1;
			this.gl.drawArrays(primitiveType, offset, count);
			
			id++;
		});
		// ------ Figure out what pixel is under the mouse and read it
		const data = new Uint8Array(4);
		this.gl.readPixels(0,            // x
						   0,            // y
						   1,                 // width
						   1,                 // height
						   this.gl.RGBA,           // format
						   this.gl.UNSIGNED_BYTE,  // type
						   data);             // typed array to hold result
		var id = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
		this.objectInteracting = id - 1;
		/* make the interacting object flash
		// restore the object's color
		if(this.oldPickNdx >= 0){
			const object = this.interactableObjects[this.oldPickNdx];
			object.materialData.colorMultiplier = this.oldPickColor;
			this.oldPickNdx = -1;
		}
		// highlight object under mouse
		if(id > 0){
			const pickNdx = id - 1;
			this.oldPickNdx = pickNdx;
			const object = this.interactableObjects[pickNdx];
			this.oldPickColor = object.materialData.colorMultiplier;
			object.materialData.colorMultiplier = (this.frameCount & 0x8) ? [1, 0, 0, 1] : [1, 1, 0, 1];
		}
		*/
		
		
		// drawing to canvas:
		// Compute a view projection matrix
		var viewProjectionMatrix = m4.multiply(this.projectionMatrix, viewMatrix);
		this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
		this.gl.viewport(0, 0, this.gl.canvas.width, this.gl.canvas.height);
		this.gl.useProgram(this.program);
		// global uniforms
		this.gl.uniformMatrix4fv(this.uniformLocations.viewInverse, false, cameraMatrix);
		this.gl.uniformMatrix4fv(this.uniformLocations.viewProjection, false, viewProjectionMatrix);
		// drawing objects to canvas
		this.renderObjects.forEach(obj => {
			// object data
			var attributeValues = obj.bufferData;
			
			// define object specific uniforms
			// transformation matrix for the object
			var worldMatrix = obj.transformMatrix;
			var worldInverseTransposeMatrix = m4.transpose(m4.inverse(worldMatrix));
			// material uniforms
			var materialValues = obj.materialData;
			
			// binding the object's texture
			this.gl.bindTexture(this.gl.TEXTURE_2D, materialValues.texture);
			
			// set object specific uniforms
			//this.gl.uniformMatrix4fv(this.uniformLocations.viewProjection, false, viewProjectionMatrix);
			this.gl.uniformMatrix4fv(this.uniformLocations.world, false, worldMatrix);
			this.gl.uniformMatrix4fv(this.uniformLocations.worldInverseTranspose, false, worldInverseTransposeMatrix);
			this.gl.uniform4fv(this.uniformLocations.colorMult, materialValues.colorMultiplier);
			this.gl.uniform1i(this.uniformLocations.diffuse, materialValues.diffuseTextureUnit);
			this.gl.uniform4fv(this.uniformLocations.specular, materialValues.specularColor);
			this.gl.uniform1f(this.uniformLocations.shininess, materialValues.shininess);
			this.gl.uniform1f(this.uniformLocations.specularFactor, materialValues.specularFactor);
			
			// set the attributes
			Object.keys(this.attributeLocations).forEach(key => {
				// Bind the buffer.
				this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.attributeBuffers[key]);
				// buffer the data
				this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(attributeValues[key].value), this.gl.STATIC_DRAW);
				// Tell the texcoord attribute how to get data out of texcoordBuffer (ARRAY_BUFFER)
				var size = attributeValues[key].size;	// no. of components per iteration
				this.gl.vertexAttribPointer(this.attributeLocations[key], size, this.gl.FLOAT, false, 0, 0);
			});
			
			// draw
			var primitiveType = this.gl.TRIANGLES;
			var offset = 0;
			var count = attributeValues.position.value.length / 3/1;
			this.gl.drawArrays(primitiveType, offset, count);
		});
		
		this.frameCount++;
	}
	
	CreateShader(type, source){
		var shader = this.gl.createShader(type);
		this.gl.shaderSource(shader, source);
		this.gl.compileShader(shader);
		var success = this.gl.getShaderParameter(shader, this.gl.COMPILE_STATUS);
		if(success) return shader;
		
		console.log(this.gl.getShaderInfoLog(shader));
		this.gl.deleteShader(shader);
	}

	CreateProgram(vertexShader, fragmentShader){
		var program = this.gl.createProgram();
		this.gl.attachShader(program, vertexShader);
		this.gl.attachShader(program, fragmentShader);
		this.gl.linkProgram(program);
		var success = this.gl.getProgramParameter(program, this.gl.LINK_STATUS);
		if(success) return program;
		
		console.log(this.gl.getProgramInfoLog(program));
		this.gl.deleteProgram(program);
	}
	
	RenderMe(object, image){
		this.renderObjects.push(object);
		
		// creating texture for the object
		object.materialData.texture = this.gl.createTexture();
		this.gl.bindTexture(this.gl.TEXTURE_2D, object.materialData.texture);
		this.gl.texImage2D(this.gl.TEXTURE_2D, 0, this.gl.RGBA, this.gl.RGBA, this.gl.UNSIGNED_BYTE, image);
		this.gl.generateMipmap(this.gl.TEXTURE_2D);
	}
	DontRenderMe(object){ this.renderObjects.splice(this.renderObjects.indexOf(object), 1); }
	InteractableMe(object){ this.interactableObjects.push(object); }
	DontInteractableMe(object){ this.interactableObjects.splice(this.interactableObjects.indexOf(object), 1); }
}
