#version 120

uniform mat4 u_viewProjection;
uniform vec3 u_lightWorldPos;
uniform mat4 u_viewInverse;
uniform mat4 u_lightProjectionMatrix;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec3 a_colour;
attribute mat4 a_world;
attribute mat4 a_worldInverseTranspose;

varying vec3 v_colour;
varying vec3 v_normal;
varying vec3 v_surfaceToLight;
varying vec3 v_surfaceToView;
varying vec3 v_lightProjection;

void main() {
	vec4 position4 = a_world * vec4(a_position, 1.0);
	vec3 position3 = position4.xyz;
	v_colour = a_colour;
	v_normal = (a_worldInverseTranspose * vec4(a_normal, 0.0)).xyz;;
	v_surfaceToLight = u_lightWorldPos - position3;
	v_surfaceToView = u_viewInverse[3].xyz - position3;
	vec4 lightPosition = u_lightProjectionMatrix * position4;
	v_lightProjection = lightPosition.xyz / lightPosition.w;
	gl_Position = u_viewProjection * position4;
}
