#version 120

uniform mat4 u_viewProjection;
uniform vec3 u_lightWorldPos;
uniform mat4 u_world;
uniform mat4 u_viewInverse;
uniform mat4 u_worldInverseTranspose;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;

varying vec4 v_position;
varying vec2 v_texCoord;
varying vec3 v_normal;
varying vec3 v_surfaceToLight;
varying vec3 v_surfaceToView;

void main() {
	vec4 a_position4 = vec4(a_position, 1.0);
	v_texCoord = a_texCoord;
	v_position = (u_viewProjection * u_world * a_position4);
	v_normal = (u_worldInverseTranspose * vec4(a_normal, 0)).xyz;
	v_surfaceToLight = u_lightWorldPos - (u_world * a_position4).xyz;
	v_surfaceToView = (u_viewInverse[3] - (u_world * a_position4)).xyz;
	gl_Position = v_position;
}
