#version 120

attribute vec3 a_translation;

attribute vec3 a_position; // a_position.x, .y between -1.0 & 1.0; a_position.z between 0.0 & 1.0
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

void main() {
	vec3 a_position_translated = a_position + a_translation;
	vec4 a_position4 = vec4(a_position_translated.x, a_position_translated.y, 0.1 + 0.0001*(a_position_translated.z - 1.0), 1.0);
	v_texCoord = a_texCoord;
	gl_Position = a_position4;
}
