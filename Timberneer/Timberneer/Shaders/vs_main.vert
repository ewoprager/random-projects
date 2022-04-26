#version 120

uniform mat4 u_viewProjection;
uniform vec3 u_lightWorldPos;
uniform mat4 u_viewInverse;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;
attribute mat4 a_world;
attribute mat4 a_worldInverseTranspose;

varying vec2 v_texCoord;
varying vec3 v_normal;
varying vec3 v_surfaceToLight;

varying vec3 v_position;
varying vec3 v_camPos;
varying vec3 v_lightWorldPos;

void main() {
	vec4 a_position4 = a_world * vec4(a_position, 1.0);
	
	v_position = a_position4.xyz;
	v_camPos = u_viewInverse[3].xyz;
	v_lightWorldPos = u_lightWorldPos;
	
	v_texCoord = a_texCoord;
	v_normal = (a_worldInverseTranspose * vec4(a_normal, 0.0)).xyz;
	v_surfaceToLight = u_lightWorldPos - v_position;
	
	gl_Position = u_viewProjection * a_position4;
}
