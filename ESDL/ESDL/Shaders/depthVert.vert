#version 120

uniform mat4 u_viewProjection;

attribute vec3 a_position;
attribute mat4 a_world;

void main() {
	gl_Position = u_viewProjection * a_world * vec4(a_position, 1.0);
}

