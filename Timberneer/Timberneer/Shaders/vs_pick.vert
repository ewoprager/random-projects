#version 120

attribute vec3 a_position;
attribute mat4 a_world;

uniform mat4 u_viewProjection;

varying vec3 v_position;

void main() {
	vec4 world_position = a_world * vec4(a_position, 1.0);
	v_position = world_position.xyz;
	gl_Position = u_viewProjection * world_position;
}
