#version 120

// It was expressed that some drivers required this next line to function properly
//precision mediump float;

varying vec2 v_texCoord;

uniform sampler2D u_texture;

void main() {
	vec4 diffuseColour = texture2D(u_texture, v_texCoord);
	gl_FragColor = diffuseColour;
}
